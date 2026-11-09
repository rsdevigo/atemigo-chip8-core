#include <iostream>
#include "src/core/coreoptions.h"
#include "src/core/chip8machine.h"

using AtemigoChip8::core::Chip8Machine;
using AtemigoChip8::core::CoreOptions;

int main (void)
{
    int a = -2;
    int b = a & 4095;

    CoreOptions core;
    
    Chip8Machine *machine;
    machine = new Chip8Machine();

    std::cout << "Decimal: " << b << '\n';
    std::cout << "Hexadecimal: " << std::hex << b << '\n';
    std::cout << "FPS: " << std::dec << core.fps << '\n';
    return 0;
}