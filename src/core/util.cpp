#include "util.h"

// Primitive helper utilities, clamping, and screen control
int clampInt(int val, int minVal, int maxVal);{
    if (val > maxVal){
        return maxVal;
    }
    if (val < minVal){
        return minVal;
    }
    return val;
}
energy = clampInt(energy + 25, 0, 100);
