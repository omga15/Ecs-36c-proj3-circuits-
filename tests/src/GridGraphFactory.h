#include "GridGraph.h"
#include "VGridGraph.h"
#include <memory>


template <typename T>
struct SGridGraphFactory;

template<>
struct SGridGraphFactory<VGridGraph > {
    static std::unique_ptr<CGraph> Create(
        TGridCoordinate width,
        TGridCoordinate height,
        const SGridConnectionPattern& pattern = {}){
        return std::make_unique<VGridGraph>(width,height,pattern);
    }
};
