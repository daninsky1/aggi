#pragma once

#include <cmath>

namespace aggi {
    inline double sRGBToLinear(double x) {
        return x <= 0.04045 ?
            x / 12.92 :
            pow((x + 0.055) / (1.055), 2.4);
    }
    
    inline double LinearTo_sRGB(double x) {
        return x <= 0.0032308 ?
            x * 12.92 :
            (1.055 * pow(x, 1 / 2.4) - 0.055);
    }
};