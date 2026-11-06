#ifndef GRAPH_H
#define GRAPH_H

#include <cstddef>
#include <vector>
#include <limits>
#include <functional>

// Abstract base class representing a graph.
class CGraph{
    public:
        // Controls how graph traversal continues after visiting a vertex.
        enum class EGraphTraversal{
            Continue,       // Continue visiting neighboring vertices.
            SkipNeighbors,  // Do not visit this vertex's neighbors.
            Stop            // Stop the entire traversal.
        };

        // Type used to identify vertices in the graph.
        using TVertexID = std::size_t;

        // Type used to store a tag associated with a vertex.
        using TVertexTag = std::size_t;

        // Invalid vertex ID value.
        constexpr const static TVertexID VertexIDInvalid = std::numeric_limits<TVertexID>::max();

        // Invalid edge weight value.
        constexpr const static double EdgeWeightInvalid = std::numeric_limits<double>::quiet_NaN();

        // Function called during graph traversal.
        // Receives the current vertex, its parent, traversal depth, and distance from the start.
        using TTraversalFunction = std::function<EGraphTraversal(TVertexID it, TVertexID parent, std::size_t depth, double distance)>;

        // Virtual destructor for proper cleanup of derived graph classes.
        virtual ~CGraph() = default;

        // Returns the total number of vertices in the graph.
        virtual std::size_t VertexCount() const noexcept = 0;

        // Gets the tag associated with a vertex.
        // Returns true if the vertex exists and its tag was retrieved.
        virtual bool GetVertexTag(TVertexID id, TVertexTag &tag) const noexcept = 0;

        // Gets all neighboring vertices connected to the specified vertex.
        // Returns true if the vertex exists and its neighbors were retrieved.
        virtual bool GetVertexNeighbors(TVertexID id, std::vector<TVertexID> &neighbors) const noexcept = 0;

        // Checks whether an edge exists between two vertices.
        virtual bool EdgeExists(TVertexID src, TVertexID dest) const noexcept = 0;

        // Gets the weight of an edge between two vertices.
        // Returns true if the edge exists and its weight was retrieved.
        virtual bool GetEdgeWeight(TVertexID src, TVertexID dest, double &weight) const noexcept = 0;

        // Performs an unweighted breadth-first search starting at the specified vertex.
        // Calls the callback for each visited vertex.
        virtual void BreadthFirstSearchUnweighted(TVertexID start, const TTraversalFunction &callback) const noexcept = 0;

        // Performs a depth-first search starting at the specified vertex.
        // Calls the callback for each visited vertex.
        virtual void DepthFirstSearch(TVertexID start, const TTraversalFunction &callback) const noexcept = 0;

        // Performs a weighted breadth-first search starting at the specified vertex.
        // Returns true if the search completes successfully.
        virtual bool BreadthFirstSearchWeighted(TVertexID start, const TTraversalFunction &callback) const noexcept = 0;

        // Visits every vertex in the graph, calling the callback for each one.
        // Returns true if the traversal completes successfully.
        virtual bool ForEach(const TTraversalFunction &callback) const noexcept = 0;
};

#endif