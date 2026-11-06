#ifndef MUTABLE_GRAPH_H
#define MUTABLE_GRAPH_H

#include "Graph.h"

// Interface for graphs that allow vertices to be added and removed.
class CVertexMutableGraph : public virtual CGraph{
    public:
        // Virtual destructor for proper cleanup of derived graph classes.
        virtual ~CVertexMutableGraph() = default;

        // Adds a new vertex with the specified tag.
        // Returns the ID assigned to the new vertex.
        virtual TVertexID AddVertex(TVertexTag id) noexcept = 0;

        // Removes the specified vertex from the graph.
        // Returns true if the vertex was successfully removed.
        virtual bool RemoveVertex(TVertexID id) noexcept = 0;
};

// Interface for graphs that allow vertex tags to be modified.
class CVertexTagMutableGraph : public virtual CGraph{
    public:
        // Virtual destructor for proper cleanup of derived graph classes.
        virtual ~CVertexTagMutableGraph() = default;

        // Changes the tag associated with the specified vertex.
        // Returns true if the vertex exists and its tag was updated.
        virtual bool SetVertexTag(TVertexID id, TVertexTag tag) noexcept = 0;
};

// Interface for graphs that allow edges to be added, removed, and modified.
class CEdgeMutableGraph : public virtual CGraph{
    public:
        // Virtual destructor for proper cleanup of derived graph classes.
        virtual ~CEdgeMutableGraph() = default;

        // Adds an edge between two vertices with the specified weight.
        // If bidir is true, the edge is added in both directions.
        // If no weight is provided, EdgeWeightInvalid is used.
        // Returns true if the edge was successfully added.
        virtual bool AddEdge(TVertexID src, TVertexID dest, double weight = EdgeWeightInvalid, bool bidir = true) noexcept = 0;

        // Removes the edge between two vertices.
        // Returns true if the edge was successfully removed.
        virtual bool RemoveEdge(TVertexID src, TVertexID dest) noexcept = 0;

        // Changes the weight of an existing edge.
        // Returns true if the edge exists and its weight was updated.
        virtual bool SetEdgeWeight(TVertexID src, TVertexID dest, double weight) noexcept = 0;
};

// Interface for graphs that allow all types of mutation.
class CFullMutableGraph : public virtual CVertexMutableGraph, public virtual CVertexTagMutableGraph, public virtual CEdgeMutableGraph{
    public:
        // Virtual destructor for proper cleanup of derived graph classes.
        virtual ~CFullMutableGraph() = default;
};

#endif