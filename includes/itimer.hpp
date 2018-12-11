#ifndef _ITIMER_
#define _ITIMER_

#include <map>
#include <string>

class ITimer
{
public:
    // virtual void turnOn() = 0;
    // virtual void turnOff() = 0;
    virtual void update(int cycles) = 0;
    virtual bool isOn() = 0;
    virtual uint32_t getClockFrequency() = 0;
    virtual int setClockFrequency() = 0;

    static uint16_t const _DIV = 0xff04; //timer divider
    static uint16_t const _TIMA = 0xff05; //timer counter
    static uint16_t const _TMA = 0xff06; // timer modulo
    static uint16_t const _TMC = 0xff07; //timer controler

protected:

    int _cycleCounter = 1024;
    int _dividerRegister = 0;
    uint32_t const _clockSpeed = 4194304;

    std::map<uint8_t, uint32_t> const _speed = {
        {0x00, 4096},
        {0x01, 262144},
        {0x02, 65536},
        {0x03, 16384}
    };
};
#endif /*ITIMER*/
