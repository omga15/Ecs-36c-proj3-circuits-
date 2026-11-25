
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "CircuitBoard.h"

#include <string>
#include <utility>
#include <vector>

class CCircuitBoardTest : public ::testing::Test {
protected:
    CCircuitBoard Board;

    SCircuitLocation A{0, 0};
    SCircuitLocation B{10, 0};
    SCircuitLocation C{0, 10};
    SCircuitLocation D{10, 10};
    SCircuitLocation E{0, 20};
    SCircuitLocation F{10, 20};

    CCircuitNetlist MakeNetlist(){
        CCircuitNetlist Netlist;

        EXPECT_TRUE(Netlist.AddPin("NetA", A));
        EXPECT_TRUE(Netlist.AddPin("NetA", B));

        EXPECT_TRUE(Netlist.AddPin("NetB", C));
        EXPECT_TRUE(Netlist.AddPin("NetB", D));

        EXPECT_TRUE(Netlist.AddPin("NetC", E));
        EXPECT_TRUE(Netlist.AddPin("NetC", F));

        return Netlist;
    }

    bool SameLocation(
        const SCircuitLocation &Actual,
        const SCircuitLocation &Expected){

        return Actual.DX == Expected.DX &&
               Actual.DY == Expected.DY;
    }

    void ExpectLocation(
        const SCircuitLocation &Actual,
        const SCircuitLocation &Expected){

        EXPECT_EQ(Actual.DX, Expected.DX);
        EXPECT_EQ(Actual.DY, Expected.DY);
    }

    bool HasLocation(
        const std::vector<SCircuitLocation> &Locations,
        const SCircuitLocation &Expected){

        for(const auto &Location : Locations){
            if(SameLocation(Location, Expected)){
                return true;
            }
        }

        return false;
    }

    bool HasPin(
        const CCircuitLayer &TestLayer,
        const SCircuitLocation &Expected){

        for(std::size_t Index = 0;
            Index < TestLayer.PinCount();
            Index++){

            SCircuitLocation Location;

            if(TestLayer.PinLocation(Index, Location) &&
               SameLocation(Location, Expected)){
                return true;
            }
        }

        return false;
    }

    bool HasNet(
        const CCircuitLayer &TestLayer,
        const TCircuitName &Name){

        CCircuitNet Net;
        return TestLayer.CircuitNet(Name, Net);
    }

    bool HasNoShortedNets(const CCircuitLayer &TestLayer){

        std::vector<std::pair<TCircuitName, TCircuitName>> Nets;

        EXPECT_FALSE(TestLayer.GetShortedNets(Nets));
        EXPECT_TRUE(Nets.empty());

        return Nets.empty();
    }

    bool HasNoBrokenNets(const CCircuitLayer &TestLayer){

        std::vector<TCircuitName> Nets;

        EXPECT_FALSE(TestLayer.GetBrokenNets(Nets));
        EXPECT_TRUE(Nets.empty());

        return Nets.empty();
    }

    void ExpectAllNetsPresent(
        const CCircuitLayer &TestLayer,
        const std::vector<TCircuitName> &Names){

        EXPECT_EQ(TestLayer.NetCount(), Names.size());

        for(const auto &Name : Names){
            EXPECT_TRUE(HasNet(TestLayer, Name));
        }
    }

    void ExpectAllNetsConnected(
        const CCircuitLayer &TestLayer,
        const std::vector<TCircuitName> &Names){

        for(const auto &Name : Names){
            CCircuitNet Net;

            if(TestLayer.CircuitNet(Name, Net)){
                EXPECT_EQ(Net.PinCount(), 2);
                EXPECT_FALSE(Net.IsBroken());
            }
        }
    }

    CCircuitBoard MakePopulatedBoard(){
        auto Netlist = MakeNetlist();
        auto TestBoard =
            CCircuitBoard::CreateBoardFromNetlist(Netlist);
        EXPECT_TRUE(TestBoard.CreateLayerNets());
        return TestBoard;
    }
};


// Construction

TEST_F(CCircuitBoardTest, DefaultConstruction){
    EXPECT_EQ(Board.LayerCount(), 0);
}

TEST_F(CCircuitBoardTest, EmptyBoardHasNoLayers){
    EXPECT_EQ(Board.LayerCount(), 0);
}

TEST_F(CCircuitBoardTest, EmptyBoardLayerRetrieval){
    CCircuitLayer Layer;

    EXPECT_FALSE(Board.Layer(0, Layer));
}


// Copy Construction

TEST_F(CCircuitBoardTest, CopyConstructionOfEmptyBoard){
    CCircuitBoard Copy(Board);

    EXPECT_EQ(Copy.LayerCount(), 0);
}

TEST_F(CCircuitBoardTest, CopyConstructionOfPopulatedBoard){
    auto Original = MakePopulatedBoard();
    CCircuitBoard Copy(Original);
    EXPECT_EQ(Copy.LayerCount(), Original.LayerCount());
    for(TLayerCoordinate Index = 0;
        Index < Original.LayerCount();
        Index++){

        CCircuitLayer OriginalLayer;
        CCircuitLayer CopiedLayer;
    
        ASSERT_TRUE(Original.Layer(Index, OriginalLayer));
        ASSERT_TRUE(Copy.Layer(Index, CopiedLayer));
        EXPECT_EQ(
            CopiedLayer.PinCount(),
            OriginalLayer.PinCount());
        EXPECT_EQ(
            CopiedLayer.NetCount(),
            OriginalLayer.NetCount());
    }
}


// Move Construction

TEST_F(CCircuitBoardTest, MoveConstructionOfEmptyBoard){
    CCircuitBoard Original;

    CCircuitBoard Moved(std::move(Original));

    EXPECT_EQ(Moved.LayerCount(), 0);
}

TEST_F(CCircuitBoardTest, MoveConstructionOfPopulatedBoard){
    auto Original = MakePopulatedBoard();

    auto OriginalLayerCount = Original.LayerCount();

    CCircuitBoard Moved(std::move(Original));

    EXPECT_EQ(Moved.LayerCount(), OriginalLayerCount);
}


// Copy Assignment

TEST_F(CCircuitBoardTest, CopyAssignmentOfEmptyBoard){
    CCircuitBoard Original;

    Board = Original;

    EXPECT_EQ(Board.LayerCount(), 0);
}

TEST_F(CCircuitBoardTest, CopyAssignmentOfPopulatedBoard){
    auto Original = MakePopulatedBoard();

    Board = Original;

    EXPECT_EQ(Board.LayerCount(), Original.LayerCount());

    for(TLayerCoordinate Index = 0;
        Index < Board.LayerCount();
        Index++){

        CCircuitLayer Layer;

        ASSERT_TRUE(Board.Layer(Index, Layer));

        EXPECT_EQ(Layer.NetCount(), 1);
    }
}

TEST_F(CCircuitBoardTest, CopyAssignmentReplacesExistingLayers){
    auto Original = MakePopulatedBoard();

    Board = Original;

    CCircuitBoard Empty;

    Board = Empty;

    EXPECT_EQ(Board.LayerCount(), 0);
}


// Move Assignment

TEST_F(CCircuitBoardTest, MoveAssignmentOfEmptyBoard){
    CCircuitBoard Original;

    Board = std::move(Original);

    EXPECT_EQ(Board.LayerCount(), 0);
}

TEST_F(CCircuitBoardTest, MoveAssignmentOfPopulatedBoard){
    auto Original = MakePopulatedBoard();

    auto OriginalLayerCount = Original.LayerCount();

    Board = std::move(Original);

    EXPECT_EQ(Board.LayerCount(), OriginalLayerCount);
}

TEST_F(CCircuitBoardTest, MoveAssignmentReplacesExistingLayers){
    auto Original = MakePopulatedBoard();

    Board = std::move(Original);

    CCircuitBoard Empty;

    Board = std::move(Empty);

    EXPECT_EQ(Board.LayerCount(), 0);
}


// Layer Count

TEST_F(CCircuitBoardTest, LayerCountInitiallyZero){
    EXPECT_EQ(Board.LayerCount(), 0);
}

TEST_F(CCircuitBoardTest, LayerCountAfterCreatingBoardFromNetlist){
    auto Netlist = MakeNetlist();

    Board = CCircuitBoard::CreateBoardFromNetlist(Netlist);

    EXPECT_EQ(Board.LayerCount(), Netlist.CircuitCount());
}

TEST_F(CCircuitBoardTest, LayerCountAfterCreatingLayerNets){
    auto Netlist = MakeNetlist();

    Board = CCircuitBoard::CreateBoardFromNetlist(Netlist);

    ASSERT_TRUE(Board.CreateLayerNets());

    EXPECT_EQ(Board.LayerCount(), Netlist.CircuitCount());
}

// Perimeter
TEST_F(CCircuitBoardTest, PerimeterCheck){
    EXPECT_EQ(Board.Perimeter(),10);

    EXPECT_EQ(Board.Perimeter(15),15);

    EXPECT_EQ(Board.Perimeter(),15);
}


// Layer Retrieval

TEST_F(CCircuitBoardTest, LayerOutOfBounds){
    CCircuitLayer Layer;

    EXPECT_FALSE(Board.Layer(0, Layer));
}

TEST_F(CCircuitBoardTest, LayerAtInvalidIndex){
    auto Netlist = MakeNetlist();

    Board = CCircuitBoard::CreateBoardFromNetlist(Netlist);

    CCircuitLayer Layer;

    EXPECT_FALSE(Board.Layer(Board.LayerCount(), Layer));
}

TEST_F(CCircuitBoardTest, RetrieveEveryLayer){
    auto Netlist = MakeNetlist();

    Board = CCircuitBoard::CreateBoardFromNetlist(Netlist);

    for(TLayerCoordinate Index = 0;
        Index < Board.LayerCount();
        Index++){

        CCircuitLayer Layer;

        EXPECT_TRUE(Board.Layer(Index, Layer));
    }
}

TEST_F(CCircuitBoardTest, RetrievedLayerContainsExpectedNets){
    auto Netlist = MakeNetlist();

    Board = CCircuitBoard::CreateBoardFromNetlist(Netlist);

    const std::vector<TCircuitName> Names = {
        "NetA",
        "NetB",
        "NetC"
    };

    for(TLayerCoordinate Index = 0;
        Index < Board.LayerCount();
        Index++){

        CCircuitLayer Layer;

        ASSERT_TRUE(Board.Layer(Index, Layer));

        // Each initial layer corresponds to a circuit.
        EXPECT_EQ(Layer.NetCount(), 1);
        CCircuitNet Net;
        EXPECT_TRUE(Layer.CircuitNet(0,Net));
        EXPECT_THAT(Names, ::testing::Contains(Net.Name()));
    }
}

TEST_F(CCircuitBoardTest, RetrievedNetlistContainsOriginalContent){
    auto Netlist = MakeNetlist();

    Board = CCircuitBoard::CreateBoardFromNetlist(Netlist);

    const std::vector<TCircuitName> Names = {
        "NetA",
        "NetB",
        "NetC"
    };
    EXPECT_EQ(Netlist.CircuitCount(), Board.Netlist().CircuitCount());
    for(std::size_t Index = 0; Index < Netlist.CircuitCount(); Index++){
        std::string Name;
        std::size_t OtherIndex;
        EXPECT_TRUE(Netlist.CircuitName(Index,Name));
        EXPECT_TRUE(Board.Netlist().CircuitNameToIndex(Name,OtherIndex));
        EXPECT_EQ(Netlist.PinCount(Index),Board.Netlist().PinCount(OtherIndex));
        std::vector<SCircuitLocation> Locations;
        std::vector<SCircuitLocation> OtherLocations;
        for(std::size_t PinIndex = 0; PinIndex < Netlist.PinCount(Index); PinIndex++){
            SCircuitLocation Location;
            EXPECT_TRUE(Netlist.PinLocation(Index,PinIndex,Location));
            Locations.push_back(Location);
            EXPECT_TRUE(Board.Netlist().PinLocation(OtherIndex,PinIndex,Location));
            OtherLocations.push_back(Location);
        }
        for(auto &Location : Locations){
            EXPECT_THAT(OtherLocations, ::testing::Contains(Location));
        }
    }


}


TEST_F(CCircuitBoardTest, BoardDimensionsAreCorrect){
    auto TestBoard = MakePopulatedBoard();

    SCircuitLocation ExpectedDimensions{F.DX+10,F.DY+10};
    EXPECT_EQ(TestBoard.Dimensions(),ExpectedDimensions);
}


TEST_F(CCircuitBoardTest, RetrievedLayerIsIndependentCopy){
    auto TestBoard = MakePopulatedBoard();

    CCircuitLayer Retrieved;

    ASSERT_TRUE(TestBoard.Layer(0, Retrieved));

    auto OriginalNetCount = Retrieved.NetCount();

    EXPECT_TRUE(Retrieved.AddPin("NetD", SCircuitLocation{50, 50}));

    EXPECT_EQ(Retrieved.NetCount(), OriginalNetCount + 1);

    CCircuitLayer Original;

    ASSERT_TRUE(TestBoard.Layer(0, Original));

    EXPECT_EQ(Original.NetCount(), OriginalNetCount);
    EXPECT_FALSE(HasNet(Original, "NetD"));
}


// CreateBoardFromNetlist

TEST_F(CCircuitBoardTest, CreateBoardFromEmptyNetlist){
    CCircuitNetlist Netlist;

    Board = CCircuitBoard::CreateBoardFromNetlist(Netlist);

    EXPECT_EQ(Board.LayerCount(), 0);
}

TEST_F(CCircuitBoardTest, CreateBoardFromSingleCircuit){
    CCircuitNetlist Netlist;

    ASSERT_TRUE(Netlist.AddPin("NetA", A));
    ASSERT_TRUE(Netlist.AddPin("NetA", B));

    Board = CCircuitBoard::CreateBoardFromNetlist(Netlist);

    EXPECT_EQ(Netlist.CircuitCount(), 1);
    EXPECT_EQ(Board.LayerCount(), 1);
}

TEST_F(CCircuitBoardTest, CreateBoardFromMultipleCircuits){
    auto Netlist = MakeNetlist();

    Board = CCircuitBoard::CreateBoardFromNetlist(Netlist);

    EXPECT_EQ(Netlist.CircuitCount(), 3);
    EXPECT_EQ(Board.LayerCount(), 3);
}

TEST_F(CCircuitBoardTest, CreateBoardFromNetlistCreatesOneLayerPerCircuit){
    auto Netlist = MakeNetlist();

    Board = CCircuitBoard::CreateBoardFromNetlist(Netlist);

    EXPECT_EQ(Board.LayerCount(), Netlist.CircuitCount());
}

TEST_F(CCircuitBoardTest, CreateBoardFromNetlistPreservesCircuitNames){
    auto Netlist = MakeNetlist();

    Board = CCircuitBoard::CreateBoardFromNetlist(Netlist);

    const std::vector<TCircuitName> Names = {
        "NetA",
        "NetB",
        "NetC"
    };

    for(TLayerCoordinate Index = 0;
        Index < Board.LayerCount();
        Index++){

        CCircuitLayer Layer;

        ASSERT_TRUE(Board.Layer(Index, Layer));

        // Each layer contains the net associated with its circuit.
        EXPECT_EQ(Layer.NetCount(), 1);
    }
}


// CreateLayerNets

TEST_F(CCircuitBoardTest, CreateLayerNetsOnEmptyBoard){
    EXPECT_FALSE(Board.CreateLayerNets());

    EXPECT_EQ(Board.LayerCount(), 0);
}

TEST_F(CCircuitBoardTest, CreateLayerNetsCreatesOneLayerPerCircuit){
    auto Netlist = MakeNetlist();

    Board = CCircuitBoard::CreateBoardFromNetlist(Netlist);

    ASSERT_TRUE(Board.CreateLayerNets());

    EXPECT_EQ(Board.LayerCount(), Netlist.CircuitCount());
}

TEST_F(CCircuitBoardTest, CreateLayerNetsCreatesConnectedNets){
    auto Netlist = MakeNetlist();

    Board = CCircuitBoard::CreateBoardFromNetlist(Netlist);

    ASSERT_TRUE(Board.CreateLayerNets());

    const std::vector<TCircuitName> Names = {
        "NetA",
        "NetB",
        "NetC"
    };

    for(TLayerCoordinate Index = 0;
        Index < Board.LayerCount();
        Index++){

        CCircuitLayer Layer;

        ASSERT_TRUE(Board.Layer(Index, Layer));

        ExpectAllNetsConnected(Layer, Names);
    }
}

TEST_F(CCircuitBoardTest, CreateLayerNetsDoesNotCreateBrokenNets){
    auto Netlist = MakeNetlist();

    Board = CCircuitBoard::CreateBoardFromNetlist(Netlist);

    ASSERT_TRUE(Board.CreateLayerNets());

    for(TLayerCoordinate Index = 0;
        Index < Board.LayerCount();
        Index++){

        CCircuitLayer Layer;

        ASSERT_TRUE(Board.Layer(Index, Layer));

        HasNoBrokenNets(Layer);
    }
}

TEST_F(CCircuitBoardTest, CreateLayerNetsDoesNotCreateShortedNets){
    auto Netlist = MakeNetlist();

    Board = CCircuitBoard::CreateBoardFromNetlist(Netlist);

    ASSERT_TRUE(Board.CreateLayerNets());

    for(TLayerCoordinate Index = 0;
        Index < Board.LayerCount();
        Index++){

        CCircuitLayer Layer;

        ASSERT_TRUE(Board.Layer(Index, Layer));

        HasNoShortedNets(Layer);
    }
}

TEST_F(CCircuitBoardTest, CreateLayerNetsWithSingleCircuit){
    CCircuitNetlist Netlist;

    ASSERT_TRUE(Netlist.AddPin("NetA", A));
    ASSERT_TRUE(Netlist.AddPin("NetA", B));

    Board = CCircuitBoard::CreateBoardFromNetlist(Netlist);

    ASSERT_TRUE(Board.CreateLayerNets());

    EXPECT_EQ(Board.LayerCount(), 1);

    CCircuitLayer Layer;

    ASSERT_TRUE(Board.Layer(0, Layer));

    EXPECT_TRUE(HasNet(Layer, "NetA"));

    CCircuitNet Net;

    ASSERT_TRUE(Layer.CircuitNet("NetA", Net));

    EXPECT_EQ(Net.PinCount(), 2);
    EXPECT_FALSE(Net.IsBroken());
}

TEST_F(CCircuitBoardTest, CreateLayerNetsCanBeCalledRepeatedly){
    auto Netlist = MakeNetlist();

    Board = CCircuitBoard::CreateBoardFromNetlist(Netlist);

    ASSERT_TRUE(Board.CreateLayerNets());

    auto OriginalLayerCount = Board.LayerCount();

    EXPECT_TRUE(Board.CreateLayerNets());

    EXPECT_EQ(Board.LayerCount(), OriginalLayerCount);
}


// MinimizeLayers

TEST_F(CCircuitBoardTest, MinimizeLayersOnEmptyBoard){
    EXPECT_FALSE(Board.MinimizeLayers());

    EXPECT_EQ(Board.LayerCount(), 0);
}

TEST_F(CCircuitBoardTest, MinimizeLayersAfterCreatingLayerNets){
    auto Netlist = MakeNetlist();

    Board = CCircuitBoard::CreateBoardFromNetlist(Netlist);

    ASSERT_TRUE(Board.CreateLayerNets());

    auto OriginalLayerCount = Board.LayerCount();

    EXPECT_TRUE(Board.MinimizeLayers());

    EXPECT_LE(Board.LayerCount(), OriginalLayerCount);
}

TEST_F(CCircuitBoardTest, MinimizeLayersDoesNotIncreaseLayerCount){
    auto Netlist = MakeNetlist();

    Board = CCircuitBoard::CreateBoardFromNetlist(Netlist);

    ASSERT_TRUE(Board.CreateLayerNets());

    auto OriginalLayerCount = Board.LayerCount();

    ASSERT_TRUE(Board.MinimizeLayers());

    EXPECT_LE(Board.LayerCount(), OriginalLayerCount);
}

TEST_F(CCircuitBoardTest, MinimizeLayersPreservesCircuitNets){
    auto Netlist = MakeNetlist();

    Board = CCircuitBoard::CreateBoardFromNetlist(Netlist);

    ASSERT_TRUE(Board.CreateLayerNets());
    ASSERT_TRUE(Board.MinimizeLayers());

    const std::vector<TCircuitName> Names = {
        "NetA",
        "NetB",
        "NetC"
    };

    for(TLayerCoordinate Index = 0;
        Index < Board.LayerCount();
        Index++){

        CCircuitLayer Layer;

        ASSERT_TRUE(Board.Layer(Index, Layer));

        for(const auto &Name : Names){
            CCircuitNet Net;

            EXPECT_TRUE(Layer.CircuitNet(Name, Net));
        }
    }
}

TEST_F(CCircuitBoardTest, MinimizeLayersPreservesConnectivity){
    auto Netlist = MakeNetlist();

    Board = CCircuitBoard::CreateBoardFromNetlist(Netlist);

    ASSERT_TRUE(Board.CreateLayerNets());
    ASSERT_TRUE(Board.MinimizeLayers());

    const std::vector<TCircuitName> Names = {
        "NetA",
        "NetB",
        "NetC"
    };

    for(TLayerCoordinate Index = 0;
        Index < Board.LayerCount();
        Index++){

        CCircuitLayer Layer;

        ASSERT_TRUE(Board.Layer(Index, Layer));

        ExpectAllNetsConnected(Layer, Names);
    }
}

TEST_F(CCircuitBoardTest, MinimizeLayersDoesNotCreateShortedNets){
    auto Netlist = MakeNetlist();

    Board = CCircuitBoard::CreateBoardFromNetlist(Netlist);

    ASSERT_TRUE(Board.CreateLayerNets());
    ASSERT_TRUE(Board.MinimizeLayers());

    for(TLayerCoordinate Index = 0;
        Index < Board.LayerCount();
        Index++){

        CCircuitLayer Layer;

        ASSERT_TRUE(Board.Layer(Index, Layer));

        HasNoShortedNets(Layer);
    }
}

TEST_F(CCircuitBoardTest, MinimizeLayersDoesNotCreateBrokenNets){
    auto Netlist = MakeNetlist();

    Board = CCircuitBoard::CreateBoardFromNetlist(Netlist);

    ASSERT_TRUE(Board.CreateLayerNets());
    ASSERT_TRUE(Board.MinimizeLayers());

    for(TLayerCoordinate Index = 0;
        Index < Board.LayerCount();
        Index++){

        CCircuitLayer Layer;

        ASSERT_TRUE(Board.Layer(Index, Layer));

        HasNoBrokenNets(Layer);
    }
}

TEST_F(CCircuitBoardTest, MinimizeLayersCanBeCalledRepeatedly){
    auto Netlist = MakeNetlist();

    Board = CCircuitBoard::CreateBoardFromNetlist(Netlist);

    ASSERT_TRUE(Board.CreateLayerNets());
    ASSERT_TRUE(Board.MinimizeLayers());

    auto OriginalLayerCount = Board.LayerCount();

    EXPECT_TRUE(Board.MinimizeLayers());

    EXPECT_LE(Board.LayerCount(), OriginalLayerCount);
}


TEST_F(CCircuitBoardTest, MinimizeLayersWontShortNets){
    CCircuitNetlist Netlist;

    EXPECT_TRUE(Netlist.AddPin("NetA", A));
    EXPECT_TRUE(Netlist.AddPin("NetA", D));

    EXPECT_TRUE(Netlist.AddPin("NetB", B));
    EXPECT_TRUE(Netlist.AddPin("NetB", C));

    EXPECT_TRUE(Netlist.AddPin("NetC", E));
    EXPECT_TRUE(Netlist.AddPin("NetC", F));

    Board = CCircuitBoard::CreateBoardFromNetlist(Netlist);

    ASSERT_TRUE(Board.CreateLayerNets());
    auto OriginalLayerCount = Board.LayerCount();
    ASSERT_TRUE(Board.MinimizeLayers());

    EXPECT_LE(Board.LayerCount(), OriginalLayerCount);
    std::size_t NetAIndex = 11, NetBIndex = 11;
    for(std::size_t Index = 0; Index < Board.LayerCount(); Index++){
        CCircuitLayer Layer;
        ASSERT_TRUE(Board.Layer(Index,Layer));
        CCircuitNet Net;
        if(Layer.CircuitNet("NetA",Net)){
            NetAIndex = Index;
        }
        if(Layer.CircuitNet("NetB",Net)){
            NetBIndex = Index;
        }
    }
    EXPECT_NE(NetAIndex,NetBIndex);
    EXPECT_LT(NetAIndex,Board.LayerCount());
    EXPECT_LT(NetBIndex,Board.LayerCount());
}
