#pragma once
#include <cstdint>

namespace AtemigoChip8::core
{
    // Fatos fixos da máquina, sem nada do libretro.
    // A camada fina traduz isto para retro_system_av_info:
    //   width/height -> geometry.base_* e geometry.max_* (a resolução nunca muda)
    //   fps          -> timing.fps
    // A taxa de amostragem não está aqui: pelo doc, é um fato fixo da camada fina.
    struct MachineInfo
    {
        uint32_t width;
        uint32_t height;
        uint32_t fps;
    };
}
