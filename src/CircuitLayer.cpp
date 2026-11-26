#include <memory>
#include <vector>
#include <utility>
#include <cstddef>
#include "CircuitLayer.h"

struct CCircuitLayer::SImplementation{
    
    std::vector<CCircuitNet> DNets;
    std::vector<SCircuitLocation> DPins;

    // Returns the total number of pin locations in the layer.
    std::size_t PinCount() const{
        size_t numberPins = 0;
        for(const CCircuitNet net : DNets){
            numberPins += net.PinCount();
        }
        return numberPins;
    }

    // Gets the pin location at the specified index.
    // Returns true if the index is valid and the location was retrieved.
    bool PinLocation(std::size_t index, SCircuitLocation &location) const{
            // TODO
        (size_t) index; 
        for(const CCircuitNet &net : DNets){
            std::size_t Count = net.PinCount();
                if(index < Count){
                    return net.PinLocation(index, location);
                }
            index -= Count;
        }
    return false;  // index was past the last pin
    }

    // Gets the pin locations of the specified circuit.
    // Returns true if the index is valid and the location was retrieved.
    bool PinLocations(const TCircuitName &name, std::vector<SCircuitLocation> &locations) const{
        for(const CCircuitNet net : DNets){
            if(name == net.Name()){
                locations.clear();
                size_t Count = net.PinCount();
                SCircuitLocation PinLocation;
                for(size_t i = 0; i< Count; i++){
                    if(net.PinLocation(i,PinLocation)){
                    locations.push_back(PinLocation);
                    }
                }
                return true;
            }
        }
        return false;
    }

    // Adds a pin of the specified net name to location.
    // Returns true if the pin was successfully added.
    bool AddPin(const TCircuitName &name, const SCircuitLocation &location){
        for(const CCircuitNet &net : DNets){      // checks for duplicate pins
            size_t Count = net.PinCount();
            SCircuitLocation PinLocation;
            for(size_t i = 0; i< Count; i++){
                if(net.PinLocation(i,PinLocation) && PinLocation == location){
                    return false;
                }
            }
        }
        for(CCircuitNet &net : DNets){ 
            if(net.Name() == name){
                return net.AddPinLocation(location);
            }
        }
        CCircuitNet DNet;
        DNet.Name(name);
        DNet.AddPinLocation(location);
        DNets.push_back(DNet);
        return true;
    }

    // Returns the total number of nets in the layer.
    std::size_t NetCount() const{
        return DNets.size();
    }

    // Gets the net at the specified index.
    // Returns true if the index is valid and the net was retrieved.
    bool CircuitNet(std::size_t index, CCircuitNet &net) const{
        if(index >= NetCount()){
            return false;
        }
        net = DNets[index];
        return true;
    }

    // Gets the net with the specified name.
    // Returns true if the name is valid and the net was retrieved.
    bool CircuitNet(const TCircuitName &name, CCircuitNet &net) const{
        for(const CCircuitNet &DNet : DNets){      // checks for name then retrieves it
            if(DNet.Name() == name){
                net = DNet;
                return true;
            }
        }

        return false;
    }

    // Adds the net, to the layer. If allowmerge is true existing net with same
    // name will be merged with provided net. If allowmerge is false, adding fails
    // if net name already exists.
    // Returns true if the net was successfully added.
    bool AddCircuitNet(const CCircuitNet &net, bool allowmerge){
        for(CCircuitNet &DNet : DNets){      // checks for name then retrieves it
            if(DNet.Name() == net.Name()){
                if(!allowmerge){
                    return false;
                }
                size_t Count = net.PinCount();
                SCircuitLocation location; 
                for (size_t i = 0; i < Count; i++)
                {
                    if(net.PinLocation(i,location)){
                        DNet.AddPinLocation(location);
                    }
                }
                CCircuitTrace trace;
                Count = net.TraceCount();
                for (size_t i = 0; i < Count; i++)
                {
                    if(net.Trace(i,trace)){
                        DNet.AddTrace(trace);
                    }
                }
                return true;
                }
            }
        DNets.push_back(net);
        return true;
    }

    // Gets all pairs of net names that are shorted together.
    // Returns true if at least one pair of nets is shorted together.
    bool GetShortedNets(std::vector<std::pair<TCircuitName,TCircuitName>> &nets) const{
        // TODO
        (void)nets;
        return false;
    }

    // Gets all net names that are broken.
    // Returns true if at least one net is broken.
    bool GetBrokenNets(std::vector<TCircuitName> &nets) const{
        // TODO
        (void)nets;
        return false;
    }
};

// Creates an empty circuit layer.
CCircuitLayer::CCircuitLayer(){
    DImplementation = std::make_unique<SImplementation>();
}

// Creates a copy of another circuit layer.
CCircuitLayer::CCircuitLayer(const CCircuitLayer &layer){
    DImplementation = std::make_unique<SImplementation>(*layer.DImplementation);
}

// Transfers ownership of another circuit layer's data.
CCircuitLayer::CCircuitLayer(CCircuitLayer &&layer) noexcept{
    DImplementation = std::move(layer.DImplementation);
}

// Cleans up the circuit layer and its internal implementation.
CCircuitLayer::~CCircuitLayer() = default;

// Replaces this layer with a copy of another layer.
CCircuitLayer &CCircuitLayer::operator=(const CCircuitLayer &layer){
    if(this != &layer){
        DImplementation = std::make_unique<SImplementation>(*layer.DImplementation);
    }
    return *this;
}

// Transfers another layer's data into this layer.
CCircuitLayer &CCircuitLayer::operator=(CCircuitLayer &&layer) noexcept{
    if(this != &layer){
        DImplementation = std::move(layer.DImplementation);
    }
    return *this;
}

// Returns the total number of pin locations in the layer.
std::size_t CCircuitLayer::PinCount() const{
    return DImplementation->PinCount();
}

// Gets the pin location at the specified index.
// Returns true if the index is valid and the location was retrieved.
bool CCircuitLayer::PinLocation(std::size_t index, SCircuitLocation &location) const{
    return DImplementation->PinLocation(index, location);
}

// Gets the pin locations of the specified circuit.
// Returns true if the index is valid and the location was retrieved.
bool CCircuitLayer::PinLocations(const TCircuitName &name, std::vector<SCircuitLocation> &locations) const{
    return DImplementation->PinLocations(name, locations);
}

// Adds a pin of the specified net name to location.
// Returns true if the pin was successfully added.
bool CCircuitLayer::AddPin(const TCircuitName &name, const SCircuitLocation &location){
    return DImplementation->AddPin(name, location);
}

// Returns the total number of nets in the layer.
std::size_t CCircuitLayer::NetCount() const{
    return DImplementation->NetCount();
}

// Gets the net at the specified index.
// Returns true if the index is valid and the net was retrieved.
bool CCircuitLayer::CircuitNet(std::size_t index, CCircuitNet &net) const{
    return DImplementation->CircuitNet(index, net);
}

// Gets the net with the specified name.
// Returns true if the name is valid and the net was retrieved.
bool CCircuitLayer::CircuitNet(const TCircuitName &name, CCircuitNet &net) const{
    return DImplementation->CircuitNet(name, net);
}

// Adds the net, to the layer. If allowmerge is true existing net with same
// name will be merged with provided net. If allowmerge is false, adding fails
// if net name already exists.
// Returns true if the net was successfully added.
bool CCircuitLayer::AddCircuitNet(const CCircuitNet &net, bool allowmerge){
    return DImplementation->AddCircuitNet(net, allowmerge);
}

// Gets all pairs of net names that are shorted together.
// Returns true if at least one pair of nets is shorted together.
bool CCircuitLayer::GetShortedNets(std::vector<std::pair<TCircuitName,TCircuitName>> &nets) const{
    return DImplementation->GetShortedNets(nets);
}

// Gets all net names that are broken.
// Returns true if at least one net is broken.
bool CCircuitLayer::GetBrokenNets(std::vector<TCircuitName> &nets) const{
    return DImplementation->GetBrokenNets(nets);
}