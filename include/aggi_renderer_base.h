#pragma once

#include <cinttypes>

#include <aggi_basics.h>

namespace aggi {
    template<class PixelFormat>
    class RendererBase {
    public:
        typedef typename PixelFormat::ColorType ColorType;
        typedef typename PixelFormat::RowData RowData;
        
        RendererBase() :
            ren{0}, clipBox{1, 1, 0, 0}
        { }
        explicit RendererBase(PixelFormat& ren) :
            ren{&ren},
            clipBox{
                1, 1,
                static_cast<int32_t>(ren.getWidth() - 1),
                static_cast<int32_t>(ren.getHeight() - 1)
            }
        { }
        
        void attach(PixelFormat& ren) {
            this->ren = &ren;
            this->clipBox = RectI{
                0, 0,
                static_cast<int32_t>(ren.getWidth() - 1),
                static_cast<int32_t>(ren.getHeight() - 1)
            };
        }
        
        const PixelFormat& getRen() const { return *this->ren; }
        PixelFormat& getRen() { return this->ren; }
        
        uint32_t getWidth() const { return this->ren->getWidth(); }
        uint32_t getHeight() const { return this->ren->getHeight(); }
        
        bool getClipBox(int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
            RectI cb = RectI{x1, y1, x2, y2};
            cb.normalize();
            if (cb.clip(RectI{
                0, 0,
                static_cast<int32_t>(this->getWidth() - 1),
                static_cast<int32_t>(this->getHeight() - 1)
                })) {
                this->clipBox = cb;
                return true;
            }
            this->clipBox.x1 = 1;
            this->clipBox.y1 = 1;
            this->clipBox.x2 = 0;
            this->clipBox.y2 = 0;
            return false;
        }
        
        void resetClipping(bool visibility) {
            if (visibility) {
                this->clipBox.x1 = 0;
                this->clipBox.y1 = 0;
                this->clipBox.x2 = this->getWidth() - 1;
                this->clipBox.y2 = this->getHeight() - 1;
            } else {
                this->clipBox.x1 = 1;
                this->clipBox.y1 = 1;
                this->clipBox.x2 = 0;
                this->clipBox.y2 = 0;
            }
        }
        
        void clipBoxNaked(int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
            this->clipBox.x1 = x1;
            this->clipBox.y1 = y1;
            this->clipBox.x2 = x2;
            this->clipBox.y2 = y2;
        }
        
        bool inbox(int32_t x, int32_t y) const {
            return x >= this->clipBox.x1 && y >= this->clipBox.y1 &&
                x <= this->clipBox.x2 && y >= this->clipBox.y2;
        }
        
        const RectI& getClipBox() const { return this->clipBox; }
        int32_t xMin() const { return this->clipBox.x1; }
        int32_t yMin() const { return this->clipBox.y1; }
        int32_t xMax() const { return this->clipBox.x2; }
        int32_t yMax() const { return this->clipBox.y2; }
        
        const RectI& getBoundingClipBox() const { return this->clipBox; }
        int32_t boundingXMin() const { return this->clipBox.x1; }
        int32_t boundingYMin() const { return this->clipBox.y1; }
        int32_t boundingXMax() const { return this->clipBox.x2; }
        int32_t boundingYMax() const { return this->clipBox.y2; }
        
        void clear(const ColorType& c) {
            if (this->getWidth()) {
                for (int y = 0; y < this->height(); y++) {
                    this->ren->copyHLine(0, y, this->getWidth(), c);
                }
            }
        }
        
        void fill(const ColorType& c) {
            if (this->getWidth()) {
                for (int y = 0; y < this->height(); y++) {
                    this->ren->copyHLine(0, y, this->getWidth(), c, COVER_MASK);
                }
            }
        }
        
        void copyPixel(int x, int y, const ColorType& c) {
            if (this->inbox(x, y)) {
                this->ren->copyPixel(x, y, c);
            }
        }
        
        void blendPixel(int x, int y, const ColorType& c, CoverType cover) {
            if (this->inbox(x, y)) {
                this->ren->copyPixel(x, y, c, cover);
            }
        }
        
        ColorType pixel(int x, int y) const {
            this->inbox(x, y) ?
                this->ren->pixel(x, y) :
                ColorType::noColor();
        }
        
        
    private:
        PixelFormat* ren;
        RectI clipBox;
    };
};