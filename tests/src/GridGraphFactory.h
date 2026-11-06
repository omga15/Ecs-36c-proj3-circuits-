#include "GridGraph.h"
#include "YOUR_GRAPH_HERE.h"
#include <memory>


template <typename T>
struct SGridGraphFactory;

template<>
struct SGridGraphFactory<YOUR_GRAPH_HERE > {
    static std::unique_ptr<CGraph> Create(
        TGridCoordinate width,
        TGridCoordinate height,
        const SGridConnectionPattern& pattern = {}){
        return std::make_unique<YOUR_GRAPH_HERE>(width,height,pattern);
    }
};
