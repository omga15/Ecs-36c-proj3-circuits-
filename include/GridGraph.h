#ifndef GRID_GRAPH_H
#define GRID_GRAPH_H

#include <cstdint>
#include "MutableGraph.h"

// Type used to represent a coordinate within the grid.
using TGridCoordinate = std::size_t;

// Stores the X and Y coordinates of a grid location.
struct SGridLocation{
    TGridCoordinate DX;
    TGridCoordinate DY;
};

// Defines the eight possible directions from a grid vertex.
enum class EGridDirection: std::uint8_t{
    N  = 0,  // North
    NE = 1,  // Northeast
    E  = 2,  // East
    SE = 3,  // Southeast
    S  = 4,  // South
    SW = 5,  // Southwest
    W  = 6,  // West
    NW = 7   // Northwest
};

// Bitmask type used to represent which directions a vertex can connect to.
using TGridVertexConnectionMask = std::uint8_t;

// Converts a grid direction into its corresponding bitmask bit.
constexpr TGridVertexConnectionMask GridDirectionBit(EGridDirection direction) noexcept {
    return TGridVertexConnectionMask{1}<<static_cast<std::uint8_t>(direction);
}

// Bitmask representing connections in all eight directions.
constexpr const TGridVertexConnectionMask GridVertexConnectionAll = GridDirectionBit(EGridDirection::N)  |
                                                                    GridDirectionBit(EGridDirection::NE) |
                                                                    GridDirectionBit(EGridDirection::E)  |
                                                                    GridDirectionBit(EGridDirection::SE) |
                                                                    GridDirectionBit(EGridDirection::S)  |
                                                                    GridDirectionBit(EGridDirection::SW) |
                                                                    GridDirectionBit(EGridDirection::W)  |
                                                                    GridDirectionBit(EGridDirection::NW);

// Bitmask representing no connections in any direction.
constexpr const TGridVertexConnectionMask GridVertexConnectionNone = 0;

// Stores the connection rules for vertices based on their row and column parity.
struct SGridConnectionPattern{
    // Allowed connections for vertices in even rows and even columns.
    TGridVertexConnectionMask DEvenRowEvenColumn = GridVertexConnectionAll;

    // Allowed connections for vertices in even rows and odd columns.
    TGridVertexConnectionMask DEvenRowOddColumn  = GridVertexConnectionAll;

    // Allowed connections for vertices in odd rows and even columns.
    TGridVertexConnectionMask DOddRowEvenColumn  = GridVertexConnectionAll;

    // Allowed connections for vertices in odd rows and odd columns.
    TGridVertexConnectionMask DOddRowOddColumn   = GridVertexConnectionAll;
};

// Interface for graphs organized as a two-dimensional grid.
class CGridGraph : public virtual CGraph{
    public:
        // Virtual destructor for proper cleanup of derived graph classes.
        virtual ~CGridGraph() = default;

        // Returns the connection pattern used by the grid.
        virtual SGridConnectionPattern GetGridConnectionPattern() const noexcept = 0;

        // Returns the vertex ID at the specified grid location.
        // Returns VertexIDInvalid if no vertex exists at that location.
        virtual TVertexID GetVertex(const SGridLocation &location) const noexcept = 0;

        // Returns the vertex ID at the specified X and Y coordinates.
        // Returns VertexIDInvalid if no vertex exists at that location.
        virtual TVertexID GetVertex(TGridCoordinate x, TGridCoordinate y) const noexcept = 0;

        // Returns the connection mask describing the allowed directions for a vertex.
        virtual TGridVertexConnectionMask GetVertexConnectionMask(TVertexID id) const noexcept = 0;

        // Gets the grid location of the specified vertex.
        // Returns true if the vertex exists and its location was retrieved.
        virtual bool GetVertexLocation(TVertexID id, SGridLocation &location) const noexcept = 0;

        // Returns the width of the grid.
        virtual TGridCoordinate Width() const noexcept = 0;

        // Returns the height of the grid.
        virtual TGridCoordinate Height() const noexcept = 0;
};

// Interface for grid graphs that allow grid connections and vertex tags to be modified.
class CMutableGridGraph : public virtual CGridGraph, public virtual CVertexTagMutableGraph{
    public:
        // Virtual destructor for proper cleanup of derived graph classes.
        virtual ~CMutableGridGraph() = default;

        // Changes the connection pattern used by the grid.
        // Returns true if the pattern was successfully updated.
        virtual bool SetGridConnectionPattern(const SGridConnectionPattern &pattern) noexcept = 0;

        // Sets the allowed connection directions for a specific vertex.
        // Returns true if the vertex exists and its connection mask was updated.
        virtual bool SetVertexConnectionMask(TVertexID id, TGridVertexConnectionMask mask) noexcept = 0;

        // Removes all allowed connections from the specified vertex.
        // Returns true if the vertex exists and its connection mask was cleared.
        virtual bool ClearVertexConnectionMask(TVertexID id) noexcept = 0;
};

#endif