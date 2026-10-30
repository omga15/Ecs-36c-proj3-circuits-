#include "Graph.h"
#include "YOUR_GRAPH_HERE.h"
#include <memory>

template <typename T>
struct SGraphFactory;

using TGraphEdge = std::tuple<CGraph::TVertexID, CGraph::TVertexID, double, bool>;

template<>
struct SGraphFactory<YOUR_GRAPH_HERE > {
    static std::unique_ptr<CGraph> Create(const std::vector<CGraph::TVertexTag>& tags,
        const std::vector<TGraphEdge>& edges = {}){
        return std::make_unique<YOUR_GRAPH_HERE>(tags,edges);
    }
};
