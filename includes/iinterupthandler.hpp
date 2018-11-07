#ifndef _IINTERRUPTHANDLER_
#define _IINTERRUPTHANDLER_

#include <vector>
#include "imemory.hpp"

class IInterruptHandler
{
public:

  virtual void doInterrupt() = 0;
  virtual bool isMasterSwitchEnabled() = 0;
  virtual void enableMasterSwitch() = 0;
  virtual void disableMasterSwitch() = 0;
  virtual void requestInterrupt(int id) = 0;

protected:

  bool _masterInterruptSwitch = false;
  uint16_t const _interruptEnableRegister = 0xffff;
  uint16_t const _interruptRequestRegister = 0xff0f;

  uint16_t const _vBlanc = 0x0040;
  uint16_t const _lcd = 0x0048;
  uint16_t const _timer = 0x0050;
  uint16_t const _serial = 0x0058;
  uint16_t const _joypad = 0x0060;

  std::vector<uint16_t> const serviceRoutineAdress =
    {_vBlanc, _lcd, _timer, _serial, _joypad};
};
#endif /*IINTERRUPTHANDLER*/
