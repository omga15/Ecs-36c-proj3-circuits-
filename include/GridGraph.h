#ifndef GRID_GRAPH_H
#define GRID_GRAPH_H

#include <cstdint>
#include "MutableGraph.h"

using TGridCoordinate    = std::size_t;

struct SGridLocation{
    TGridCoordinate DX;
    TGridCoordinate DY;
};

enum class EGridDirection: std::uint8_t{
    N  = 0,
    NE = 1,
    E  = 2,
    SE = 3,
    S  = 4,
    SW = 5,
    W  = 6,
    NW = 7
};

using TGridVertexConnectionMask = std::uint8_t;

constexpr TGridVertexConnectionMask GridDirectionBit(EGridDirection direction) noexcept {
    return TGridVertexConnectionMask{1}<<static_cast<std::uint8_t>(direction);
}

constexpr const TGridVertexConnectionMask GridVertexConnectionAll = GridDirectionBit(EGridDirection::N)  |
                                                                    GridDirectionBit(EGridDirection::NE) |
                                                                    GridDirectionBit(EGridDirection::E)  |
                                                                    GridDirectionBit(EGridDirection::SE) |
                                                                    GridDirectionBit(EGridDirection::S)  |
                                                                    GridDirectionBit(EGridDirection::SW) |
                                                                    GridDirectionBit(EGridDirection::W)  |
                                                                    GridDirectionBit(EGridDirection::NW);

constexpr const TGridVertexConnectionMask GridVertexConnectionNone = 0;

struct SGridConnectionPattern{
    TGridVertexConnectionMask DEvenRowEvenColumn = GridVertexConnectionAll;
    TGridVertexConnectionMask DEvenRowOddColumn  = GridVertexConnectionAll;
    TGridVertexConnectionMask DOddRowEvenColumn  = GridVertexConnectionAll;
    TGridVertexConnectionMask DOddRowOddColumn   = GridVertexConnectionAll;
};

class CGridGraph : public virtual CGraph{
    public:
        virtual ~CGridGraph() = default;

        virtual SGridConnectionPattern GetGridConnectionPattern() const noexcept = 0;
        virtual TVertexID GetVertex(const SGridLocation &location) const noexcept = 0;
        virtual TVertexID GetVertex(TGridCoordinate x, TGridCoordinate y) const noexcept = 0;
        virtual TGridVertexConnectionMask GetVertexConnectionMask(TVertexID id) const noexcept = 0;
        virtual bool GetVertexLocation(TVertexID id, SGridLocation &location) const noexcept = 0;
        virtual TGridCoordinate Width() const noexcept = 0;
        virtual TGridCoordinate Height() const noexcept = 0;
};

class CMutableGridGraph : public virtual CGridGraph, public virtual CVertexTagMutableGraph{
    public:
        virtual ~CMutableGridGraph() = default;

        virtual bool SetGridConnectionPattern(const SGridConnectionPattern &pattern) noexcept = 0;
        virtual bool SetVertexConnectionMask(TVertexID id, TGridVertexConnectionMask mask) noexcept = 0;
        virtual bool ClearVertexConnectionMask(TVertexID id) noexcept = 0;

        virtual bool SetVertexTag(TVertexID id, TVertexTag tag) noexcept = 0;
};

#endif