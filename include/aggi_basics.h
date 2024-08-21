#pragma once

#include <cinttypes>

namespace aggi {
    template<class T>
    struct ConstRowInfo {
        uint32_t x1, x2;
        const T* ptr;
        ConstRowInfo() { }
        ConstRowInfo(uint32_t x1, uint32_t x2, const T* ptr) :
            x1{x1}, x2{x2}, ptr{ptr}
        { }
    }
};