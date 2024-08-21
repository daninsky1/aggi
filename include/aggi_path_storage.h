#pragma once

#include <cinttypes>

namespace aggi {
    template<class T, uint32_t BlockShift = 8, uint32_t BlockPool = 256>
    class VertexBlockStorage {
    public:
        enum BlockScaleE {
            BLOCK_SHIFT = BlockShift,
            BLOCK_SIZE = 1 << BLOCK_SHIFT,
            BLOCK_MASK = BLOCK_SIZE - 1,
            BLOCK_POOL = BlockPool
        };
        
        typedef T ValueType;
        typedef VertexBlockStorage<T, BlockShift, BlockPool> SelfType;

        VertexBlockStorage();
        ~VertexBlockStorage();
        VertexBlockStorage(const SelfType&);
        const VertexBlockStorage& operator=(const VertexBlockStorage&);
        
        void removeAll();
        void freeAll();
        
        void addVertex(double x, double y, uint32_t cmd);
        void modifyVertex(uint32_t idx, double x, double y);
        void modifyVertex(uint32_t idx, double x, double y, uint32_t cmd);
        void modifyCommand(uint32_t idx, uint32_t cmd);
        void swapVertices(uint32_t v1, uint32_t v2);
        
        uint32_t lastCommand() const;
        uint32_t lastVertex(double* x, double* y) const;
        uint32_t prevVertex(double* x, double* y) const;
        
        double lastX() const;
        double lastY() const;
        
        uint32_t totalVertices() const;
        uint32_t vertex(uint32_t idx, double* x, double* y) const;
        uint32_t totalVertices(uint32_t idx) const;
    private:
        void allocateBlock(uint32_t nb);
        uint8_t storagePtrs(T** xyPtr);
    
        uint32_t totalVertices;
        uint32_t totalBlocks;
        uint32_t maxBlocks;
        T** coordinateBlocks;
        uint8_t cmdBlocks;
    };
    
    template<class VertexContainer>
    class PathBase {
    public:
        typedef VertexContainer ContainerType;
        typedef PathBase<VertexContainer> SelfType;
        
        PathBase() 
    private:
        uint32_t perceivePolygonOrientation(uint32_t start, uint32_t end);
        void invertPolygon(uint32_t start, uint32_t end);
        
        VertexContainer vertices;
        uint32_t iterator;
    };
    
    typedef PathBase<VertexBlockStorage<double>> PathStorage;
};