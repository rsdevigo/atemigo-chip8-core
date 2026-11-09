#pragma once
#include <cstdint>

namespace AtemigoChip8::core {
    struct CoreOptions {
        int numberOfInstructs;
        bool eightXBehavior;
        bool bNNNBehavior;
        bool fX1EBehavior;
        bool fXsBehavior;
        int soundFrequency;
        int sampleRate;
        int fps;
        uint32_t pixelColor;

        CoreOptions()
        {
            numberOfInstructs = 12;
            eightXBehavior = true;
            bNNNBehavior = false;
            fX1EBehavior = true;
            fXsBehavior = true;
            soundFrequency = 440;
            sampleRate = 48000;
            fps = 60;
            pixelColor = 0xFFFFFF;
        }
        
    };
}