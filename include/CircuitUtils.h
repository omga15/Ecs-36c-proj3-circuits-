#ifndef CIRCUIT_UTILS_H
#define CIRCUIT_UTILS_H

#include "CircuitBoard.h"
#include <unordered_map>

class CCircuitUtils{
    public:
        static bool LoadNetlist(const std::string &filename, CCircuitNetlist &netlist);
        static bool LoadBoard(const std::string &filename, CCircuitBoard &board);

        static bool SaveNetlist(const std::string &filename, CCircuitNetlist &netlist);
        static bool SaveBoard(const std::string &filename, CCircuitBoard &board);
};

std::string StringStrip(const std::string &str) noexcept;
std::string StringReplace(const std::string &str, const std::string &old, const std::string &rep) noexcept;
std::string StringReplace(const std::string &str, const std::unordered_map<std::string,std::string> &context);
std::string StringLower(const std::string &str) noexcept;

#endif