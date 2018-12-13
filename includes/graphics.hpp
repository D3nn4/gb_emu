#ifndef _GRAPHICS_
#define _GRAPHICS_

#include "imemory.hpp"
#include "iinterupthandler.hpp"

class Graphics
{
public:

    enum class LCDSTATUS
        {
            HBLANK = 0x00,
            VBLANK = 0x01,
            SPRITES = 0x10,
            DATATODRIVER = 0x11
        };

    enum class LCDCONTROLREG
        {
            LCDDISPLAY = 7,
            TILEMAP = 6,
            WINDIPLAY = 5,
            BGTILEDATA = 4,
            BGTILEDISPLAY = 3,
            OBJSIZE = 2,
            OBJDISPLAY = 1,
            BGDISPLAY = 0

        };
    Graphics(IMemory& memory, IInterruptHandler& interruptHandler);
    void update(int cycles);

private:

    bool isLCDEnabled();
    void setLCDStatus();

    IMemory& _memory;
    IInterruptHandler& _interruptHandler;

    int            _scanlineCounter = 0;

    uint16_t const _coincidenceAdress  = 0xff45;
    uint16_t const _scanlineAdress  = 0xff44;
    uint16_t const _LCDStatusAdress  = 0xff41;
    uint16_t const _LCDControlRegister  = 0xff40;

    uint16_t const  _verticalBlanc   = 0x0144;
    uint16_t const  _maxScanline     = 0x0153;

};
#endif                          /*GRAPHICS*/
