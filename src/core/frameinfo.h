#pragma once
#include <cstdint>
#include "coreerror.h"

namespace AtemigoChip8::core
{
    // Fatos fixos da máquina, sem nada do libretro.
    // A camada fina traduz isto para retro_system_av_info:
    //   width/height -> geometry.base_* e geometry.max_* (a resolução nunca muda)
    //   fps          -> timing.fps
    // A taxa de amostragem não está aqui: pelo doc, é um fato fixo da camada fina.
    struct FrameInfo
    {
        CoreError errorData;
        uint8_t isStopped = 0;
        const uint8_t* frameBuffer = 0;
        bool isEmitting = false;
    };
}
