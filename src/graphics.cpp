#include <cassert>
#include <iostream>
#include "graphics.hpp"

Graphics::Graphics(IMemory& memory, IInterruptHandler& interruptHandler)
    : _screenData(144, {160, {0xff,0xff,0xff}}),
      _memory(memory),
      _interruptHandler(interruptHandler){};

//////////////////////////////////////////////////////////////////
std::vector<std::vector<RGB>> const & Graphics::getScreenData()
{
    return _screenData;
}
//////////////////////////////////////////////////////////////////

void Graphics::resetScreen()
{
    // TODO duplication might cause issue in the future
    // use screenData and change the value (like _screnData.fill())
    std::vector<std::vector<RGB>> newScreenData(144, {160, {0xff,0xff,0xff}});
    _screenData = newScreenData;
}

//////////////////////////////////////////////////////////////////

bool Graphics::isLCDEnabled()
{
    return true; // TODO
    std::bitset<8> lcdControlRegister(_memory.readInMemory(_LCDControlRegister));
    return lcdControlRegister.test(static_cast<uint8_t>(LCDCONTROLREG::LCDDISPLAY));
}

//////////////////////////////////////////////////////////////////


uint8_t Graphics::getLCDMode() const
{
    std::bitset<8> lcdStatus(_memory.readInMemory(_LCDStatusAdress));
    std::bitset<8> toAnd(0x03);
    std::bitset<8> res = lcdStatus & toAnd;
    return res.to_ulong();
}

//////////////////////////////////////////////////////////////////

std::bitset<8> Graphics::getLCDControl()
{
    uint8_t lcdControl = _memory.readInMemory(_LCDControlRegister);
    std::bitset<8> bitsetControl(lcdControl);
    return bitsetControl;
}
//////////////////////////////////////////////////////////////////

//TODO check if the one on .old isn't better
void Graphics::update( int cycles )
{
    setLCDStatus();

    // count down the LY register which is the current line being drawn. When reaches 144 (0x90) its vertical blank time
    uint8_t currentLine = _memory.readInMemory(_scanlineAdress);

    if(isLCDEnabled()) {
        _scanlineCounter -= cycles;
    }

    if (currentLine > _verticalBlancmaxScanline) {
        _memory.writeInMemory(0, _scanlineAdress);
    }

    if (_scanlineCounter <= 0){
        drawCurrentLine();
    }
}


//////////////////////////////////////////////////////////////////

void Graphics::drawScanline()
{

    // we can only draw of the LCD is enabled
    if (isLCDEnabled()) {
        renderBackground();
        renderSprites();
    }
}

//////////////////////////////////////////////////////////////////

void Graphics::drawCurrentLine()
{
    if (isLCDEnabled()) {
        _memory.incrementScanline();
        _scanlineCounter = _retraceStart;

        uint8_t currentLine = _memory.readInMemory(_scanlineAdress);

        if ( currentLine == _verticalBlancScanline) {
            _interruptHandler.requestInterrupt(IInterruptHandler::INTERRUPT::VBLANC);
        }

        if (currentLine > _verticalBlancmaxScanline) {
            _memory.writeInMemory(0, _scanlineAdress);
        }

        if (currentLine < _verticalBlancScanline) {
            drawScanline();
        }
    }
}

//////////////////////////////////////////////////////////////////
uint16_t Graphics::getBackgroundMem(bool usingWindow) 
{
    std::bitset<8> lcdControl(getLCDControl());
    if(!usingWindow) {
        if(lcdControl.test(static_cast<int>(LCDCONTROLREG::BGTILEDISPLAY))) {
            return 0x9C00;
        }
        else {
            return 0x9800;
        }
    }
    else {
        if(lcdControl.test(static_cast<int>(LCDCONTROLREG::TILEMAP))) {
            return 0x9C00;
        }
        else {
            return 0x9800;
        }
    }
}
//////////////////////////////////////////////////////////////////
RGB Graphics::getColour(uint8_t colourNum, uint16_t address) const
{
    uint8_t palette = _memory.readInMemory(address);
    std::bitset<8> bitsetPalette(palette);
    int hi = 0;
    int lo = 0;

    // which bits of the colour palette does the colour id map to?
    switch (colourNum) {
    case 0: hi = 1; lo = 0;break;
    case 1: hi = 3; lo = 2;break;
    case 2: hi = 5; lo = 4;break;
    case 3: hi = 7; lo = 6;break;
    }

    // use the palette to get the colour
    int colour = 0;
    colour = bitsetPalette[hi] << 1;
    colour |= bitsetPalette[lo];

    // convert the game colour to emulator colour
    auto currColour = _colorPalette.find(colour);
    if(currColour != _colorPalette.end()) {
        auto currRGB = _rgbPalette.find(currColour->second);
        if(currRGB != _rgbPalette.end()) {
            return currRGB->second;
        }
    }
    return {0, 0, 0};
}
//////////////////////////////////////////////////////////////////

void Graphics::renderBackground()
{
    // lets draw the background (however it does need to be enabled)
    std::bitset<8> lcdControl(getLCDControl());
    uint8_t currentLine = _memory.readInMemory(_scanlineAdress);

    if (lcdControl.test(static_cast<int>(LCDCONTROLREG::BGDISPLAY))) {
        bool unsig = true;
        bool usingWindow = false;

        uint8_t scrollY = _memory.readInMemory(_scrollY);
        uint8_t scrollX = _memory.readInMemory(_scrollX);
        uint8_t windowY = _memory.readInMemory(_windowY);
        uint8_t windowX = _memory.readInMemory(_windowX) - 7;

        if (lcdControl.test(static_cast<int>(LCDCONTROLREG::WINDISPLAY))) {
            if (windowY <= currentLine)
                usingWindow = true;
        }

        // which tile data are we using?
        uint16_t tileData = _dataMemoryAdress;
        if(!lcdControl.test(static_cast<int>(LCDCONTROLREG::BGTILEDATA))) {
            tileData += 0x0800;
            unsig = false;
        }

        // which background mem?
        uint16_t backgroundMemory = getBackgroundMem(usingWindow);

        // yPos is used to calculate which of 32 vertical tiles the
        // current scanline is drawing
        uint8_t yPos = scrollY + currentLine;
        if (usingWindow) {
            yPos = currentLine - windowY;
        }

        // which of the 8 vertical pixels of the current
        // tile is the scanline on?
        uint16_t tileRow = ((static_cast<uint8_t>(yPos / 8)) * 32);

        // time to start drawing the 160 horizontal pixels
        // for this scanline
        for (int pixel = 0; pixel < 160; pixel++) {
            uint8_t xPos = pixel + scrollX;

            // translate the current x pos to window space if necessary
            if (usingWindow) {
                if (pixel >= windowX) {
                    xPos = pixel - windowX;
                }
            }

            // which of the 32 horizontal tiles does this xPos fall within?
            uint16_t tileCol = (xPos / 8);

            // get the tile identity number. Remember it can be signed
            // or unsigned
            uint16_t tileAdress = backgroundMemory + tileRow + tileCol;
            int16_t tileID = static_cast<int8_t>(_memory.readInMemory(tileAdress));
            if(unsig){
                tileID = static_cast<uint8_t>(_memory.readInMemory(tileAdress));
            }

            // deduce where this tile identifier is in memory. Remember i
            // shown this algorithm earlier
            uint16_t tileLocation = tileData;

            if (unsig)
                tileLocation += (tileID * 16);
            else
                tileLocation += ((tileID + 128) * 16);

            // find the correct vertical line we're on of the
            // tile to get the tile data from in memory
            uint8_t line = (yPos % 8) * 2;
            std::bitset<8> data1(_memory.readInMemory(tileLocation + line));
            std::bitset<8> data2(_memory.readInMemory(tileLocation + line + 2));

            // pixel 0 in the tile is it 7 of data 1 and data2.
            // Pixel 1 is bit 6 etc..
            int colourBit = ((xPos % 8) - 7) * -1;

            // combine data 2 and data 1 to get the colour id for this pixel
            // in the tile
            int colourID = data2[colourBit];
            colourID <<= 1;
            colourID |= data1[colourBit];

            // // now we have the colour id get the actual
            // // colour from palette 0xFF47
            RGB palette = getColour(colourID, _colorPaletteAdress);

            int finalY = _memory.readInMemory(_scanlineAdress);

            // safety check to make sure what im about
            // to set is int the 160x144 bounds
            if ((finalY < 0) || (finalY > 143) || (pixel < 0) || (pixel > 159)) {
                exit(1);
            }
            _screenData[finalY][pixel] = palette;
        }
    }
}

//////////////////////////////////////////////////////////////////

void Graphics::renderSprites()
{
    // lets draw the sprites (however it does need to be enabled)
    std::bitset<8> lcdControl = getLCDControl();
    if (lcdControl.test(static_cast<int>(LCDCONTROLREG::OBJDISPLAY))) {
        bool use8x16 = false;
        if (lcdControl.test(static_cast<int>(LCDCONTROLREG::OBJSIZE))) {
            use8x16 = true;
        }

        for (int sprite = 0; sprite < 40; sprite++) {
            uint8_t index = sprite * 4;
            uint8_t yPos = _memory.readInMemory(0xFE00 + index) - 16;
            uint8_t xPos = _memory.readInMemory(0xFE00 + index + 1) - 8;
            uint8_t tileLocation = _memory.readInMemory(0xFE00 + index + 2);
            std::bitset<8> attributes(_memory.readInMemory(0xFE00 + index+ 3));

            bool yFlip = attributes.test(6);
            bool xFlip = attributes.test(5);

            int scanline = _memory.readInMemory(_scanlineAdress);

            int ysize = 8;
            if (use8x16)
                ysize = 16;

            if ((scanline >= yPos) && (scanline < (yPos+ysize))) {
                int line = scanline - yPos;

                if (yFlip) {
                    line -= ysize;
                    line *= -1;
                }

                line *= 2;
                uint16_t dataAdress =
                    _dataMemoryAdress + (tileLocation * _sizeOfTileInMemory) + line ;
                std::bitset<8> data1(_memory.readInMemory(dataAdress));
                std::bitset<8> data2(_memory.readInMemory(dataAdress) + 1);

                for (int tilePixel = 7; tilePixel >= 0; tilePixel--) {
                    int colourBit = tilePixel;
                    if (xFlip) {
                        colourBit -= 7;
                        colourBit *= -1;
                    }
                    int colourID = data2[colourBit];
                    colourID <<= 1;
                    colourID |= data1[colourBit];

                    uint16_t address = 0xff49;
                    if(!attributes.test(4)){
                        address = 0xff48;
                    }

                    COLOUR colourPalette = _colorPalette.find(colourID)->second;
                    RGB rgb = getColour(colourID, address);

                    // white is transparent for sprites.
                    if (colourPalette == COLOUR::WHITE) {
                        continue;
                    }

                    int xPix = (0 - tilePixel) + 7;

                    int pixel = xPos + xPix;

                    if ((scanline < 0) || (scanline > 143) || (pixel < 0) || (pixel > 159)) {
                        assert(false);
                        continue;
                    }

                    // check if pixel is hidden behind background
                    if (attributes.test(7)) {
                        if ( (_screenData[scanline][pixel]._red != 0xff)
                             || (_screenData[scanline][pixel]._green != 0xff)
                             || (_screenData[scanline][pixel]._blue != 0xff))
                            continue;
                    }

                    _screenData[scanline][pixel] = rgb;

                }
            }
        }
    }
}

//////////////////////////////////////////////////////////////////

// Graphics::COLOUR Emulator::GetColour(uint8_t colourNum, uint16_t address) const
// {
//     COLOUR res = WHITE;
//     uint8_t palette = _memory.readInMemory(address);
//     int hi = 0;;
//     int lo = 0;

//     switch (colourNum)
//         {
//         case 0: hi = 1; lo = 0 ;break ;
//         case 1: hi = 3; lo = 2 ;break ;
//         case 2: hi = 5; lo = 4 ;break ;
//         case 3: hi = 7; lo = 6 ;break ;
//         default: assert(false); break ;
//         }

//     int colour = 0;
//     colour = BitGetVal(palette, hi) << 1;
//     colour |= BitGetVal(palette, lo);

//     switch (colour)
//         {
//         case 0: res = WHITE;break ;
//         case 1: res = LIGHT_GRAY;break ;
//         case 2: res = DARK_GRAY;break ;
//         case 3: res = BLACK;break ;
//         default: assert(false); break ;
//         }

//     return res;
// }

//////////////////////////////////////////////////////////////////
void Graphics::setLCDStatus()
{

    std::bitset<8> lcdStatus(_memory.readInMemory(_LCDStatusAdress));
    if (!isLCDEnabled()) {
        _scanlineCounter = _retraceStart;
        _memory.writeInMemory(0, _scanlineAdress);

        // mode gets set to 1 when disabled screen.
        lcdStatus &= 252;
        lcdStatus.set(0);
        _memory.writeInMemory(lcdStatus.to_ulong(), _LCDStatusAdress);
        return;
    }


    uint8_t currentLine = _memory.readInMemory(_scanlineAdress);
    uint8_t currentMode = getLCDMode();

    int mode = 0;
    bool reqInt = false;

    // in vblank so set mode to 1
    if (currentLine >= _verticalBlancScanline) {
        mode = 1;
        lcdStatus.set(0);
        lcdStatus.reset(1);
        reqInt = lcdStatus.test(4);
    }
    else {
        int mode2Bounds = (_retraceStart - 80);
        int mode3Bounds = (mode2Bounds - 172);


        // mode 2
        if (_scanlineCounter >= mode2Bounds) {
            mode = 2;
            lcdStatus.set(1);
            lcdStatus.reset(0);
            reqInt = lcdStatus.test(5);
        }
        // mode 3
        else if (_scanlineCounter >= mode3Bounds) {
            mode = 3;
            lcdStatus.set(1);
            lcdStatus.set(0);
        }
        // mode 0
        else {
            mode = 0;
            lcdStatus.reset(1);
            lcdStatus.reset(0);
            reqInt = lcdStatus.test(3);
        }
    }

    // just entered a new mode. Request interupt
    if (reqInt && (currentMode != mode))
        _interruptHandler.requestInterrupt(IInterruptHandler::INTERRUPT::LCD);

    // check for coincidence flag
    uint8_t coincidenceFlag = _memory.readInMemory(_coincidenceAdress);
    if ( currentLine == coincidenceFlag) {
        lcdStatus.set(2);

        if (lcdStatus.test(6)) {
            _interruptHandler.requestInterrupt(IInterruptHandler::INTERRUPT::LCD);
        }
    }
    else {
        lcdStatus.reset(2);
    }
    _memory.writeInMemory(lcdStatus.to_ulong(), _LCDStatusAdress);
}

