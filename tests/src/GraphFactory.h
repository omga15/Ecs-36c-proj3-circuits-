#include "Graph.h"
#include "AdjacencyListGraph.h"
#include <memory>

template <typename T>
struct SGraphFactory;

using TGraphEdge = std::tuple<CGraph::TVertexID, CGraph::TVertexID, double, bool>;

template<>
struct SGraphFactory<CAdjacencyListGraph> {
    static std::unique_ptr<CGraph> Create(const std::vector<CGraph::TVertexTag>& tags,
        const std::vector<TGraphEdge>& edges = {}){
        return std::make_unique<CAdjacencyListGraph>(tags,edges);
    }
};
