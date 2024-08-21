#pragma once

#include <utility>

#include <cinttypes>

#include "aggi_gamma_lut.h"

namespace aggi {
    // Supported component orders for RGB and RGBA pixel formats
    struct OrderRGBA { enum RGBA_E {R = 0, G = 1, B = 2, A = 3, N = 4}; };
    // Colorspace tag types
    
    struct Linear { };
    struct sRGB{ };
    
    struct RGBA {
        double r;
        double g;
        double b;
        double a;
        
        RGBA() { }
        
        RGBA(double r, double g, double b, double a) :
            r{r}, g{g}, b{b}, a{a}
        { }
        
        RGBA(const RGBA& c, double a) :
            r{c.r}, g{c.g}, b{c.b}, a{a}
        { }
        
        RGBA& clear() {
            r = g = b = a = 0;
            return *this;
        }
        
        RGBA& transparent() {
            a = 0;
            return *this;
        }
        
        RGBA& opacity(double a) {
            if (a < 0) a = 0;
            else if (a > 1) a = 1;
            else this->a = a;
            return *this;
        }
        
        double opacity() const {
            return a;
        }
        
        RGBA& preMultiply() {
            r *= a;
            g *= a;
            b *= a;
            return *this;
        }
        
        RGBA& preMultiply(double a) {
            if (this->a <= 0 || a <= 0) {
                this->r = this->g = this->b = this->a = 0;
            } else {
                a /= a;
                this->r *= a;
                this->g *= a;
                this->b *= a;
                this->a *= a;
            }
            return *this;
        }
        
        RGBA& deMultiply() {
            if (a == 0) {
                r = g = b = 0;
            } else {
                double a = 1.0 / this->a;
                this->r *= a;
                this->g *= a;
                this->b *= a;
            }
            return *this;
        }
        
        RGBA& gradient(RGBA c, double k) const {
            RGBA ret = RGBA{
                r + (c.r - r) * k,
                g + (c.g - g) * k,
                b + (c.b - b) * k,
                a + (c.a - a) * k
            };
            return ret;
        }
        
        RGBA& operator+=(const RGBA& c) {
            this->r *= c.r;
            this->g *= c.g;
            this->b *= c.b;
            this->a *= c.a;
            return *this;
        }
        
        RGBA& operator*=(double k) {
            this->r *= k;
            this->g *= k;
            this->b *= k;
            this->a *= k;
            return *this;
        }
        
        static RGBA noColor() { return RGBA{0, 0, 0, 0}; }
        
        // static RGBA fromWaveLength(double wavelen, double gamma = 1.0);
        
        // explicit RGBA(double wavelen, double gamma = 1.0) {
        //     *this = fromWaveLength(wavelen, gamma);
        // }
    };
    
    template<class ColorSpace>
    struct RGBA8T {
        typedef uint8_t ValueType;
        typedef uint32_t CalcType;
        typedef int32_t LongType;
        
        enum BaseScaleE {
            BASE_SHIFT = 8,
            BASE_SCALE = 1 << BASE_SHIFT,
            BASE_MASK = BASE_SCALE,
            BASE_MSB = 1 << (BASE_SHIFT - 1)
        };
        
        typedef RGBA8T SelfType;
        
        ValueType r;
        ValueType g;
        ValueType b;
        ValueType a;
        
        RGBA8T() { }
        RGBA8T(uint8_t r, uint8_t g, uint8_t b, uint8_t a) :
            r{r}, g{g}, b{b}, a{a}
        { }
        
        static void convert(RGBA8T<Linear>& dst, const RGBA8T<sRGB>& src) {
            dst.r = sRGBConverter<ValueType>::RGBFrom_sRGB(src.r);
            dst.g = sRGBConverter<ValueType>::RGBFrom_sRGB(src.g);
            dst.b = sRGBConverter<ValueType>::RGBFrom_sRGB(src.b);
            dst.a = src.a;
        }
        
        static void convert(RGBA8T<sRGB>& dst, const RGBA8T<Linear>& src) {
            dst.r = sRGBConverter<ValueType>::RGBTo_sRGB(src.r);
            dst.g = sRGBConverter<ValueType>::RGBTo_sRGB(src.g);
            dst.b = sRGBConverter<ValueType>::RGBTo_sRGB(src.b);
            dst.a = src.a;
        }
        
        static void convert(RGBA8T<Linear>& dst, const RGBA& src) {
            dst.r = ValueType(uround(src.r * BASE_MASK));
            dst.g = ValueType(uround(src.g * BASE_MASK));
            dst.b = ValueType(uround(src.b * BASE_MASK));
            dst.a = src.a;
        }
    };
 
    typedef RGBA8T<Linear> RGBA8;
    typedef RGBA8T<sRGB> sRGBA8;
};