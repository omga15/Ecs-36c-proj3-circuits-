#include "AdjacencyListGraph.h"
#include <vector>

struct CAdjacencyListGraph::SImplementation{
    struct SEdge{
        TVertexID DDestination;
        double DWeight;
    };

    struct SVertex{
        std::vector<SEdge> DEdges;
        TVertexTag DTag;
    };

    std::vector<SVertex> DVertices;
    std::vector<bool> DDeleted;

    SImplementation(const std::vector<CGraph::TVertexTag> &tags,const std::vector<std::tuple<CGraph::TVertexID, CGraph::TVertexID, double, bool>>& edges){
        for(auto &Tag : tags){
            AddVertex(Tag);
        }
        for(auto &Edge : edges){
            AddEdge(std::get<0>(Edge),std::get<1>(Edge),std::get<2>(Edge),std::get<3>(Edge));
        }
    }

    bool VertexIDIsValid(TVertexID id) const{
        return id < DVertices.size() && !DDeleted[id];
    }

    // Returns the total number of vertices in the graph.
    std::size_t VertexCount() const noexcept{
        return DVertices.size();
    }

    // Gets the tag associated with a vertex.
    // Returns true if the vertex exists and its tag was retrieved.
    bool GetVertexTag(TVertexID id, TVertexTag &tag) const noexcept{
        if(VertexIDIsValid(id)){
            tag = DVertices[id].DTag;
            return true;
        }
        return false;
    }


    // Gets all neighboring vertices connected to the specified vertex.
    // Returns true if the vertex exists and its neighbors were retrieved.
    bool GetVertexNeighbors(TVertexID id, std::vector<TVertexID> &neighbors) const noexcept{
        if(VertexIDIsValid(id)){
            neighbors.clear();
            for(auto &Edge : DVertices[id].DEdges){
                if(VertexIDIsValid(Edge.DDestination)){
                    neighbors.push_back(Edge.DDestination);
                }
            }
            return true;
        }
        return false;
    }


    // Checks whether an edge exists between two vertices.
    bool EdgeExists(TVertexID src, TVertexID dest) const noexcept{
        if(VertexIDIsValid(src) && VertexIDIsValid(dest)){
            for(auto &Edge : DVertices[src].DEdges){
                if(Edge.DDestination == dest){
                    return true;
                }
            }
        }
        return false;
    }


    // Gets the weight of an edge between two vertices.
    // Returns true if the edge exists and its weight was retrieved.
    bool GetEdgeWeight(TVertexID src, TVertexID dest, double &weight) const noexcept{
        if(VertexIDIsValid(src) && VertexIDIsValid(dest)){
            for(auto &Edge : DVertices[src].DEdges){
                if(Edge.DDestination == dest){
                    weight = Edge.DWeight;
                    return true;
                }
            }
        }
        return false;
    }


    // Performs an unweighted breadth-first search starting at the specified vertex.
    // Calls the callback for each visited vertex.
    void BreadthFirstSearchUnweighted(TVertexID start, const TTraversalFunction &callback) const noexcept{

    }


    // Performs a depth-first search starting at the specified vertex.
    // Calls the callback for each visited vertex.
    void DepthFirstSearch(TVertexID start, const TTraversalFunction &callback) const noexcept{

    }


    // Performs a weighted breadth-first search starting at the specified vertex.
    // Returns true if the search completes successfully.
    bool BreadthFirstSearchWeighted(TVertexID start, const TTraversalFunction &callback) const noexcept{

    }


    // Visits every vertex in the graph, calling the callback for each one.
    // Returns true if the traversal completes successfully.
    bool ForEach(const TTraversalFunction &callback) const noexcept{

    }



    // Adds a new vertex with the specified tag.
    // Returns the ID assigned to the new vertex.
    TVertexID AddVertex(TVertexTag tag) noexcept{
        TVertexID NewID = DVertices.size();
        DVertices.push_back({{},tag});
        DDeleted.push_back(false);
        return NewID;
    }


    // Removes the specified vertex from the graph.
    // Returns true if the vertex was successfully removed.
    bool RemoveVertex(TVertexID id) noexcept{
        if(VertexIDIsValid(id)){
            DDeleted[id] = true;
            return true;
        }
        return false;
    }


    // Changes the tag associated with the specified vertex.
    // Returns true if the vertex exists and its tag was updated.
    bool SetVertexTag(TVertexID id, TVertexTag tag) noexcept{
        if(VertexIDIsValid(id)){
            DVertices[id].DTag = tag;
            return true;
        }
        return false;
    }


    // Adds an edge between two vertices with the specified weight.
    // If bidir is true, the edge is added in both directions.
    // If no weight is provided, EdgeWeightInvalid is used.
    // Returns true if the edge was successfully added.
    bool AddEdge(TVertexID src, TVertexID dest, double weight = EdgeWeightInvalid, bool bidir = true) noexcept{
        if(VertexIDIsValid(src) && VertexIDIsValid(dest)){
            DVertices[src].DEdges.push_back({dest,weight});
            if(bidir){
                DVertices[dest].DEdges.push_back({src,weight});
            }
            return true;
        }
        return false;
    }


    // Removes the edge between two vertices.
    // Returns true if the edge was successfully removed.
    bool RemoveEdge(TVertexID src, TVertexID dest) noexcept{

    }


    // Changes the weight of an existing edge.
    // Returns true if the edge exists and its weight was updated.
    bool SetEdgeWeight(TVertexID src, TVertexID dest, double weight) noexcept{

    }


};

CAdjacencyListGraph::CAdjacencyListGraph(const std::vector<CGraph::TVertexTag>& tags,const std::vector<std::tuple<CGraph::TVertexID, CGraph::TVertexID, double, bool>>& edges){
    DImplementation = std::make_unique<SImplementation>(tags,edges);
}


// Virtual destructor for proper cleanup of derived graph classes.
CAdjacencyListGraph::~CAdjacencyListGraph(){

}


// Returns the total number of vertices in the graph.
std::size_t CAdjacencyListGraph::VertexCount() const noexcept{
    return DImplementation->VertexCount();
}


// Gets the tag associated with a vertex.
// Returns true if the vertex exists and its tag was retrieved.
bool CAdjacencyListGraph::GetVertexTag(TVertexID id, TVertexTag &tag) const noexcept{
    return DImplementation->GetVertexTag(id,tag);
}


// Gets all neighboring vertices connected to the specified vertex.
// Returns true if the vertex exists and its neighbors were retrieved.
bool CAdjacencyListGraph::GetVertexNeighbors(TVertexID id, std::vector<TVertexID> &neighbors) const noexcept{
    return DImplementation->GetVertexNeighbors(id,neighbors);
}


// Checks whether an edge exists between two vertices.
bool CAdjacencyListGraph::EdgeExists(TVertexID src, TVertexID dest) const noexcept{
    return DImplementation->EdgeExists(src,dest);
}


// Gets the weight of an edge between two vertices.
// Returns true if the edge exists and its weight was retrieved.
bool CAdjacencyListGraph::GetEdgeWeight(TVertexID src, TVertexID dest, double &weight) const noexcept{
    return DImplementation->GetEdgeWeight(src,dest,weight);
}


// Performs an unweighted breadth-first search starting at the specified vertex.
// Calls the callback for each visited vertex.
void CAdjacencyListGraph::BreadthFirstSearchUnweighted(TVertexID start, const TTraversalFunction &callback) const noexcept{
    DImplementation->BreadthFirstSearchUnweighted(start,callback);
}


// Performs a depth-first search starting at the specified vertex.
// Calls the callback for each visited vertex.
void CAdjacencyListGraph::DepthFirstSearch(TVertexID start, const TTraversalFunction &callback) const noexcept{
    DImplementation->DepthFirstSearch(start,callback);
}


// Performs a weighted breadth-first search starting at the specified vertex.
// Returns true if the search completes successfully.
bool CAdjacencyListGraph::BreadthFirstSearchWeighted(TVertexID start, const TTraversalFunction &callback) const noexcept{
    return DImplementation->BreadthFirstSearchWeighted(start,callback);
}


// Visits every vertex in the graph, calling the callback for each one.
// Returns true if the traversal completes successfully.
bool CAdjacencyListGraph::ForEach(const TTraversalFunction &callback) const noexcept{
    return DImplementation->ForEach(callback);
}



// Adds a new vertex with the specified tag.
// Returns the ID assigned to the new vertex.
CGraph::TVertexID CAdjacencyListGraph::AddVertex(TVertexTag tag) noexcept{
    return DImplementation->AddVertex(tag);
}


// Removes the specified vertex from the graph.
// Returns true if the vertex was successfully removed.
bool CAdjacencyListGraph::RemoveVertex(TVertexID id) noexcept{
    return DImplementation->RemoveVertex(id);
}


// Changes the tag associated with the specified vertex.
// Returns true if the vertex exists and its tag was updated.
bool CAdjacencyListGraph::SetVertexTag(TVertexID id, TVertexTag tag) noexcept{
    return DImplementation->SetVertexTag(id,tag);
}


// Adds an edge between two vertices with the specified weight.
// If bidir is true, the edge is added in both directions.
// If no weight is provided, EdgeWeightInvalid is used.
// Returns true if the edge was successfully added.
bool CAdjacencyListGraph::AddEdge(TVertexID src, TVertexID dest, double weight, bool bidir) noexcept{
    return DImplementation->AddEdge(src,dest,weight,bidir);
}


// Removes the edge between two vertices.
// Returns true if the edge was successfully removed.
bool CAdjacencyListGraph::RemoveEdge(TVertexID src, TVertexID dest) noexcept{
    return DImplementation->RemoveEdge(src,dest);
}


// Changes the weight of an existing edge.
// Returns true if the edge exists and its weight was updated.
bool CAdjacencyListGraph::SetEdgeWeight(TVertexID src, TVertexID dest, double weight) noexcept{
    return DImplementation->SetEdgeWeight(src,dest,weight);
}
