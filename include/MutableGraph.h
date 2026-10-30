#ifndef MUTABLE_GRAPH_H
#define MUTABLE_GRAPH_H

#include "Graph.h"

class CVertexMutableGraph : public virtual CGraph{
    public:
        virtual ~CVertexMutableGraph() = default;

        virtual TVertexID AddVertex(TVertexTag id) noexcept = 0;
        virtual bool RemoveVertex(TVertexID id) noexcept = 0;
};

class CVertexTagMutableGraph : public virtual CGraph{
    public:
        virtual ~CVertexTagMutableGraph() = default;

        virtual bool SetVertexTag(TVertexID id, TVertexTag tag) noexcept = 0;
};

class CEdgeMutableGraph : public virtual CGraph{
    public:
        virtual ~CEdgeMutableGraph() = default;

        virtual bool AddEdge(TVertexID src, TVertexID dest, double weight = EdgeWeightInvalid, bool bidir = true) noexcept = 0;
        virtual bool RemoveEdge(TVertexID src, TVertexID dest) noexcept = 0;
        virtual bool SetEdgeWeight(TVertexID src, TVertexID dest, double weight) noexcept = 0;
};

#endif