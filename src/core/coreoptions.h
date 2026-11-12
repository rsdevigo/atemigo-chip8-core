#pragma once
#include <cstdint>

namespace AtemigoChip8::core {
    struct CoreOptions {
        int numberOfInstructs;
        bool eightXBehavior;
        bool bNNNBehavior;
        bool fX1EBehavior;
        bool fXsBehavior;
        

        CoreOptions()
        {
            numberOfInstructs = 12;
            eightXBehavior = true;
            bNNNBehavior = false;
            fX1EBehavior = true;
            fXsBehavior = true;
        }
        
    };
}