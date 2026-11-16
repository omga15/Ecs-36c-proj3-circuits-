#ifndef VGRIDGRAPH_H
#define VGRIDGRAPH_H

#include "GridGraph.h"
#include "AdjacencyListGraph.h"
#include <memory>

class VGridGraph : public CMutableGridGraph, public CAdjacencyListGraph {

    private: 
        struct SImplementation;
        std::unique_ptr<SImplementation> DImplementation;
    
    public: 
        VGridGraph(TGridCoordinate width, TGridCoordinate height, const SGridConnectionPattern& pattern = SGridConnectionPattern{});

        ~VGridGraph();
         // Returns the connection pattern used by the grid.
        SGridConnectionPattern GetGridConnectionPattern() const noexcept override;

        // Returns the vertex ID at the specified grid location.
        // Returns VertexIDInvalid if no vertex exists at that location.
        TVertexID GetVertex(const SGridLocation &location) const noexcept override;

        // Returns the vertex ID at the specified X and Y coordinates.
        // Returns VertexIDInvalid if no vertex exists at that location.
        TVertexID GetVertex(TGridCoordinate x, TGridCoordinate y) const noexcept override;

        // Returns the connection mask describing the allowed directions for a vertex.
        TGridVertexConnectionMask GetVertexConnectionMask(TVertexID id) const noexcept override;

        // Gets the grid location of the specified vertex.
        // Returns true if the vertex exists and its location was retrieved.
        bool GetVertexLocation(TVertexID id, SGridLocation &location) const noexcept override;

        // Returns the width of the grid.
        TGridCoordinate Width() const noexcept override;

        // Returns the height of the grid.
        TGridCoordinate Height() const noexcept override;

        // Changes the connection pattern used by the grid.
        // Returns true if the pattern was successfully updated.
        bool SetGridConnectionPattern(const SGridConnectionPattern &pattern) noexcept override;

        // Sets the allowed connection directions for a specific vertex.
        // Returns true if the vertex exists and its connection mask was updated.
        bool SetVertexConnectionMask(TVertexID id, TGridVertexConnectionMask mask) noexcept override;

        // Removes all allowed connections from the specified vertex.
        // Returns true if the vertex exists and its connection mask was cleared.
        bool ClearVertexConnectionMask(TVertexID id) noexcept override;

}; 

#endif