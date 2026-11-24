#ifndef CIRCUIT_BOARD_H
#define CIRCUIT_BOARD_H

#include "CircuitLayer.h"
#include "CircuitNetlist.h"

class CCircuitBoard{
    private:
        struct SImplementation;
        std::unique_ptr<SImplementation> DImplemenetation;

    public:
        // Creates an empty circuit board.
        CCircuitBoard();

        // Creates a copy of another circuit board.
        CCircuitBoard(const CCircuitBoard &board);

        // Transfers ownership of another circuit board's data.
        CCircuitBoard(CCircuitBoard &&board);

        // Cleans up the circuit board and its internal implementation.
        ~CCircuitBoard();

        // Replaces this net with a copy of another board.
        CCircuitBoard &operator=(const CCircuitBoard &board);

        // Transfers another boards's data into this board.
        CCircuitBoard &operator=(CCircuitBoard &&board) noexcept;

        // Returns the board max dimensions
        SCircuitLocation Dimensions() const;

        // Returns the total number of layers in the board.
        TLayerCoordinate LayerCount() const;

        // Returns allowed perimeter around the edges of the extreme edge components
        std::size_t Perimeter() const;

        // Sets allowed perimeter around the edges of the extreme edge components
        std::size_t Perimeter(std::size_t perimeter);

        // Gets the layer at the specified index.
        // Returns true if the index is valid and the layer was retrieved.
        bool Layer(TLayerCoordinate index, CCircuitLayer &layer) const;

        // Creates a circuit board with all the pins specified in the netlist, and a layer for each net.
        static CCircuitBoard CreateBoardFromNetlist(const CCircuitNetlist &netlist);

        // Creates a circuit board with all the pins specified in the netlist, and a layer for each net.
        static CCircuitBoard CreateBoardFromNetlistAndLayers(const CCircuitNetlist &netlist, const std::vector<CCircuitLayer> &layers);

        // Returns the netlist that the board was created from
        const CCircuitNetlist &Netlist() const;

        // Creates connected nets one on each layer
        bool CreateLayerNets();

        // Minimizes the number of layers given the existing nets
        bool MinimizeLayers();

        // Reroute and optimize layers
        bool RerouteAndOptimizeLayers();
};


#endif