#include "Deque.h"
#include "MaxSizeDeque.h"
#include "VariableSizeDeque.h"
#include <memory>


template <typename T>
struct SDequeFactory;

template<>
struct SDequeFactory<CMaxSizeDeque> {
    static std::unique_ptr<CDeque> Create(){
        return std::make_unique<CMaxSizeDeque>(128);
    }
};

template<>
struct SDequeFactory<CVariableSizeDeque> {
    static std::unique_ptr<CDeque> Create(){
        return std::make_unique<CVariableSizeDeque>();
    }
};