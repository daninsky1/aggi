#pragma once

#include <cinttypes>

namespace aggi {
    typedef uint8_t CoverType;
    enum CoverScaleE {
        COVER_SHIFT = 8,
        COVER_SIZE = 1 << COVER_SHIFT,
        COVER_MASK = COVER_SIZE - 1,
        COVER_NONE = 0,
        COVER_FULL = COVER_MASK
    };
    
    template<class T>
    struct RectBase {
        typedef T ValueType;
        typedef RectBase<T> SelfType;
        
        T x1, y1, x2, y2;
        
        RectBase() { }
        RectBase(T x1, T y1, T x2, T y2) :
            x1{x1}, y1{y1}, x2{x2}, y2{y2}
        { }
        
        void init(T x1, T y1, T x2, T y2) {
            this->x1 = x1;
            this->y1 = y1;
            this->x2 = x2;
            this->y2 = y2;
        }
        
        const RectBase<T>& normalize() {
            T t;
            if (x1 > x2) {
                t = x1;
                x1 = x2;
                x2 = t;
            }
            if (y1 > y2) {
                t = y1;
                y1 = y2;
                y2 = t;
            }
        }
        
        bool clip(const RectBase<T>& r) {
            if (x2 > r.x2) x2 = r.x2;
            if (y2 > r.y2) y2 = r.y2;
            if (x1 < r.x1) x1 = r.x1;
            if (y1 < r.y1) y1 = r.y1;
            return x1 <= x2 && y1 <= y2;
        }
        
        bool isValid() const {
            return x1 <= x2 && y1 <= y2;
        }
        
        bool hitTest(T x, T y) const {
            return (x >= x1 && x <= x2 && y >= y1 && y <= y2);
        }
        
        bool overlaps(const RectBase<T>& r) const {
            return !(r.x1 > x2 || r.x2 < x1 || r.y1 > y2 || r.y2 < y1);
        }
    };
    
    typedef RectBase<int> RectI;
    typedef RectBase<float> RectF;
    typedef RectBase<double> RectD;
    
    template<class T>
    struct ConstRowInfo {
        uint32_t x1, x2;
        const T* ptr;
        ConstRowInfo() { }
        ConstRowInfo(uint32_t x1, uint32_t x2, const T* ptr) :
            x1{x1}, x2{x2}, ptr{ptr}
        { }
    };
    
    uint32_t uround(double v) {
        return static_cast<uint32_t>(v + 0.5);
    }
};