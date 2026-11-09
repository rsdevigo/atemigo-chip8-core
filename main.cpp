#include <iostream>
#include "src/core/coreoptions.h"
#include "src/core/chip8machine.h"

int main (void)
{
    int a = -2;
    int b = a & 4095;

    AtemigoChip8::core::CoreOptions core;
    

    std::cout << "Decimal: " << b << '\n';
    std::cout << "Hexadecimal: " << std::hex << b << '\n';
    std::cout << "FPS: " << std::dec << core.fps << '\n';
    return 0;
}