
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "CircuitLayer.h"

#include <algorithm>
#include <initializer_list>
#include <string>

class CCircuitLayerTest : public ::testing::Test {
protected:
    CCircuitLayer Layer;

    SCircuitLocation A{0, 0};
    SCircuitLocation B{10, 10};
    SCircuitLocation C{20, 20};
    SCircuitLocation D{30, 30};
    SCircuitLocation E{40, 40};

    SCircuitTraceEdge EdgeAB{A, B};
    SCircuitTraceEdge EdgeBC{B, C};
    SCircuitTraceEdge EdgeCD{C, D};

    CCircuitTrace MakeTrace(
        const TCircuitName &Name,
        std::initializer_list<SCircuitTraceEdge> Edges){

        CCircuitTrace Trace;
        Trace.Name(Name);

        for(const auto &Edge : Edges){
            EXPECT_TRUE(Trace.AddEdge(Edge));
        }

        return Trace;
    }

    CCircuitNet MakeNet(
        const TCircuitName &Name,
        std::initializer_list<SCircuitLocation> Pins,
        std::initializer_list<SCircuitTraceEdge> Edges = {}){

        CCircuitNet Net;
        Net.Name(Name);

        for(const auto &Pin : Pins){
            EXPECT_TRUE(Net.AddPinLocation(Pin));
        }

        if(Edges.size() > 0){
            auto Trace = MakeTrace(Name, Edges);
            EXPECT_TRUE(Net.AddTrace(Trace));
        }
        return Net;
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

    bool HasShortedPair(
        const std::vector<std::pair<std::string, std::string>> &Nets,
        const std::string &Name1,
        const std::string &Name2){

        for(const auto &Pair : Nets){
            if((Pair.first == Name1 && Pair.second == Name2) ||
               (Pair.first == Name2 && Pair.second == Name1)){
                return true;
            }
        }

        return false;
    }

    std::size_t TotalPinCount(const CCircuitNet &Net){
        return Net.PinCount();
    }
};


// Construction

TEST_F(CCircuitLayerTest, DefaultConstruction){
    EXPECT_EQ(Layer.PinCount(), 0);
    EXPECT_EQ(Layer.NetCount(), 0);
}

TEST_F(CCircuitLayerTest, EmptyLayerHasNoPins){
    SCircuitLocation Location{100, 100};

    EXPECT_FALSE(Layer.PinLocation(0, Location));

    ExpectLocation(Location, SCircuitLocation{100, 100});
}

TEST_F(CCircuitLayerTest, MoveConstructionOfEmptyLayer){
    CCircuitLayer Moved(std::move(Layer));

    EXPECT_EQ(Moved.PinCount(), 0);
}

TEST_F(CCircuitLayerTest, MoveConstructionOfPopulatedLayer){
    Layer.AddPin("NetA",A);
    Layer.AddPin("NetA",B);

    auto OriginalPinCount = Layer.PinCount();
    CCircuitLayer Moved(std::move(Layer));

    EXPECT_EQ(Moved.PinCount(), OriginalPinCount);
    std::vector<SCircuitLocation> PinLocations{A,B};
    for(std::size_t Index = 0; Index < Moved.PinCount(); Index++){
        SCircuitLocation Location;
        EXPECT_TRUE(Moved.PinLocation(Index,Location));
        EXPECT_THAT(PinLocations, ::testing::Contains(Location));
    }
}


TEST_F(CCircuitLayerTest, EmptyLayerHasNoNets){
    CCircuitNet Net;

    EXPECT_FALSE(Layer.CircuitNet(0, Net));
    EXPECT_FALSE(Layer.CircuitNet("Missing", Net));
}


// Pins

TEST_F(CCircuitLayerTest, PinCountInitiallyZero){
    EXPECT_EQ(Layer.PinCount(), 0);
}

TEST_F(CCircuitLayerTest, PinLocationOutOfBounds){
    SCircuitLocation Location{100, 100};

    EXPECT_FALSE(Layer.PinLocation(0, Location));

    ExpectLocation(Location, SCircuitLocation{100, 100});
}

TEST_F(CCircuitLayerTest, AddFirstPin){
    EXPECT_TRUE(Layer.AddPin("NetA", A));

    EXPECT_EQ(Layer.PinCount(), 1);
    EXPECT_TRUE(HasPin(Layer, A));
}

TEST_F(CCircuitLayerTest, AddMultiplePins){
    EXPECT_TRUE(Layer.AddPin("NetA", A));
    EXPECT_TRUE(Layer.AddPin("NetA", B));
    EXPECT_TRUE(Layer.AddPin("NetA", C));

    EXPECT_EQ(Layer.PinCount(), 3);

    EXPECT_TRUE(HasPin(Layer, A));
    EXPECT_TRUE(HasPin(Layer, B));
    EXPECT_TRUE(HasPin(Layer, C));
}

TEST_F(CCircuitLayerTest, PinRetrieval){
    EXPECT_TRUE(Layer.AddPin("NetA", A));
    EXPECT_TRUE(Layer.AddPin("NetA", B));

    SCircuitLocation Location;

    EXPECT_TRUE(Layer.PinLocation(0, Location));
    EXPECT_TRUE(HasPin(Layer, Location));

    EXPECT_TRUE(Layer.PinLocation(1, Location));
    EXPECT_TRUE(HasPin(Layer, Location));
}

TEST_F(CCircuitLayerTest, PinLocationAtInvalidIndex){
    EXPECT_TRUE(Layer.AddPin("NetA", A));

    SCircuitLocation Location{100, 100};

    EXPECT_FALSE(Layer.PinLocation(1, Location));

    ExpectLocation(Location, SCircuitLocation{100, 100});
}

TEST_F(CCircuitLayerTest, AddPinToDifferentNets){
    EXPECT_TRUE(Layer.AddPin("NetA", A));
    EXPECT_TRUE(Layer.AddPin("NetB", B));

    EXPECT_EQ(Layer.NetCount(), 2);

    EXPECT_TRUE(HasPin(Layer, A));
    EXPECT_TRUE(HasPin(Layer, B));
}

TEST_F(CCircuitLayerTest, AddDuplicatePin){
    EXPECT_TRUE(Layer.AddPin("NetA", A));
    EXPECT_FALSE(Layer.AddPin("NetA", A));

    EXPECT_EQ(Layer.NetCount(), 1);
}

TEST_F(CCircuitLayerTest, AddSameLocationToDifferentNets){
    EXPECT_TRUE(Layer.AddPin("NetA", A));
    EXPECT_FALSE(Layer.AddPin("NetB", A));

    EXPECT_EQ(Layer.NetCount(), 1);

    EXPECT_TRUE(HasNet(Layer, "NetA"));
    EXPECT_FALSE(HasNet(Layer, "NetB"));
}


// Pin Locations By Net

TEST_F(CCircuitLayerTest, PinLocationsForExistingNet){
    EXPECT_TRUE(Layer.AddPin("NetA", A));
    EXPECT_TRUE(Layer.AddPin("NetA", B));
    EXPECT_TRUE(Layer.AddPin("NetA", C));

    std::vector<SCircuitLocation> Locations;

    EXPECT_TRUE(Layer.PinLocations("NetA", Locations));

    EXPECT_EQ(Locations.size(), 3);

    EXPECT_TRUE(HasLocation(Locations, A));
    EXPECT_TRUE(HasLocation(Locations, B));
    EXPECT_TRUE(HasLocation(Locations, C));
}

TEST_F(CCircuitLayerTest, PinLocationsForDifferentNets){
    EXPECT_TRUE(Layer.AddPin("NetA", A));
    EXPECT_TRUE(Layer.AddPin("NetA", B));
    EXPECT_TRUE(Layer.AddPin("NetB", C));

    std::vector<SCircuitLocation> Locations;

    ASSERT_TRUE(Layer.PinLocations("NetA", Locations));

    EXPECT_EQ(Locations.size(), 2);
    EXPECT_TRUE(HasLocation(Locations, A));
    EXPECT_TRUE(HasLocation(Locations, B));
    EXPECT_FALSE(HasLocation(Locations, C));

    Locations.clear();

    ASSERT_TRUE(Layer.PinLocations("NetB", Locations));

    EXPECT_EQ(Locations.size(), 1);
    EXPECT_TRUE(HasLocation(Locations, C));
}

TEST_F(CCircuitLayerTest, PinLocationsForMissingNet){
    std::vector<SCircuitLocation> Locations;

    EXPECT_FALSE(Layer.PinLocations("Missing", Locations));
}

TEST_F(CCircuitLayerTest, PinLocationsForNetWithoutPins){
    auto Net = MakeNet("NetA", {});

    ASSERT_TRUE(Layer.AddCircuitNet(Net));

    std::vector<SCircuitLocation> Locations;

    EXPECT_TRUE(Layer.PinLocations("NetA", Locations));
    EXPECT_TRUE(Locations.empty());
}


// Nets

TEST_F(CCircuitLayerTest, NetCountInitiallyZero){
    EXPECT_EQ(Layer.NetCount(), 0);
}

TEST_F(CCircuitLayerTest, AddFirstNet){
    auto Net = MakeNet("NetA", {A, B});

    EXPECT_TRUE(Layer.AddCircuitNet(Net));

    EXPECT_EQ(Layer.NetCount(), 1);
    EXPECT_TRUE(HasNet(Layer, "NetA"));
}

TEST_F(CCircuitLayerTest, AddMultipleNets){
    auto NetA = MakeNet("NetA", {A, B});
    auto NetB = MakeNet("NetB", {C, D});

    EXPECT_TRUE(Layer.AddCircuitNet(NetA));
    EXPECT_TRUE(Layer.AddCircuitNet(NetB));

    EXPECT_EQ(Layer.NetCount(), 2);

    EXPECT_TRUE(HasNet(Layer, "NetA"));
    EXPECT_TRUE(HasNet(Layer, "NetB"));
}

TEST_F(CCircuitLayerTest, CircuitNetOutOfBounds){
    auto NetA = MakeNet("NetA", {A});

    ASSERT_TRUE(Layer.AddCircuitNet(NetA));

    CCircuitNet Retrieved;

    EXPECT_FALSE(Layer.CircuitNet(1, Retrieved));
}

TEST_F(CCircuitLayerTest, CircuitNetByNameOutOfBounds){
    CCircuitNet Retrieved;

    EXPECT_FALSE(Layer.CircuitNet("Missing", Retrieved));
}

TEST_F(CCircuitLayerTest, CircuitNetByIndex){
    auto NetA = MakeNet("NetA", {A, B});
    auto NetB = MakeNet("NetB", {C, D});

    ASSERT_TRUE(Layer.AddCircuitNet(NetA));
    ASSERT_TRUE(Layer.AddCircuitNet(NetB));

    EXPECT_EQ(Layer.NetCount(), 2);

    for(std::size_t Index = 0; Index < Layer.NetCount(); Index++){
        CCircuitNet Retrieved;

        ASSERT_TRUE(Layer.CircuitNet(Index, Retrieved));

        EXPECT_TRUE(
            Retrieved.Name() == "NetA" ||
            Retrieved.Name() == "NetB"
        );
    }
}

TEST_F(CCircuitLayerTest, CircuitNetByName){
    auto NetA = MakeNet("NetA", {A, B});

    ASSERT_TRUE(Layer.AddCircuitNet(NetA));

    CCircuitNet Retrieved;

    ASSERT_TRUE(Layer.CircuitNet("NetA", Retrieved));

    EXPECT_EQ(Retrieved.Name(), "NetA");
    EXPECT_EQ(Retrieved.PinCount(), 2);
    EXPECT_TRUE(Retrieved.IsBroken());
}

TEST_F(CCircuitLayerTest, CircuitNetRetrievalReturnsIndependentCopy){
    auto NetA = MakeNet("NetA", {A, B});

    ASSERT_TRUE(Layer.AddCircuitNet(NetA));

    CCircuitNet Retrieved;

    ASSERT_TRUE(Layer.CircuitNet("NetA", Retrieved));

    EXPECT_TRUE(Retrieved.AddPinLocation(C));

    EXPECT_EQ(Retrieved.PinCount(), 3);

    CCircuitNet Original;

    ASSERT_TRUE(Layer.CircuitNet("NetA", Original));

    EXPECT_EQ(Original.PinCount(), 2);
}


// AddCircuitNet

TEST_F(CCircuitLayerTest, AddCircuitNetWithPins){
    auto Net = MakeNet("NetA", {A, B, C});

    EXPECT_TRUE(Layer.AddCircuitNet(Net));

    EXPECT_EQ(Layer.NetCount(), 1);

    CCircuitNet Retrieved;

    ASSERT_TRUE(Layer.CircuitNet("NetA", Retrieved));

    EXPECT_EQ(Retrieved.PinCount(), 3);
}

TEST_F(CCircuitLayerTest, AddCircuitNetWithTraces){
    auto Net = MakeNet("NetA", {A, B}, {EdgeAB});

    EXPECT_TRUE(Layer.AddCircuitNet(Net));

    CCircuitNet Retrieved;

    ASSERT_TRUE(Layer.CircuitNet("NetA", Retrieved));

    EXPECT_EQ(Retrieved.TraceCount(), 1);
    EXPECT_FALSE(Retrieved.IsBroken());
}

TEST_F(CCircuitLayerTest, AddEmptyCircuitNet){
    auto Net = MakeNet("NetA", {});

    EXPECT_TRUE(Layer.AddCircuitNet(Net));

    EXPECT_EQ(Layer.NetCount(), 1);
}

TEST_F(CCircuitLayerTest, AddDuplicateCircuitNetWithoutMerge){
    std::cout<<"IN "<<__FILE__<<" @ "<<__LINE__<<std::endl;
    auto NetA = MakeNet("NetA", {A, B});
    std::cout<<"IN "<<__FILE__<<" @ "<<__LINE__<<std::endl;
    auto NetB = MakeNet("NetA", {C, D});
    std::cout<<"IN "<<__FILE__<<" @ "<<__LINE__<<std::endl;

    EXPECT_TRUE(Layer.AddCircuitNet(NetA));
    std::cout<<"IN "<<__FILE__<<" @ "<<__LINE__<<std::endl;
    EXPECT_FALSE(Layer.AddCircuitNet(NetB));

    EXPECT_EQ(Layer.NetCount(), 1);

    CCircuitNet Retrieved;

    ASSERT_TRUE(Layer.CircuitNet("NetA", Retrieved));

    EXPECT_EQ(Retrieved.PinCount(), 2);
}

TEST_F(CCircuitLayerTest, AddDifferentNamesWithoutMerge){
    auto NetA = MakeNet("NetA", {A});
    auto NetB = MakeNet("NetB", {B});

    EXPECT_TRUE(Layer.AddCircuitNet(NetA));
    EXPECT_TRUE(Layer.AddCircuitNet(NetB));

    EXPECT_EQ(Layer.NetCount(), 2);
}


// Net Merging

TEST_F(CCircuitLayerTest, MergeExistingNet){
    auto NetA = MakeNet("NetA", {A, B});
    auto NetB = MakeNet("NetA", {C, D});

    ASSERT_TRUE(Layer.AddCircuitNet(NetA));

    EXPECT_TRUE(Layer.AddCircuitNet(NetB, true));

    EXPECT_EQ(Layer.NetCount(), 1);

    CCircuitNet Retrieved;

    ASSERT_TRUE(Layer.CircuitNet("NetA", Retrieved));

    EXPECT_EQ(Retrieved.PinCount(), 4);

    EXPECT_TRUE(HasLocation(
        std::vector<SCircuitLocation>{A, B, C, D}, A));
}

TEST_F(CCircuitLayerTest, MergePreservesExistingPins){
    auto NetA = MakeNet("NetA", {A, B});
    auto NetB = MakeNet("NetA", {C});

    ASSERT_TRUE(Layer.AddCircuitNet(NetA));
    ASSERT_TRUE(Layer.AddCircuitNet(NetB, true));

    CCircuitNet Retrieved;

    ASSERT_TRUE(Layer.CircuitNet("NetA", Retrieved));

    EXPECT_EQ(Retrieved.PinCount(), 3);

    std::vector<SCircuitLocation> Locations;

    ASSERT_TRUE(Layer.PinLocations("NetA", Locations));

    EXPECT_TRUE(HasLocation(Locations, A));
    EXPECT_TRUE(HasLocation(Locations, B));
    EXPECT_TRUE(HasLocation(Locations, C));
}

TEST_F(CCircuitLayerTest, MergeDoesNotDuplicatePins){
    auto NetA = MakeNet("NetA", {A, B});
    auto NetB = MakeNet("NetA", {B, C});

    ASSERT_TRUE(Layer.AddCircuitNet(NetA));
    EXPECT_TRUE(Layer.AddCircuitNet(NetB, true));

    CCircuitNet Retrieved;

    ASSERT_TRUE(Layer.CircuitNet("NetA", Retrieved));

    EXPECT_EQ(Retrieved.PinCount(), 3);
}

TEST_F(CCircuitLayerTest, MergePreservesTraces){
    auto NetA = MakeNet("NetA", {A, B}, {EdgeAB});
    auto NetB = MakeNet("NetA", {C, D}, {EdgeCD});

    ASSERT_TRUE(Layer.AddCircuitNet(NetA));
    ASSERT_TRUE(Layer.AddCircuitNet(NetB, true));

    CCircuitNet Retrieved;

    ASSERT_TRUE(Layer.CircuitNet("NetA", Retrieved));

    EXPECT_EQ(Retrieved.TraceCount(), 2);
}

TEST_F(CCircuitLayerTest, MergeDifferentNetDoesNotChangeNetCount){
    auto NetA = MakeNet("NetA", {A});
    auto NetB = MakeNet("NetB", {B});

    ASSERT_TRUE(Layer.AddCircuitNet(NetA));
    ASSERT_TRUE(Layer.AddCircuitNet(NetB));

    EXPECT_TRUE(Layer.AddCircuitNet(NetA, true));

    EXPECT_EQ(Layer.NetCount(), 2);
}


// AddPin And Existing Nets

TEST_F(CCircuitLayerTest, AddPinToExistingCircuitNet){
    auto NetA = MakeNet("NetA", {A});

    ASSERT_TRUE(Layer.AddCircuitNet(NetA));

    EXPECT_TRUE(Layer.AddPin("NetA", B));

    CCircuitNet Retrieved;

    ASSERT_TRUE(Layer.CircuitNet("NetA", Retrieved));

    EXPECT_EQ(Retrieved.PinCount(), 2);
    EXPECT_EQ(Layer.NetCount(), 1);
}

TEST_F(CCircuitLayerTest, AddPinCreatesNewCircuitNet){
    EXPECT_TRUE(Layer.AddPin("NetA", A));

    EXPECT_EQ(Layer.NetCount(), 1);
    EXPECT_TRUE(HasNet(Layer, "NetA"));
}

TEST_F(CCircuitLayerTest, AddPinToExistingNetDoesNotCreateAnotherNet){
    EXPECT_TRUE(Layer.AddPin("NetA", A));
    EXPECT_TRUE(Layer.AddPin("NetA", B));
    EXPECT_TRUE(Layer.AddPin("NetA", C));

    EXPECT_EQ(Layer.NetCount(), 1);
}

TEST_F(CCircuitLayerTest, AddPinToAnotherNet){
    EXPECT_TRUE(Layer.AddPin("NetA", A));
    EXPECT_TRUE(Layer.AddPin("NetB", B));

    EXPECT_EQ(Layer.NetCount(), 2);
}


// Shorted Nets

TEST_F(CCircuitLayerTest, NoShortedNets){
    auto NetA = MakeNet("NetA", {A, B});
    auto NetB = MakeNet("NetB", {C, D});

    ASSERT_TRUE(Layer.AddCircuitNet(NetA));
    ASSERT_TRUE(Layer.AddCircuitNet(NetB));

    std::vector<std::pair<TCircuitName,TCircuitName>> Nets;

    EXPECT_FALSE(Layer.GetShortedNets(Nets));
    EXPECT_TRUE(Nets.empty());
}

TEST_F(CCircuitLayerTest, EmptyLayerHasNoShortedNets){
    std::vector<std::pair<TCircuitName,TCircuitName>> Nets;

    EXPECT_FALSE(Layer.GetShortedNets(Nets));
}

TEST_F(CCircuitLayerTest, TwoNetsIntersectingAreShorted){
    SCircuitLocation F{0,5}, G{10,5};
    auto NetA = MakeNet("NetA", {A, B}, {{A,B}});
    auto NetB = MakeNet("NetB", {F, G}, {{F,G}});

    ASSERT_TRUE(Layer.AddCircuitNet(NetA));
    ASSERT_TRUE(Layer.AddCircuitNet(NetB));

    std::vector<std::pair<TCircuitName,TCircuitName>> Nets;

    EXPECT_TRUE(Layer.GetShortedNets(Nets));

    EXPECT_TRUE(HasShortedPair(Nets, "NetA", "NetB"));
}

TEST_F(CCircuitLayerTest, MultipleShortedNets){
    SCircuitLocation F{0,5}, G{10,5}, H{5,0}, I{5,10};
    auto NetA = MakeNet("NetA", {A, B}, {{A,B}});
    auto NetB = MakeNet("NetB", {F, G}, {{F,G}});
    auto NetC = MakeNet("NetC", {H, I}, {{H,I}});

    ASSERT_TRUE(Layer.AddCircuitNet(NetA));
    ASSERT_TRUE(Layer.AddCircuitNet(NetB));
    ASSERT_TRUE(Layer.AddCircuitNet(NetC));

    std::vector<std::pair<TCircuitName,TCircuitName>> Nets;

    EXPECT_TRUE(Layer.GetShortedNets(Nets));

    EXPECT_TRUE(HasShortedPair(Nets, "NetA", "NetB"));
    EXPECT_TRUE(HasShortedPair(Nets, "NetA", "NetC"));
    EXPECT_TRUE(HasShortedPair(Nets, "NetB", "NetC"));
}

TEST_F(CCircuitLayerTest, ShortedNetsAreNotReportedForDifferentLocations){
    auto NetA = MakeNet("NetA", {A});
    auto NetB = MakeNet("NetB", {B});

    ASSERT_TRUE(Layer.AddCircuitNet(NetA));
    ASSERT_TRUE(Layer.AddCircuitNet(NetB));

    std::vector<std::pair<TCircuitName,TCircuitName>> Nets;

    EXPECT_FALSE(Layer.GetShortedNets(Nets));
}


// Broken Nets

TEST_F(CCircuitLayerTest, EmptyLayerHasNoBrokenNets){
    std::vector<TCircuitName> Nets;

    EXPECT_FALSE(Layer.GetBrokenNets(Nets));
}

TEST_F(CCircuitLayerTest, LayerWithValidNetHasNoBrokenNets){
    auto NetA = MakeNet("NetA", {A, B}, {EdgeAB});

    ASSERT_TRUE(Layer.AddCircuitNet(NetA));

    std::vector<TCircuitName> Nets;

    EXPECT_FALSE(Layer.GetBrokenNets(Nets));
}

TEST_F(CCircuitLayerTest, LayerWithBrokenNetReportsBrokenNet){
    auto NetA = MakeNet("NetA", {A, B});

    ASSERT_TRUE(Layer.AddCircuitNet(NetA));

    std::vector<TCircuitName> Nets;

    EXPECT_TRUE(Layer.GetBrokenNets(Nets));
    EXPECT_FALSE(Nets.empty());
}

TEST_F(CCircuitLayerTest, LayerWithValidAndBrokenNets){
    auto NetA = MakeNet("NetA", {A, B}, {EdgeAB});
    auto NetB = MakeNet("NetB", {C, D});

    ASSERT_TRUE(Layer.AddCircuitNet(NetA));
    ASSERT_TRUE(Layer.AddCircuitNet(NetB));

    std::vector<TCircuitName> Nets;

    EXPECT_TRUE(Layer.GetBrokenNets(Nets));
    EXPECT_FALSE(Nets.empty());
}

TEST_F(CCircuitLayerTest, LayerWithMultipleBrokenNets){
    auto NetA = MakeNet("NetA", {A, B});
    auto NetB = MakeNet("NetB", {C, D});

    ASSERT_TRUE(Layer.AddCircuitNet(NetA));
    ASSERT_TRUE(Layer.AddCircuitNet(NetB));

    std::vector<TCircuitName> Nets;

    EXPECT_TRUE(Layer.GetBrokenNets(Nets));
    EXPECT_FALSE(Nets.empty());
}

TEST_F(CCircuitLayerTest, BrokenNetDoesNotPreventAddingAnotherNet){
    auto NetA = MakeNet("NetA", {A, B});
    auto NetB = MakeNet("NetB", {C, D}, {EdgeCD});

    ASSERT_TRUE(Layer.AddCircuitNet(NetA));

    EXPECT_TRUE(Layer.AddCircuitNet(NetB));

    EXPECT_EQ(Layer.NetCount(), 2);
}

// Pins Independent Of Nets

TEST_F(CCircuitLayerTest, AddPinDoesCreateCircuitNet){
    EXPECT_TRUE(Layer.AddPin("NetA", A));

    EXPECT_EQ(Layer.PinCount(), 1);
    EXPECT_EQ(Layer.NetCount(), 1);

    EXPECT_TRUE(HasPin(Layer, A));
    EXPECT_TRUE(HasNet(Layer, "NetA"));
}

TEST_F(CCircuitLayerTest, AddMultiplePinsWithCreatingNets){
    EXPECT_TRUE(Layer.AddPin("NetA", A));
    EXPECT_TRUE(Layer.AddPin("NetA", B));
    EXPECT_TRUE(Layer.AddPin("NetB", C));

    EXPECT_EQ(Layer.PinCount(), 3);
    EXPECT_EQ(Layer.NetCount(), 2);
}

TEST_F(CCircuitLayerTest, AddPinWhenNetAlreadyExists){
    auto Net = MakeNet("NetA", {B});

    ASSERT_TRUE(Layer.AddCircuitNet(Net));

    EXPECT_TRUE(Layer.AddPin("NetA", A));

    EXPECT_EQ(Layer.NetCount(), 1);
    EXPECT_EQ(Layer.PinCount(), 2);

    CCircuitNet Retrieved;
    ASSERT_TRUE(Layer.CircuitNet("NetA", Retrieved));

    // The layer pin is independent of the net's pin collection.
    EXPECT_EQ(Retrieved.PinCount(), 2);
}

TEST_F(CCircuitLayerTest, AddPinsForMultipleNetsWithAddingNets){
    EXPECT_TRUE(Layer.AddPin("NetA", A));
    EXPECT_TRUE(Layer.AddPin("NetB", B));
    EXPECT_TRUE(Layer.AddPin("NetC", C));

    EXPECT_EQ(Layer.PinCount(), 3);
    EXPECT_EQ(Layer.NetCount(), 3);
}

TEST_F(CCircuitLayerTest, PinLocationsAvailableWithCircuitNet){
    EXPECT_TRUE(Layer.AddPin("NetA", A));
    EXPECT_TRUE(Layer.AddPin("NetA", B));

    std::vector<SCircuitLocation> Locations;

    EXPECT_TRUE(Layer.PinLocations("NetA", Locations));

    EXPECT_EQ(Locations.size(), 2);
    EXPECT_TRUE(HasLocation(Locations, A));
    EXPECT_TRUE(HasLocation(Locations, B));

    EXPECT_EQ(Layer.NetCount(), 1);
}


