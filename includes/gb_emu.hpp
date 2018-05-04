#ifndef _GB_
#define _GB_

#include <vector>
#include "romloader.hpp"
#include "registers.hpp"

class Gb
{
public:
  Gb();
private:
  // RomLoader _romLoader;
  // std::vector<uint8_t> _cartridge;
  Registers _registers;
  // std::vector<uint8_t> _readOnlyMemory;
  Memory _memoryHandler;
};
#endif /*GB*/
