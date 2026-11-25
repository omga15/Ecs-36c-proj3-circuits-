#include <memory>
#include <vector>
#include <cstddef>
#include "CircuitBoard.h"

struct CCircuitBoard::SImplementation{
    CCircuitNetlist DNetlist;
    std::vector<CCircuitLayer> DLayers;
    std::size_t DPerimeter = 0;

    // Returns the board max dimensions
    SCircuitLocation Dimensions() const{
        return SCircuitLocation{};
    }

    // Returns the total number of layers in the board.
    TLayerCoordinate LayerCount() const{
        
        return TLayerCoordinate{};
    }

    // Returns allowed perimeter around the edges of the extreme edge components
    std::size_t Perimeter() const{
        // TODO
        return DPerimeter;
    }

    // Sets allowed perimeter around the edges of the extreme edge components
    std::size_t Perimeter(std::size_t perimeter){
        // TODO
        DPerimeter = perimeter;
        return DPerimeter;
    }

    // Gets the layer at the specified index.
    // Returns true if the index is valid and the layer was retrieved.
    bool Layer(TLayerCoordinate index, CCircuitLayer &layer) const{
        // TODO
        return false;
    }

    // Sets up a circuit board with all the pins specified in the netlist, and a layer for each net.
    void InitFromNetlist(const CCircuitNetlist &netlist){
        // TODO
        DNetlist = netlist;
    }

    // Sets up a circuit board with all the pins specified in the netlist, and a layer for each net.
    void InitFromNetlistAndLayers(const CCircuitNetlist &netlist, const std::vector<CCircuitLayer> &layers){
        // TODO
        DNetlist = netlist;
        DLayers = layers;
    }

    // Returns the netlist that the board was created from
    const CCircuitNetlist &Netlist() const{
        // TODO
        return DNetlist;
    }

    // Creates connected nets one on each layer
    bool CreateLayerNets(){
        // TODO
        return false;
    }

    // Minimizes the number of layers given the existing nets
    bool MinimizeLayers(){
        // TODO
        return false;
    }

    // Reroute and optimize layers
    bool RerouteAndOptimizeLayers(){
        // TODO
        return false;
    }
};



// Creates an empty circuit board.
CCircuitBoard::CCircuitBoard(){
    DImplementation = std::make_unique<SImplementation>();
}

// Creates a copy of another circuit board.
CCircuitBoard::CCircuitBoard(const CCircuitBoard &board){
    DImplementation = std::make_unique<SImplementation>(*board.DImplementation);
}

// Transfers ownership of another circuit board's data.
CCircuitBoard::CCircuitBoard(CCircuitBoard &&board){ 
    DImplementation = std::move(board.DImplementation);
}

// Cleans up the circuit board and its internal implementation.
CCircuitBoard::~CCircuitBoard() = default;

// Replaces this net with a copy of another board.
CCircuitBoard &CCircuitBoard::operator=(const CCircuitBoard &board){
    if(this != &board){
        DImplementation = std::make_unique<SImplementation>(*board.DImplementation);
    }
    return *this;
}

// Transfers another boards's data into this board.
CCircuitBoard &CCircuitBoard::operator=(CCircuitBoard &&board) noexcept{
    if(this != &board){
        DImplementation = std::move(board.DImplementation);
    }
    return *this;
}

// Returns the board max dimensions
SCircuitLocation CCircuitBoard::Dimensions() const{
    return DImplementation->Dimensions();
}

// Returns the total number of layers in the board.
TLayerCoordinate CCircuitBoard::LayerCount() const{
    return DImplementation->LayerCount();
}

// Returns allowed perimeter around the edges of the extreme edge components
std::size_t CCircuitBoard::Perimeter() const{
    return DImplementation->Perimeter();
}

// Sets allowed perimeter around the edges of the extreme edge components
std::size_t CCircuitBoard::Perimeter(std::size_t perimeter){
    return DImplementation->Perimeter(perimeter);
}

// Gets the layer at the specified index.
// Returns true if the index is valid and the layer was retrieved.
bool CCircuitBoard::Layer(TLayerCoordinate index, CCircuitLayer &layer) const{
    return DImplementation->Layer(index, layer);
}

// Creates a circuit board with all the pins specified in the netlist, and a layer for each net.
CCircuitBoard CCircuitBoard::CreateBoardFromNetlist(const CCircuitNetlist &netlist){
    CCircuitBoard Board;
    Board.DImplementation->InitFromNetlist(netlist);
    return Board;
}

// Creates a circuit board with all the pins specified in the netlist, and a layer for each net.
CCircuitBoard CCircuitBoard::CreateBoardFromNetlistAndLayers(const CCircuitNetlist &netlist, const std::vector<CCircuitLayer> &layers){
    CCircuitBoard Board;
    Board.DImplementation->InitFromNetlistAndLayers(netlist, layers);
    return Board;
}

// Returns the netlist that the board was created from
const CCircuitNetlist &CCircuitBoard::Netlist() const{
    return DImplementation->Netlist();
}

// Creates connected nets one on each layer
bool CCircuitBoard::CreateLayerNets(){
    return DImplementation->CreateLayerNets();
}

// Minimizes the number of layers given the existing nets
bool CCircuitBoard::MinimizeLayers(){
    return DImplementation->MinimizeLayers();
}

// Reroute and optimize layers
bool CCircuitBoard::RerouteAndOptimizeLayers(){
    return DImplementation->RerouteAndOptimizeLayers();
}