#include "GridGraph.h"
#include <memory>

class CGridGraphImpl : public CGridGraph{
    private:
        
        struct SImplementation{

            std::size_t VertexCount() const noexcept{
                return 0;
            }

            bool GetVertexTag(TVertexID id, TVertexTag &tag) const noexcept{
                return false;
            }

            bool GetVertexNeighbors(TVertexID id, std::vector<TVertexID> &neighbors) const noexcept{
                return false;
            }

            bool EdgeExists(TVertexID src, TVertexID dest) const noexcept{
                return false;
            }

            bool GetEdgeWeight(TVertexID src, TVertexID dest, double &weight) const noexcept{
                return false;
            }

            void BreadthFirstSearchUnweighted(TVertexID start, const TTraversalFunction &callback) const noexcept{
            }

            void DepthFirstSearch(TVertexID start, const TTraversalFunction &callback) const noexcept{
            }

            bool BreadthFirstSearchWeighted(TVertexID start, const TTraversalFunction &callback) const noexcept{
                return false;
            }

            bool ForEach(const TTraversalFunction &callback) const noexcept{
                return false;
            }

            SGridConnectionPattern GetGridConnectionPattern() const noexcept{
                return SGridConnectionPattern();
            }

            TVertexID GetVertex(const SGridLocation &location) const noexcept{
                return VertexIDInvalid;
            }

            TVertexID GetVertex(TGridCoordinate x, TGridCoordinate y) const noexcept{
                return VertexIDInvalid;
            }

            TGridVertexConnectionMask GetVertexConnectionMask(TVertexID id) const noexcept{
                return GridVertexConnectionNone;
            }

            bool GetVertexLocation(TVertexID id, SGridLocation &location) const noexcept{
                return false;
            }

            TGridCoordinate Width() const noexcept{
                return 0;
            }

            TGridCoordinate Height() const noexcept{
                return 0;
            }
        };

        std::unique_ptr<SImplementation> DImplementation;

    public:
        CGridGraphImpl();
        ~CGridGraphImpl() override;

        // CGraph
        std::size_t VertexCount() const noexcept override;
        bool GetVertexTag(TVertexID id, TVertexTag &tag) const noexcept override;
        bool GetVertexNeighbors(TVertexID id, std::vector<TVertexID> &neighbors) const noexcept override;
        bool EdgeExists(TVertexID src, TVertexID dest) const noexcept override;
        bool GetEdgeWeight(TVertexID src, TVertexID dest, double &weight) const noexcept override;
        void BreadthFirstSearchUnweighted(TVertexID start, const TTraversalFunction &callback) const noexcept override;
        void DepthFirstSearch(TVertexID start, const TTraversalFunction &callback) const noexcept override;
        bool BreadthFirstSearchWeighted(TVertexID start, const TTraversalFunction &callback) const noexcept override;
        bool ForEach(const TTraversalFunction &callback) const noexcept override;

        // CGridGraph
        SGridConnectionPattern GetGridConnectionPattern() const noexcept override;
        TVertexID GetVertex(const SGridLocation &location) const noexcept override;
        TVertexID GetVertex(TGridCoordinate x, TGridCoordinate y) const noexcept override;
        TGridVertexConnectionMask GetVertexConnectionMask(TVertexID id) const noexcept override;
        bool GetVertexLocation(TVertexID id, SGridLocation &location) const noexcept override;
        TGridCoordinate Width() const noexcept override;
        TGridCoordinate Height() const noexcept override;
};

CGridGraphImpl::CGridGraphImpl() : DImplementation(std::make_unique<SImplementation>()){
}

CGridGraphImpl::~CGridGraphImpl() = default;

// Returns the total number of vertices in the graph.
std::size_t CGridGraphImpl::VertexCount() const noexcept{
    return DImplementation->VertexCount();
}

// Gets the tag associated with a vertex.
// Returns true if the vertex exists and its tag was retrieved.
bool CGridGraphImpl::GetVertexTag(TVertexID id, TVertexTag &tag) const noexcept{
    return DImplementation->GetVertexTag(id,tag);
}

// Gets all neighboring vertices connected to the specified vertex.
// Returns true if the vertex exists and its neighbors were retrieved.
bool CGridGraphImpl::GetVertexNeighbors(TVertexID id, std::vector<TVertexID> &neighbors) const noexcept{
    return DImplementation->GetVertexNeighbors(id,neighbors);
}

// Checks whether an edge exists between two vertices.
bool CGridGraphImpl::EdgeExists(TVertexID src, TVertexID dest) const noexcept{
    return DImplementation->EdgeExists(src,dest);
}

// Gets the weight of an edge between two vertices.
bool CGridGraphImpl::GetEdgeWeight(TVertexID src, TVertexID dest, double &weight) const noexcept{
    return DImplementation->GetEdgeWeight(src,dest,weight);
}

// Performs an unweighted breadth-first search.
void CGridGraphImpl::BreadthFirstSearchUnweighted(TVertexID start, const TTraversalFunction &callback) const noexcept{
    DImplementation->BreadthFirstSearchUnweighted(start,callback);
}

// Performs a depth-first search.
void CGridGraphImpl::DepthFirstSearch(TVertexID start, const TTraversalFunction &callback) const noexcept{
    DImplementation->DepthFirstSearch(start,callback);
}

// Performs a weighted breadth-first search.
bool CGridGraphImpl::BreadthFirstSearchWeighted(TVertexID start, const TTraversalFunction &callback) const noexcept{
    return DImplementation->BreadthFirstSearchWeighted(start,callback);
}

// Visits every vertex in the graph.
bool CGridGraphImpl::ForEach(const TTraversalFunction &callback) const noexcept{
    return DImplementation->ForEach(callback);
}

// Returns the connection pattern used by the grid.
SGridConnectionPattern CGridGraphImpl::GetGridConnectionPattern() const noexcept{
    return DImplementation->GetGridConnectionPattern();
}

// Returns the vertex ID at the specified grid location.
CGraph::TVertexID CGridGraphImpl::GetVertex(const SGridLocation &location) const noexcept{
    return DImplementation->GetVertex(location);
}

// Returns the vertex ID at the specified X and Y coordinates.
CGraph::TVertexID CGridGraphImpl::GetVertex(TGridCoordinate x, TGridCoordinate y) const noexcept{
    return DImplementation->GetVertex(x,y);
}

// Returns the connection mask describing the allowed directions for a vertex.
TGridVertexConnectionMask CGridGraphImpl::GetVertexConnectionMask(TVertexID id) const noexcept{
    return DImplementation->GetVertexConnectionMask(id);
}

// Gets the grid location of the specified vertex.
bool CGridGraphImpl::GetVertexLocation(TVertexID id, SGridLocation &location) const noexcept{
    return DImplementation->GetVertexLocation(id,location);
}

// Returns the width of the grid.
TGridCoordinate CGridGraphImpl::Width() const noexcept{
    return DImplementation->Width();
}

// Returns the height of the grid.
TGridCoordinate CGridGraphImpl::Height() const noexcept{
    return DImplementation->Height();
}