#ifndef CIRCUIT_BOARD_DEFINITIONS_H
#define CIRCUIT_BOARD_DEFINITIONS_H

#include <cstdint>
#include <string>

using TCircuitCoordinate    = std::size_t;
using TLayerCoordinate      = std::size_t;
using TCircuitName          = std::string;

struct SCircuitLocation{
    TCircuitCoordinate DX;
    TCircuitCoordinate DY;

    int operator<=>(const SCircuitLocation &) const = default;
};

struct SCircuitBoardLocation{
    SCircuitLocation DLocation;
    TLayerCoordinate DLayer;
};

constexpr const TCircuitName CircuitNameGound           = "GND";
constexpr const TCircuitName CircuitNameVCC             = "VCC";
constexpr const TCircuitName CircuitNameNoConnection    = "NC";

#endif