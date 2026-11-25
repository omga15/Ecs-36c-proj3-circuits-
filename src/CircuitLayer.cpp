#include <memory>
#include <vector>
#include <utility>
#include "CircuitNet.h"

struct CCircuitLayer::SImplementation{
    // Creates an empty circuit layer.
    CCircuitLayer();

    // Creates a copy of another circuit net.
    CCircuitLayer(const CCircuitLayer &net);

    // Transfers ownership of another circuit net's data.
    CCircuitLayer(CCircuitLayer &&net) noexcept;

    // Cleans up the circuit layer and its internal implementation.
    ~CCircuitLayer();

    // Replaces this net with a copy of another net.
    CCircuitLayer &operator=(const CCircuitLayer &net);

    // Transfers another net's data into this net.
    CCircuitLayer &operator=(CCircuitLayer &&net) noexcept{
        return ;
    }

    // Returns the total number of pin locations in the layer.
    std::size_t PinCount() const{
        return 0;
    }

    // Gets the pin location at the specified index.
    // Returns true if the index is valid and the location was retrieved.
    bool PinLocation(std::size_t index, SCircuitLocation &location) const;

    // Gets the pin locations of the specified circuit.
    // Returns true if the index is valid and the location was retrieved.
    bool PinLocations(const TCircuitName &name, std::vector<SCircuitLocation> &locations) const;

    // Adds a pin of the specified net name to location.
    // Returns true if the pin was successfully added.
    bool AddPin(const TCircuitName &name, const SCircuitLocation &location);

    // Returns the total number of nets in the layer.
    std::size_t NetCount() const;

    // Gets the net at the specified index.
    // Returns true if the index is valid and the net was retrieved.
    bool CircuitNet(std::size_t index, CCircuitNet &net) const;

    // Gets the net with the specified name.
    // Returns true if the name is valid and the net was retrieved.
    bool CircuitNet(const TCircuitName &name, CCircuitNet &net) const;

    // Adds the net, to the layer. If allowmerge is true existing net with same 
    // name will be merged with provided net. If allowmerge is false, adding fails 
    // if net name already exists.
    // Returns true if the net was successfully added.
    bool AddCircuitNet(const CCircuitNet &net, bool allowmerge = false);

    // Gets all pairs of net names that are shorted together. 
    // Returns true if at least one pair of nets is shorted together.
    bool GetShortedNets(std::vector<std::pair<TCircuitName,TCircuitName>> &nets) const;

    // Gets all net names that are broken. 
    // Returns true if at least one net is broken.
    bool GetBrokenNets(std::vector<TCircuitName> &nets) const;

};

// Creates an empty circuit layer.
    CCircuitLayer();

    // Creates a copy of another circuit net.
    CCircuitLayer(const CCircuitLayer &net);

    // Transfers ownership of another circuit net's data.
    CCircuitLayer(CCircuitLayer &&net) noexcept;

    // Cleans up the circuit layer and its internal implementation.
    ~CCircuitLayer();

    // Replaces this net with a copy of another net.
    CCircuitLayer &operator=(const CCircuitLayer &net);

    // Transfers another net's data into this net.
    CCircuitLayer &operator=(CCircuitLayer &&net) noexcept;

    // Returns the total number of pin locations in the layer.
    std::size_t PinCount() const;

    // Gets the pin location at the specified index.
    // Returns true if the index is valid and the location was retrieved.
    bool PinLocation(std::size_t index, SCircuitLocation &location) const;

    // Gets the pin locations of the specified circuit.
    // Returns true if the index is valid and the location was retrieved.
    bool PinLocations(const TCircuitName &name, std::vector<SCircuitLocation> &locations) const;

    // Adds a pin of the specified net name to location.
    // Returns true if the pin was successfully added.
    bool AddPin(const TCircuitName &name, const SCircuitLocation &location);

    // Returns the total number of nets in the layer.
    std::size_t NetCount() const;

    // Gets the net at the specified index.
    // Returns true if the index is valid and the net was retrieved.
    bool CircuitNet(std::size_t index, CCircuitNet &net) const;

    // Gets the net with the specified name.
    // Returns true if the name is valid and the net was retrieved.
    bool CircuitNet(const TCircuitName &name, CCircuitNet &net) const;

    // Adds the net, to the layer. If allowmerge is true existing net with same 
    // name will be merged with provided net. If allowmerge is false, adding fails 
    // if net name already exists.
    // Returns true if the net was successfully added.
    bool AddCircuitNet(const CCircuitNet &net, bool allowmerge = false);

    // Gets all pairs of net names that are shorted together. 
    // Returns true if at least one pair of nets is shorted together.
    bool CCircuitLayer::GetShortedNets(std::vector<std::pair<TCircuitName,TCircuitName>> &nets) const;

    // Gets all net names that are broken. 
    // Returns true if at least one net is broken.
    bool CCircuitLayer::GetBrokenNets(std::vector<TCircuitName> &nets) const{

    }

