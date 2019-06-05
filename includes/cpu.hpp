#ifndef _CPU_
#define _CPU_

#include <string>
#include "memory.hpp"
#include "iromloader.hpp"
#include "instructionhandler.hpp"
#include "interupthandler.hpp"
#include "timer.hpp"
#include "graphics.hpp"
#include "irenderer.hpp"

//TEMP
#include <QObject>


class Cpu : public QObject
{ // TEMP
    Q_OBJECT
public:
    Cpu(IRomLoader& romloader);
    int getCurrentCycles();
    // void boot();

    void nextStep();
    void updateDebug();
    bool launchGameDebug(std::string const & cartridgeName);
    IMemory::State getState();

    void update();
    bool launchGame(std::string const & cartridgeName);
    void stopGame();

    std::string getReadableInstruction();

    std::vector<std::vector<RGB>> getScreen() {
        return _graphics.getScreenData();
    }

   std::vector<uint8_t> getPalette();
signals:
    void screen_refresh();

private:

    bool _gameLoaded = false;
    int _cycles = 0;
    int const _maxCycles = 70221;
    Memory _memory;
    IRomLoader& _romLoader;
    InterruptHandler _interruptHandler;
    Timer _timer;
    InstructionHandler _instructionHandler;
    Graphics _graphics;
    // IRenderer _renderer;

    std::stringstream _readableInstructionStream;

    // TEMP
    quint8 _screenBuffer[144][160][4];

};
#endif /*CPU*/
