#include <bitset>
#include "graphics.hpp"

Graphics::Graphics(IMemory& memory, IInterruptHandler& interruptHandler)
    :_memory(memory),
     _interruptHandler(interruptHandler){};

void Graphics::update(int cycles)
{
    // SetLCDStatus( ) ;

    if (isLCDEnabled()) {
        _scanlineCounter -= cycles ;

        if (_scanlineCounter <= 0) {
            // time to move onto next scanline
            _memory.incrementScanline();
            uint8_t currentline = _memory.readInMemory(_scanlineAdress);

            _scanlineCounter = 456 ;

            // we have entered vertical blank period
            if (currentline == _verticalBlanc) {
                _interruptHandler.requestInterrupt(IInterruptHandler::INTERRUPT::VBLANC);
            }

            // if gone past scanline 153 reset to 0
            else if (currentline > _maxScanline) {
                _memory.writeInMemory(0, _scanlineAdress);
            }

            // draw the current scanline 
            else if (currentline < _verticalBlanc) {
                // DrawScanLine() ;
            }
        }
    }
}

void Graphics::setLCDStatus()
{
   uint8_t status = _memory.readInMemory(_LCDStatusAdress);
   std::bitset<8> bitsetStatus(status);

   if (!isLCDEnabled()) {
     // set the mode to 1 during lcd disabled and reset scanline
     _scanlineCounter = 456;
     _memory.writeInMemory(0, _scanlineAdress);
     bitsetStatus &= 252;
     bitsetStatus.set(0);
     _memory.writeInMemory(bitsetStatus.to_ulong(), _LCDStatusAdress);
     return;
   }

   uint8_t currentLine = _memory.readInMemory(_scanlineAdress);
   //TODO find what it is and rename it
   std::bitset<8> bitsetAnd(0x03);
   std::bitset<8> bitsetOp = bitsetStatus & bitsetAnd;
   uint8_t currentMode = bitsetOp.to_ulong();

   uint8_t mode = 0;
   bool reqInt = false;

   // in vblank so set mode to 1
   if (currentLine >= _verticalBlanc) {
       mode = 1;
       bitsetStatus.set(0);
       bitsetStatus.reset(1);
       reqInt = bitsetStatus.test(4);
   }
   else {
       int mode2bounds = 456-80;
       int mode3bounds = mode2bounds - 172;

       // mode 2
       if (_scanlineCounter >= mode2bounds) {
           mode = 2;
           bitsetStatus.set(1);
           bitsetStatus.reset(0);
           reqInt = bitsetStatus.test(5);
       }
       // mode 3
     else if(_scanlineCounter >= mode3bounds) {
         mode = 3;
         bitsetStatus.set(1);
         bitsetStatus.set(0);
     }
       // mode 0
     else {
         mode = 0;
         bitsetStatus.reset(1);
         bitsetStatus.reset(0);
         reqInt = bitsetStatus.test(3);
     }
   }

   // just entered a new mode so request interupt
   if (reqInt && (mode != currentMode)) {
       _interruptHandler.requestInterrupt(IInterruptHandler::INTERRUPT::LCD);
   }

   // check the coincidence flag
   //TODO && check if
   if (_memory.readInMemory(_scanlineAdress) == _memory.readInMemory(_coincidenceAdress)) {
       bitsetStatus.set(2);
       if (bitsetStatus.test(6)) {
           _interruptHandler.requestInterrupt(IInterruptHandler::INTERRUPT::LCD);
       }
   }
   else
   {
       bitsetStatus.reset(2);
   }
   _memory.writeInMemory(bitsetStatus.to_ulong(), _LCDStatusAdress);
}

bool Graphics::isLCDEnabled()
{
    std::bitset<8> lcdControlRegister(_memory.readInMemory(_LCDControlRegister));
    return lcdControlRegister.test(static_cast<uint8_t>(LCDCONTROLREG::LCDDISPLAY));
}
