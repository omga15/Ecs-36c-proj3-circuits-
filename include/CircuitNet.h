#ifndef CIRCUIT_NET_H
#define CIRCUIT_NET_H

#include <memory>
#include <vector>
#include "CircuitTrace.h"

class CCircuitNet{
    private:
        struct SImplementation;
        std::unique_ptr<SImplementation> DImplementation;
        
    public:
        CCircuitNet();
        ~CCircuitNet();

        TCircuitName Name() const;
        TCircuitName Name(const TCircuitName &name);

        std::size_t PinCount() const;
        SCircuitLocation PinLocation(std::size_t index) const;
        bool AddPinLocation(const SCircuitLocation &location);

        std::size_t TraceCount() const;
        CCircuitTrace Trace(std::size_t index) const;
        
};


#endif