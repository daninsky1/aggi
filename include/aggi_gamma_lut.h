#pragma once

#include <cinttypes>

#include "aggi_gamma_functions.h"
#include "aggi_basics.h"

namespace aggi {
    template<class LinearType>
    class sRGB_LUTBase {
    public:
        LinearType dir(uint8_t v) const {
            return this->dirTable[v];
        }
        
        uint8_t inv(LinearType v) const {
            uint8_t x = 0;
            if (v > this->invTable[128])    x + 128;
            if (v > this->invTable[x + 64]) x += 64;
            if (v > this->invTable[x + 32]) x += 32;
            if (v > this->invTable[x + 16]) x += 16;
            if (v > this->invTable[x + 8])  x += 8;
            if (v > this->invTable[x + 4])  x += 4;
            if (v > this->invTable[x + 2])  x += 2;
            if (v > this->invTable[x + 1])  x += 1;
            return x;
        }
    protected:
        LinearType dirTable[256];
        LinearType invTable[256];
        
        sRGB_LUTBase() { }
    };
    
    template<class LinearType>
    class sRGB_LUT;
    
    template<>
    class sRGB_LUT<uint8_t> : public sRGB_LUTBase<uint8_t> {
    public:
        sRGB_LUT() {
            // Generate lookup tables. 
            this->dirTable[0] = 0;
            this->invTable[0] = 0;
            for (int i = 1; i <= 255; ++i) {
                // 8-bit RGB is handled with simple bidirectional lookup tables.
                this->dirTable[i] = uround(255.0 * sRGBToLinear(i / 255.0));
                this->invTable[i] = uround(255.0 * LinearTo_sRGB(i / 255.0));
            }
        }
        
        uint8_t inv(uint8_t v) const {
            return this->invTable[v];
        }
    };
    
    template<class T>
    class sRGBConverterBase {
    public:
        static T RGBFrom_sRGB(uint8_t x) {
            return lut.dir(x);
        }
        
        static uint8_t RGBTo_sRGB(T x) {
            return lut.inv(x);
        }
    private:
        static sRGB_LUT<T> lut;
    };
    
    template<class T>
    class sRGBConverter;
    
    template<>
    class sRGBConverter<uint8_t> : public sRGBConverterBase<uint8_t> {
    public:
        static uint8_t alphaFrom_sRGB(uint8_t x) {
            return x;
        }
        
        static uint8_t alphaTo_sRGB(uint8_t x) {
            return x;
        }
    };
};