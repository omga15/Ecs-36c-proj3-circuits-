#ifndef CIRCUIT_HASHES_H
#define CIRCUIT_HASHES_H

#include "HashMap.h"

template<>
struct CDefaultHash<std::string> {
    std::size_t operator()(const std::string &key) const {
        uint32_t Hashed = 2166136261;
        for(auto Ch : key){
            Hashed ^= Ch;
            Hashed *= 16777619;
        }
        return static_cast<std::size_t>(Hashed);
    }
};

template<>
struct CDefaultHash<SCircuitLocation> {
    std::size_t operator()(const SCircuitLocation &key) const {
        return static_cast<std::size_t>(key.DX) ^ (static_cast<std::size_t>(key.DY)<<15);
    }
};

template<>
struct CDefaultHash<std::pair<std::size_t,std::size_t>> {
    std::size_t operator()(const std::pair<std::size_t,std::size_t> &key) const {
        return static_cast<std::size_t>(std::get<0>(key)) ^ (static_cast<std::size_t>(std::get<1>(key))<<2);
    }
};

#endif