#ifndef CIRCUIT_TRACE_H
#define CIRCUIT_TRACE_H

#include <memory>
#include <vector>
#include "CircuitBoardDefinitions.h"

// Represents a connection between two locations in a circuit trace.
struct SCircuitTraceEdge{
    // Starting location of the edge.
    SCircuitLocation DSource;

    // Ending location of the edge.
    SCircuitLocation DDestination;

    // Compares two edges based on their source and destination locations.
    auto operator<=>(const SCircuitTraceEdge &) const = default;
};

// Represents a sequence of connected locations within a circuit.
class CCircuitTrace{
    private:
        // Forward declaration of the internal implementation.
        struct SImplementation;

        // Stores the internal implementation using the PImpl design pattern.
        std::unique_ptr<SImplementation> DImplementation;

    public:
        // Creates an empty circuit trace.
        CCircuitTrace();

        // Creates a copy of another circuit trace.
        CCircuitTrace(const CCircuitTrace &trace);

        // Transfers ownership of another circuit trace's data.
        CCircuitTrace(CCircuitTrace &&trace) noexcept;

        // Cleans up the circuit trace and its internal implementation.
        virtual ~CCircuitTrace();

        // Replaces this trace with a copy of another trace.
        CCircuitTrace &operator=(const CCircuitTrace &trace);

        // Transfers another trace's data into this trace.
        CCircuitTrace &operator=(CCircuitTrace &&trace) noexcept;

        // Returns the name of the circuit associated with this trace.
        TCircuitName Name() const;

        // Sets the name of the circuit associated with this trace.
        // Returns the updated circuit name.
        TCircuitName Name(const TCircuitName &name);

        // Returns the total number of locations in the trace.
        std::size_t LocationCount() const;

        // Gets the location at the specified index.
        // Returns true if the index is valid and the location was retrieved.
        bool Location(std::size_t index, SCircuitLocation &location) const;

        // Returns the total number of edges in the trace.
        std::size_t EdgeCount() const;

        // Gets the edge at the specified index.
        // Returns true if the index is valid and the edge was retrieved.
        bool Edge(std::size_t index, SCircuitTraceEdge &edge) const;

        // Gets the edge connecting the first and last locations of the trace.
        // Returns true if the source and destination locations were retrieved.
        bool SourceDestinationAsEdge(SCircuitTraceEdge &edge) const;

        // Adds an edge to the trace.
        // Returns true if the edge was successfully added.
        bool AddEdge(const SCircuitTraceEdge &edge);

        // Appends another circuit trace to this trace.
        // Returns true if the traces were successfully concatenated.
        bool ConcatenateTrace(const CCircuitTrace &trace);

        // Checks whether the trace contains broken or disconnected connections.
        bool IsBroken() const;

        // Attempts to repair broken connections in the trace.
        // Returns true if the trace was successfully fixed.
        bool Fix();

        // Returns a simplified version of the trace by consolidating into longer edges.
        CCircuitTrace Simplify() const;

        // Returns a more detailed version of the trace by creating an edge for each grid step.
        CCircuitTrace Elaborate() const;
};

#endif