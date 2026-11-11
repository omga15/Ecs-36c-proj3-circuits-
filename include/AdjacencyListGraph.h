#ifndef ADJACENCY_LIST_GRAPH_H
#define ADJACENCY_LIST_GRAPH_H

#include "MutableGraph.h"
#include <memory>

class CAdjacencyListGraph : public virtual CFullMutableGraph{
    private:
        struct SImplementation;
        std::unique_ptr<SImplementation> DImplementation;
    public:
        CAdjacencyListGraph(const std::vector<CGraph::TVertexTag>& tags,const std::vector<std::tuple<CGraph::TVertexID, CGraph::TVertexID, double, bool>>& edges);
        
        // Virtual destructor for proper cleanup of derived graph classes.
        ~CAdjacencyListGraph();

        // Returns the total number of vertices in the graph.
        std::size_t VertexCount() const noexcept override;

        // Gets the tag associated with a vertex.
        // Returns true if the vertex exists and its tag was retrieved.
        bool GetVertexTag(TVertexID id, TVertexTag &tag) const noexcept override;

        // Gets all neighboring vertices connected to the specified vertex.
        // Returns true if the vertex exists and its neighbors were retrieved.
        bool GetVertexNeighbors(TVertexID id, std::vector<TVertexID> &neighbors) const noexcept override;

        // Checks whether an edge exists between two vertices.
        bool EdgeExists(TVertexID src, TVertexID dest) const noexcept override;

        // Gets the weight of an edge between two vertices.
        // Returns true if the edge exists and its weight was retrieved.
        bool GetEdgeWeight(TVertexID src, TVertexID dest, double &weight) const noexcept override;

        // Performs an unweighted breadth-first search starting at the specified vertex.
        // Calls the callback for each visited vertex.
        void BreadthFirstSearchUnweighted(TVertexID start, const TTraversalFunction &callback) const noexcept override;

        // Performs a depth-first search starting at the specified vertex.
        // Calls the callback for each visited vertex.
        void DepthFirstSearch(TVertexID start, const TTraversalFunction &callback) const noexcept override;

        // Performs a weighted breadth-first search starting at the specified vertex.
        // Returns true if the search completes successfully.
        bool BreadthFirstSearchWeighted(TVertexID start, const TTraversalFunction &callback) const noexcept override;

        // Visits every vertex in the graph, calling the callback for each one.
        // Returns true if the traversal completes successfully.
        bool ForEach(const TTraversalFunction &callback) const noexcept override;


        // Adds a new vertex with the specified tag.
        // Returns the ID assigned to the new vertex.
        TVertexID AddVertex(TVertexTag id) noexcept override;

        // Removes the specified vertex from the graph.
        // Returns true if the vertex was successfully removed.
        bool RemoveVertex(TVertexID id) noexcept override;

        // Changes the tag associated with the specified vertex.
        // Returns true if the vertex exists and its tag was updated.
        bool SetVertexTag(TVertexID id, TVertexTag tag) noexcept override;

        // Adds an edge between two vertices with the specified weight.
        // If bidir is true, the edge is added in both directions.
        // If no weight is provided, EdgeWeightInvalid is used.
        // Returns true if the edge was successfully added.
        bool AddEdge(TVertexID src, TVertexID dest, double weight = EdgeWeightInvalid, bool bidir = true) noexcept override;

        // Removes the edge between two vertices.
        // Returns true if the edge was successfully removed.
        bool RemoveEdge(TVertexID src, TVertexID dest) noexcept override;

        // Changes the weight of an existing edge.
        // Returns true if the edge exists and its weight was updated.
        bool SetEdgeWeight(TVertexID src, TVertexID dest, double weight) noexcept override;

};

#endif