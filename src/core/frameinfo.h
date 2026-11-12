#pragma once
#include <cstdint>
#include "coreerror.h"

namespace AtemigoChip8::core
{
    struct FrameInfo
    {
        CoreError errorData;
        bool isStopped = false;
        const uint8_t* frameBuffer = nullptr;
        bool isEmitting = false;
    };
}
