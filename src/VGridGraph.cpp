#include "VGridGraph.h"
#include <vector>
#include <cmath>


struct VGridGraph::SImplementation {

    VGridGraph& DGraph;

    std::vector<uint16_t> DMask;

    TGridCoordinate DWidth;
    TGridCoordinate DHeight;

    SGridConnectionPattern DPattern;

    SImplementation(VGridGraph& graph, TGridCoordinate width, TGridCoordinate height) : DGraph(graph), DWidth(width), DHeight(height), DMask(width*height, 0) {}

    SGridConnectionPattern GetGridConnectionPattern() const noexcept { return DPattern; }

    TVertexID GetVertex(const SGridLocation &location) const noexcept {

        return GetVertex(location.DX, location.DY);

    }

    TVertexID GetVertex(TGridCoordinate x, TGridCoordinate y) const noexcept {

        if(x < DWidth && y < DHeight) return y * DWidth + x;

        return VertexIDInvalid;

    }

    TGridVertexConnectionMask GetVertexConnectionMask(TVertexID id) const noexcept {

        return id < DMask.size() ? static_cast<TGridVertexConnectionMask>(DMask[id] & 0xFF) : GridVertexConnectionNone;

    }


    bool GetVertexLocation(VGridGraph::TVertexID id, SGridLocation &location) const noexcept {

        if(id < DWidth * DHeight) {

            location.DX = id % DWidth;
            
            location.DY = id / DWidth;

            return true;

        }

        return false;

    }


    TGridCoordinate Width() const noexcept { return DWidth; }

    TGridCoordinate Height() const noexcept { return DHeight; }

    bool SetGridConnectionPattern(const SGridConnectionPattern &pattern) noexcept {

        DPattern = pattern;

        for(TGridCoordinate y = 0; y < DHeight; y++) {

            for(TGridCoordinate x = 0; x < DWidth; x++) {

                TVertexID id = GetVertex(x, y);

                if((DMask[id] & 256) != 0) continue;

                TGridVertexConnectionMask mask = (y % 2 == 0)
                    ? ((x % 2 == 0) ? pattern.DEvenRowEvenColumn : pattern.DEvenRowOddColumn)
                    : ((x % 2 == 0) ? pattern.DOddRowEvenColumn : pattern.DOddRowOddColumn);

                SetVertexConnectionMask(id, mask);

                DMask[id] &= 0xFF; 
            }
        }

        return true;
    }


    bool SetVertexConnectionMask(TVertexID id, TGridVertexConnectionMask mask) noexcept { 
        
        SGridLocation location;

        if(!GetVertexLocation(id, location)) return false;

        static constexpr int DDeltaX[8] = {0, 1, 1, 1, 0, -1, -1, -1};
        static constexpr int DDeltaY[8] = {-1, -1, 0, 1, 1, 1, 0, -1};

        TGridVertexConnectionMask validMask = GridVertexConnectionNone;

        for(std::uint8_t index = 0; index < 8; index++) {

            auto direction = static_cast<EGridDirection>(index);
            auto bit = GridDirectionBit(direction);

            int nx = static_cast<int>(location.DX) + DDeltaX[index];
            int ny = static_cast<int>(location.DY) + DDeltaY[index];

            bool inBounds = (nx >= 0 && static_cast<TGridCoordinate>(nx) < DWidth) && (ny >= 0 && static_cast<TGridCoordinate>(ny) < DHeight);

            if(!inBounds) continue;

            TVertexID neighbor = GetVertex(static_cast<TGridCoordinate>(nx), static_cast<TGridCoordinate>(ny));

            bool shouldConnect = (bit & mask) != 0;
            bool exists = DGraph.EdgeExists(id, neighbor);

            if(shouldConnect) {

                validMask |= bit;

                if(!exists) {

                    double weight = (DDeltaX[index] != 0 && DDeltaY[index] != 0) ? std::sqrt(2.0) : 1.0;

                    DGraph.AddEdge(id, neighbor, weight, false);

                }

            }
            else if(exists) {

                DGraph.RemoveEdge(id, neighbor);

            }

        }
    
        DMask[id] = validMask | 256;

        return true;

    }

    bool ClearVertexConnectionMask(TVertexID id) noexcept {

        if(id >= DMask.size()) return false;

        DMask[id] &= 0xFF; 

        return SetGridConnectionPattern(DPattern);
    }

};

VGridGraph::VGridGraph(TGridCoordinate width, TGridCoordinate height, const SGridConnectionPattern& pattern) : CAdjacencyListGraph(std::vector<CGraph::TVertexTag>(width*height), {}) {

    DImplementation = std::make_unique<SImplementation>(*this, width, height);

    SetGridConnectionPattern(pattern);

}

VGridGraph::~VGridGraph() = default;

// Returns the connection pattern used by the grid.
SGridConnectionPattern VGridGraph::GetGridConnectionPattern() const noexcept { return DImplementation->GetGridConnectionPattern(); }

// Returns the vertex ID at the specified grid location.
// Returns VertexIDInvalid if no vertex exists at that location.
VGridGraph::TVertexID VGridGraph::GetVertex(const SGridLocation &location) const noexcept { return DImplementation->GetVertex(location); }

// Returns the vertex ID at the specified X and Y coordinates.
// Returns VertexIDInvalid if no vertex exists at that location.
VGridGraph::TVertexID VGridGraph::GetVertex(TGridCoordinate x, TGridCoordinate y) const noexcept { return DImplementation->GetVertex(x, y); }

// Returns the connection mask describing the allowed directions for a vertex.
TGridVertexConnectionMask VGridGraph::GetVertexConnectionMask(TVertexID id) const noexcept { return DImplementation->GetVertexConnectionMask(id); }

// Gets the grid location of the specified vertex.
// Returns true if the vertex exists and its location was retrieved.
bool VGridGraph::GetVertexLocation(VGridGraph::TVertexID id, SGridLocation &location) const noexcept { return DImplementation->GetVertexLocation(id, location); }

// Returns the width of the grid.
TGridCoordinate VGridGraph::Width() const noexcept { return DImplementation->Width(); }

// Returns the height of the grid.
TGridCoordinate VGridGraph::Height() const noexcept { return DImplementation->Height(); }

// Changes the connection pattern used by the grid.
// Returns true if the pattern was successfully updated.
bool VGridGraph::SetGridConnectionPattern(const SGridConnectionPattern &pattern) noexcept { return DImplementation->SetGridConnectionPattern(pattern); }

// Sets the allowed connection directions for a specific vertex.
// Returns true if the vertex exists and its connection mask was updated.
bool VGridGraph::SetVertexConnectionMask(TVertexID id, TGridVertexConnectionMask mask) noexcept { return DImplementation->SetVertexConnectionMask(id, mask); }

// Removes all allowed connections from the specified vertex.
// Returns true if the vertex exists and its connection mask was cleared.
bool VGridGraph::ClearVertexConnectionMask(TVertexID id) noexcept { return DImplementation->ClearVertexConnectionMask(id); }