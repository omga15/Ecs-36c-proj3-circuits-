#ifndef CIRCUIT_NETLIST_H
#define CIRCUIT_NETLIST_H

#include <memory>
#include <vector>
#include "CircuitBoardDefinitions.h"

// Manages a collection of circuits and their associated pins.
class CCircuitNetlist{
    private:
        // Forward declaration of the internal implementation.
        struct SImplementation;

        // Stores the internal implementation using the PImpl design pattern.
        std::unique_ptr<SImplementation> DImplementation;

    public:
        // Type used to identify circuits in the netlist.
        using TCircuitID = std::size_t;

        // Type used to identify pins within a circuit.
        using TPinID = std::size_t;

        // Creates an empty circuit netlist.
        CCircuitNetlist();

        // Creates a copy of another circuit netlist.
        CCircuitNetlist(const CCircuitNetlist &net);

        // Transfers ownership of another circuit netlist's data.
        CCircuitNetlist(CCircuitNetlist &&net) noexcept;

        // Cleans up the circuit netlist and its internal implementation.
        ~CCircuitNetlist();

        // Replaces this net with a copy of another netlist.
        CCircuitNetlist &operator=(const CCircuitNetlist &net);

        // Transfers another netlist's data into this netlist.
        CCircuitNetlist &operator=(CCircuitNetlist &&net) noexcept;

        // Returns the total number of circuits in the netlist.
        std::size_t CircuitCount() const;

        // Returns the number of pins belonging to the specified circuit.
        std::size_t PinCount(TCircuitID circuit) const;

        // Gets the name of the specified circuit.
        // Returns true if the circuit exists and its name was retrieved.
        bool CircuitName(TCircuitID circuit, TCircuitName &name) const;

        // Gets the circuit ID of the specified name.
        // Returns true if the circuit name exists and its ID was retrieved.
        bool CircuitNameToIndex(const TCircuitName &name, TCircuitID &circuit) const;

        // Gets the location of the specified pin within a circuit.
        // Returns true if the circuit and pin exist and the location was retrieved.
        bool PinLocation(TCircuitID circuit, TPinID pin, SCircuitLocation &location) const;

        // Adds a pin to the specified circuit at the given location.
        // Creates the circuit if it does not already exist.
        bool AddPin(TCircuitName circuit, const SCircuitLocation &location);
};

#endif