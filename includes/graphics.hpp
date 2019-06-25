#ifndef _GRAPHICS_
#define _GRAPHICS_

#include <bitset>
#include "imemory.hpp"
#include "iinterupthandler.hpp"

struct RGB
{
public:
    RGB(uint8_t red, uint8_t green, uint8_t blue)
        :_red(red),
         _green(green),
         _blue(blue){};
    uint8_t _red;
    uint8_t _green;
    uint8_t _blue;
};

class Graphics
{
public:

    enum class COLOUR
        {
            WHITE,
            LIGHT_GRAY,
            DARK_GRAY,
            BLACK
        };

    enum class LCDSTATUS
        {
            HBLANK       = 0x00,
            VBLANK       = 0x01,
            SPRITES      = 0x10,
            DATATODRIVER = 0x11
        };

    enum class LCDCONTROLREG
        {
            LCDDISPLAY    = 7,
            TILEMAP       = 6,
            WINDISPLAY    = 5,
            BGTILEDATA    = 4,
            BGTILEDISPLAY = 3,
            OBJSIZE       = 2,
            OBJDISPLAY    = 1,
            BGDISPLAY     = 0
        };

    Graphics(IMemory& memory, IInterruptHandler& interruptHandler);
    void update(int cycles);
    std::vector<std::vector<RGB>> const & getScreenData();
    void resetScreen();

private:

    bool isLCDEnabled();
    void setLCDStatus();
    void drawScanline();
    void drawCurrentLine();
    void issueVerticalBlank();
    void renderBackground();
    void renderSprites();
    RGB getColour(uint8_t colourNum, uint16_t address) const;

    std::bitset<8> getLCDControl();
    uint8_t getLCDMode() const;
    uint16_t getBackgroundMem(bool usingWindow);

    std::map<int, COLOUR> const _colorPalette = {
        {0, COLOUR::WHITE},
        {1, COLOUR::LIGHT_GRAY},
        {2, COLOUR::DARK_GRAY},
        {3, COLOUR::BLACK}
    };

    std::map<COLOUR, RGB> const _rgbPalette = {
        {COLOUR::WHITE,      {0xff, 0xff, 0xff}},
        {COLOUR::LIGHT_GRAY, {0xcc, 0xcc, 0xcc}},
        {COLOUR::DARK_GRAY,  {0x77, 0x77, 0x77}},
        {COLOUR::BLACK,      {0x00, 0x00, 0x00}}
    };

    std::vector<std::vector<RGB>> _screenData;
    IMemory& _memory;
    IInterruptHandler& _interruptHandler;

    int _scanlineCounter = 0;

    uint16_t const _windowX            = 0xff4B;
    uint16_t const _windowY            = 0xff4A;
    uint16_t const _colorPaletteAdress = 0xff47;
    uint16_t const _coincidenceAdress  = 0xff45;
    uint16_t const _scanlineAdress     = 0xff44;
    uint16_t const _scrollX            = 0xff43;
    uint16_t const _scrollY            = 0xff42;
    uint16_t const _LCDStatusAdress    = 0xff41;
    uint16_t const _LCDControlRegister = 0xff40;
    uint16_t const _dataMemoryAdress   = 0x8000;

    int const _sizeOfTileInMemory = 16;
    int const _offset             = 128;
    int const _retraceStart       = 456;


    uint8_t const  _verticalBlancScanline = 0x90;
    uint8_t const  _verticalBlancmaxScanline   = 0x99;

};
#endif                          /*GRAPHICS*/
