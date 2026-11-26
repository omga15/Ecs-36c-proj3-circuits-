#include <memory>
#include <vector>
#include <cstddef>
#include "CircuitNetlist.h"

struct CCircuitNetlist::SImplementation{ 
    // Type used to identify circuits in the netlist.
    using TCircuitID = std::size_t;

    // Type used to identify pins within a circuit.
    using TPinID = std::size_t;

    // Returns the total number of circuits in the netlist.
    std::size_t CircuitCount() const{
        // TODO
        return 0;
    }

    // Returns the number of pins belonging to the specified circuit.
    std::size_t PinCount(TCircuitID circuit) const{
        // TODO
        (void)circuit;
        return 0;
    }

    // Gets the name of the specified circuit.
    // Returns true if the circuit exists and its name was retrieved.
    bool CircuitName(TCircuitID circuit, TCircuitName &name) const{
        // TODO
        (void)circuit;
        (void)name;
        return false;
    }

    // Gets the circuit ID of the specified name.
    // Returns true if the circuit name exists and its ID was retrieved.
    bool CircuitNameToIndex(const TCircuitName &name, TCircuitID &circuit) const{
        // TODO
        (void)name;
        (void)circuit;
        return false;
    }

    // Gets the location of the specified pin within a circuit.
    // Returns true if the circuit and pin exist and the location was retrieved.
    bool PinLocation(TCircuitID circuit, TPinID pin, SCircuitLocation &location) const{
        // TODO
        (void)circuit;
        (void)pin;
        (void)location;
        return false;
    }

    // Adds a pin to the specified circuit at the given location.
    // Creates the circuit if it does not already exist.
    bool AddPin(TCircuitName circuit, const SCircuitLocation &location){
        // TODO
        (void)circuit;
        (void)location;
        return false;
    }
};

// Creates an empty netlist.
CCircuitNetlist::CCircuitNetlist(){
        DImplementation = std::make_unique<SImplementation>();
}

// Creates a copy of another netlist.
CCircuitNetlist::CCircuitNetlist(const CCircuitNetlist &net){
        DImplementation = std::make_unique<SImplementation>(*net.DImplementation);
}

// Transfers ownership of another netlist's data.
CCircuitNetlist::CCircuitNetlist(CCircuitNetlist &&net){
        DImplementation = std::move(net.DImplementation);
}

// Cleans up the netlist and its internal implementation.
CCircuitNetlist::~CCircuitNetlist() = default;

// Replaces this net with a copy of another netlist.
CCircuitNetlist &CCircuitNetlist::operator=(const CCircuitNetlist &net){
    if(this != &net){
        DImplementation = std::make_unique<SImplementation>(*net.DImplementation);
    }
    return *this;
}

// Transfers another netlist's data into this netlist.
CCircuitNetlist &CCircuitNetlist::operator=(CCircuitNetlist &&net) noexcept{
    if(this != &net){
        DImplementation = std::move(net.DImplementation);
    }
    return *this;
}

// Returns the total number of circuits in the netlist.
std::size_t CCircuitNetlist::CircuitCount() const{
    return DImplementation->CircuitCount();
}

// Returns the number of pins belonging to the specified circuit.
std::size_t CCircuitNetlist::PinCount(TCircuitID circuit) const{
    return DImplementation->PinCount(circuit);
}

// Gets the name of the specified circuit.
// Returns true if the circuit exists and its name was retrieved.
bool CCircuitNetlist::CircuitName(TCircuitID circuit, TCircuitName &name) const{
    return DImplementation->CircuitName(circuit, name);
}

// Gets the circuit ID of the specified name.
// Returns true if the circuit name exists and its ID was retrieved.
bool CCircuitNetlist::CircuitNameToIndex(const TCircuitName &name, TCircuitID &circuit) const{
    return DImplementation->CircuitNameToIndex(name, circuit);
}

// Gets the location of the specified pin within a circuit.
// Returns true if the circuit and pin exist and the location was retrieved.
bool CCircuitNetlist::PinLocation(TCircuitID circuit, TPinID pin, SCircuitLocation &location) const{
    return DImplementation->PinLocation(circuit, pin, location);
}

// Adds a pin to the specified circuit at the given location.
// Creates the circuit if it does not already exist.
bool CCircuitNetlist::AddPin(TCircuitName circuit, const SCircuitLocation &location){
    return DImplementation->AddPin(circuit, location);
}