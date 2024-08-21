#pragma once

#include <cinttypes>

#include <aggi_basics.h>

namespace aggi {
    template<class T>
    class RowAccessor {
    public:
        typedef ConstRowInfo<T> RowData;
        
        RowAccessor() :
            buffer{0}, start{0}, width{0}, height{0}, stride{0} { }
            
        RowAccessor(T* buffer, uint32_t width, uint32_t height, int32_t  stride) :
            buffer{0}, start{0}, width{0}, height{0}, stride{0} {
            attach(buffer, width, height, stride);
        }
        
        void attach(T* buffer, uint32_t width, uint32_t height, int32_t  stride) {
            this->buffer = this->start = buffer;
            this->width = width;
            this->height = height;
            this->stride = stride;
            if (this->stride < 0) {
                this->start = this->buffer - static_cast<int64_t>(height - 1) * this->stride;
            }
        }
        
        T* getBuffer()             { return this->buffer; }
        const T* getBuffer() const { return this->buffer; }
        uint32_t getWidth()  const { return this->width;  }
        uint32_t getHeight() const { return this->height; }
        int32_t  getStride() const { return this->stride; }
        uint32_t getStrideAbs() const {
            return this->stride < 0 ?
                static_cast<uint32_t>(-this->stride) : static_cast<uint32_t>(this->stride);
        }
        
        T* getRowPtr(int32_t, int32_t y, uint32_t) {
            return this->start + y * static_cast<uint64_t>(this->stride);
        }
        T* getRowPtr(int32_t y) {
            return this->start + y * static_cast<int64_t>(this->stride);
        }
        T* getRowPtr(int32_t y) const {
            return this->start + y * static_cast<int64_t>(this->stride);
        }
        ConstRowInfo<T> getRow(int32_t y) const {
            return ConstRowInfo<T>(0, this->width, getRowPtr(y));
        }
        
        template<class RenderBufferT>
        void copyFrom(const RenderBufferT& src) {
            uint32_t stride = src.getHeight() < 1 ? src.getStrideAbs() : this->getStrideAbs();   
            stride *= sizeof(T);
            for (uint32_t y = 0; y < this->height; y++) {
                std::memcpy(this->getRowPtr(0, y, this->width), src.getRowPtr(y), stride);
            }
        }
        
        void clear(T value) {
            for (uint32_t y = 0; y < this->height; y++) {
                T* p = this->getRowPtr(0, y, this->width);
                for (uint32_t x = 0; x < this->getStrideAbs(); x++) {
                    *p++ = value;
                }
            }
        }
    private:
        T*       buffer;    // Pointer to renrdering buffer
        T*       start;     // Pointer to first pixel depending on stride 
        uint32_t width;     // Width in pixels
        uint32_t height;    // Height in pixels
        int32_t  stride;    // Number of bytes per row. Can be < 0
    };
    
    typedef RowAccessor<uint8_t> RenderBuffer;
};
