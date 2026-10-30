#ifndef GRAPH_H
#define GRAPH_H

#include <cstddef>
#include <vector>
#include <limits>
#include <functional>

class CGraph{

    public:
        enum class EGraphTraversal{
            Continue,
            SkipNeighbors,
            Stop
        };
        using TVertexID = std::size_t;
        using TVertexTag = std::size_t;
        constexpr const static TVertexID VertexIDInvalid = std::numeric_limits<TVertexID>::max();
        constexpr const static double EdgeWeightInvalid = std::numeric_limits<double>::quiet_NaN();

        using TTraversalFunction = std::function<EGraphTraversal(TVertexID it, TVertexID parent, std::size_t depth, double distance)>;

        virtual ~CGraph() = default;

        virtual std::size_t VertexCount() const noexcept = 0;
        virtual bool GetVertexTag(TVertexID id, TVertexTag &tag) const noexcept = 0;
        virtual bool GetVertexNeighbors(TVertexID id, std::vector<TVertexID> &neighbors) const noexcept = 0;
        virtual bool EdgeExists(TVertexID src, TVertexID dest) const noexcept = 0;
        virtual bool GetEdgeWeight(TVertexID src, TVertexID dest, double &weight) const noexcept = 0; // give a value of weight so it knows how far it is from a voltage source?
        virtual void BreadthFirstSearchUnweighted(TVertexID start, const TTraversalFunction &callback) const noexcept = 0;
        virtual void DepthFirstSearch(TVertexID start, const TTraversalFunction &callback) const noexcept = 0;
        virtual bool BreadthFirstSearchWeighted(TVertexID start, const TTraversalFunction &callback) const noexcept = 0;
        virtual bool ForEach(const TTraversalFunction &callback) const noexcept = 0;
};

#endif