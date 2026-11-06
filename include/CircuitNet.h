#ifndef CIRCUIT_NET_H
#define CIRCUIT_NET_H

#include <memory>
#include <vector>
#include "CircuitTrace.h"

// Represents a circuit net containing pins and connected traces.
class CCircuitNet{
    private:
        // Forward declaration of the internal implementation.
        struct SImplementation;

        // Stores the internal implementation using the PImpl design pattern.
        std::unique_ptr<SImplementation> DImplementation;

    public:
        // Creates an empty circuit net.
        CCircuitNet();

        // Creates a copy of another circuit net.
        CCircuitNet(const CCircuitNet &net);

        // Transfers ownership of another circuit net's data.
        CCircuitNet(CCircuitNet &&net) noexcept;

        // Cleans up the circuit net and its internal implementation.
        virtual ~CCircuitNet();

        // Replaces this net with a copy of another net.
        CCircuitNet &operator=(const CCircuitNet &net);

        // Transfers another net's data into this net.
        CCircuitNet &operator=(CCircuitNet &&net) noexcept;

        // Returns the name of the circuit associated with this net.
        TCircuitName Name() const;

        // Sets the name of the circuit associated with this net.
        // Returns the updated circuit name.
        TCircuitName Name(const TCircuitName &name);

        // Returns the total number of pin locations in the net.
        std::size_t PinCount() const;

        // Gets the pin location at the specified index.
        // Returns true if the index is valid and the location was retrieved.
        bool PinLocation(std::size_t index, SCircuitLocation &location) const;

        // Adds a pin location to the net.
        // Returns true if the location was successfully added.
        bool AddPinLocation(const SCircuitLocation &location);

        // Returns the total number of traces in the net.
        std::size_t TraceCount() const;

        // Gets the trace at the specified index.
        // Returns true if the index is valid and the trace was retrieved.
        bool Trace(std::size_t index, CCircuitTrace &trace) const;

        // Adds a trace to the net.
        // Returns true if the trace was successfully added.
        bool AddTrace(const CCircuitTrace &trace);

        // Checks whether the net contains broken or disconnected connections.
        bool IsBroken() const;

        // Attempts to repair broken connections in the net.
        // Returns true if the net was successfully fixed.
        bool Fix();

        // Simplifies all traces contained in the net.
        // Returns true if the traces were successfully simplified.
        bool SimplifyTraces();

        // Expands all traces contained in the net into more detailed representations.
        // Returns true if the traces were successfully elaborated.
        bool ElaborateTraces();
};

#endif