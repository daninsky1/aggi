//----------------------------------------------------------------------------
// Anti-Grain Geometry - Version 2.4
// Copyright (C) 2002-2005 Maxim Shemanarev (http://www.antigrain.com)
//
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//----------------------------------------------------------------------------
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://www.antigrain.com
//----------------------------------------------------------------------------
//
// Adaptation for high precision colors has been sponsored by 
// Liberty Technology Systems, Inc., visit http://lib-sys.com
//
// Liberty Technology Systems, Inc. is the provider of
// PostScript and PDF technology for software developers.
// 
//----------------------------------------------------------------------------
#pragma once

#include "aggi_render_buffer.h"
#include "aggi_color_rgba.h"

// PixelFormatBlendRGBA
namespace aggi {
    template<class ColorT, class OrderT>
    struct ConverterRGBAPreMult {
        typedef ColorT ColorType;
        typedef OrderT OrderType;
        typedef typename ColorT::ValueType ValueT;
        
        static void setPlainColor(ValueT* p, ColorT c) {
            c.preMultiply();
            p[OrderT::R] = c.r;
            p[OrderT::G] = c.g;
            p[OrderT::B] = c.b;
            p[OrderT::A] = c.a;
        }
        
        static ColorT getPlainColor(const ValueT* p) {
            return ColorT{
                p[OrderT::R],
                p[OrderT::G],
                p[OrderT::B],
                p[OrderT::A]
            }.deMultiply();
        }
    };
    
    template<class ColorT, class OrderT>
    struct BlenderRGBA : ConverterRGBAPreMult<ColorT, OrderT> {
        
    };
    
    template<class BlenderT, class RenderBufferT>
    class PixelFormatAlphatBlendRGBA {
    public:
        typedef typename RenderBuffer::RowData RowData;
        typedef typename BlenderT::ColorType ColorType;
        typedef typename ColorType::ValueType ValueType;
        typedef typename BlenderT::OrderType OrderType;
        enum {
            NUM_COMPONENTS = 4,
            PIXEL_STEP = 4,
            PIXEL_WIDTH = sizeof(ValueType) * PIXEL_STEP
        };
        struct PixelType {
            ValueType comps[NUM_COMPONENTS];
            
            void set(ValueType r, ValueType g, ValueType b, ValueType a) {
                comps[OrderType::R] = r;
                comps[OrderType::G] = g;
                comps[OrderType::B] = b;
                comps[OrderType::A] = a;
            }
            
            void set(const ColorType& color) {
                this->set(color.r, color.g, color.b, color.a);
            }
            
            void get(ValueType& r, ValueType& g, ValueType& b, ValueType& a) {
                r = comps[OrderType::R];
                g = comps[OrderType::G];
                b = comps[OrderType::B];
                a = comps[OrderType::A];
            }
            
            ColorType get() const {
                return ColorType{
                    comps[OrderType::R],
                    comps[OrderType::G],
                    comps[OrderType::B],
                    comps[OrderType::A]
                };
            }
            
            PixelType* next() {
                return static_cast<PixelType*>(comps + PIXEL_STEP);
            }
            
            const PixelType* next() const {
                return static_cast<const PixelType*>(comps + PIXEL_STEP);
            }
            
            PixelType* advance(int32_t n) {
                return static_cast<PixelType*>(comps + n * PIXEL_STEP);
            }
            
            const PixelType* advance(int32_t n) const {
                return static_cast<const PixelType*>(comps + n * PIXEL_STEP);
            }
        };
        
        PixelFormatAlphatBlendRGBA() : renderBuffer{0} { }
        explicit PixelFormatAlphatBlendRGBA(RenderBuffer& rb) : renderBuffer{&rb} { }
        void attach(RenderBuffer& rb) { this->renderBuffer = &rb; }
        
        template<class PixelFormat>
        bool attach(PixelFormat& pf, int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
            // TODO
            // RectI r{x1, y1, x2, y2};
            // if (r.clip(RectI{0, 0, pf.width() - 1, pf.height() - 1})) {
            //     int32_t stride = pf.stride();
            //     this->renderBuffer->attach(pf.)
            //     return true;
            // }
            // return false;
        }
        
        uint32_t getWidth() const { return this->renderBuffer->getWidth(); }
        uint32_t getHeight() const { return this->renderBuffer->getHeight(); }
        int32_t getStride() const { return this->renderBuffer->getStride(); }
        
        uint8_t getRowPtr(int32_t y) { return this->renderBuffer->getRowPtr(y); }
        const uint8_t getRowPtr(int32_t y) const  { return this->renderBuffer->getRowPtr(y); }
        RowData getRow(int y) const { return this->renderBuffer->getRow(y); }
        
        int8_t getPixPtr(int32_t x, int32_t y) {
            return this->renderBuffer->getRowPtr(y) + sizeof(ValueType) * (x * PIXEL_STEP);
        }
        
        const uint8_t* getPixPtr(int32_t x, int32_t y) const {
            return this->renderBuffer(y) + sizeof(ValueType) * (x * PIXEL_STEP);
        }
        
        PixelType* getPixelValuePtr(int32_t x, int32_t y, uint32_t len) {
            return static_cast<PixelType*>(this->renderBuffer->getRowPtr(x, y, len) + sizeof(ValueType) * (x * PIXEL_STEP));
        }
        
        const PixelType* getPixelValuePtr(int32_t x, int32_t y) const {
            uint8_t* p = this->renderBuffer->getRowPtr();
            return p ? static_cast<PixelType*>(p + sizeof(ValueType) * (x * PIXEL_STEP)) : 0;
        }
    private:
        void BlendPixels(PixelType* p, const ColorType& c, uint32_t, CoverType cover) {
            this->blerder.blendPixels(p->c, c.r, c.g, c.b, c.a, cover);
        }
        
        void BlendPixels(PixelType* p, const ColorType& c) {
            this->blerder.blendPixels(p->c, c.r, c.g, c.b, c.a);
        }
        
        void copyOrBlendPixels(PixelType* p, const ColorType& c, uint32_t cover) {
            if (!c.isTransparent()) {
                if (c.isOpaque() && cover == COVER_MASK) {
                    p->set(c.r, c.g, c.b, c.a);
                } else {
                    this->blender.blendPixels(p->c, c.r, c.g, c.b, c.a, cover);
                }
            }
        }
        
        void copyOrBlendPixels(PixelType* p, const ColorType& c) {
            if (!c.isTransparent()) {
                if (c.isOpaque()) {
                    p->set(c.r, c.g, c.b, c.a);
                } else {
                    this->blender.blendPixels(p->c, c.r, c.g, c.b, c.a);
                }
            }
        }
        
        RenderBufferT* renderBuffer;
        BlenderT blender;
    };
    
    typedef BlenderRGBA<RGBA8, OrderRGBA> BlenderRGBA32;    
    typedef PixelFormatAlphatBlendRGBA<BlenderRGBA32, RenderBuffer> PixelFormatRGBA32;
};
