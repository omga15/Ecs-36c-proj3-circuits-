
#include <gtest/gtest.h>
#include "CircuitNet.h"

class CCircuitNetTest : public ::testing::Test {
protected:
    CCircuitNet Net;

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

    bool SameLocation(
        const SCircuitLocation &Actual,
        const SCircuitLocation &Expected){
        
        return Actual.DX == Expected.DX &&
               Actual.DY == Expected.DY;
    }

    bool SameEdge(
        const SCircuitTraceEdge &Actual,
        const SCircuitTraceEdge &Expected){
        
        return SameLocation(Actual.DSource, Expected.DSource) &&
               SameLocation(Actual.DDestination, Expected.DDestination);
    }

    void ExpectLocation(
        const SCircuitLocation &Actual,
        const SCircuitLocation &Expected){
        
        EXPECT_EQ(Actual.DX, Expected.DX);
        EXPECT_EQ(Actual.DY, Expected.DY);
    }

    void ExpectEdge(
        const SCircuitTraceEdge &Actual,
        const SCircuitTraceEdge &Expected){
        
        ExpectLocation(Actual.DSource, Expected.DSource);
        ExpectLocation(Actual.DDestination, Expected.DDestination);
    }

    bool HasPin(const CCircuitNet &TestNet, const SCircuitLocation &Expected){
        for(std::size_t Index = 0; Index < TestNet.PinCount(); Index++){
            SCircuitLocation Location;
            if(TestNet.PinLocation(Index, Location) &&
               SameLocation(Location, Expected)){
                return true;
            }
        }
        return false;
    }

    bool HasEdge(const CCircuitNet &TestNet, const SCircuitTraceEdge &Expected, bool RequiredExact = true){
        for(std::size_t TraceIndex = 0; TraceIndex < TestNet.TraceCount(); TraceIndex++){
            CCircuitTrace Trace;
            if(!TestNet.Trace(TraceIndex, Trace)){
                continue;
            }

            for(std::size_t EdgeIndex = 0; EdgeIndex < Trace.EdgeCount(); EdgeIndex++){
                SCircuitTraceEdge Edge;
                if(Trace.Edge(EdgeIndex, Edge) && (SameEdge(Edge, Expected) || (!RequiredExact && SameEdge({Edge.DDestination,Edge.DSource}, Expected)))){
                    return true;
                }
            }
        }
        return false;
    }

    std::size_t TotalEdgeCount(const CCircuitNet &TestNet){
        std::size_t Count = 0;

        for(std::size_t Index = 0; Index < TestNet.TraceCount(); Index++){
            CCircuitTrace Trace;
            if(TestNet.Trace(Index, Trace)){
                Count += Trace.EdgeCount();
            }
        }

        return Count;
    }

    void ExpectEdgeExists(
        const CCircuitNet &TestNet,
        const SCircuitTraceEdge &Expected, 
        bool RequiredExact = true){
        
        EXPECT_TRUE(HasEdge(TestNet, Expected, RequiredExact));
    }

    void ExpectEdgeDoesNotExist(
        const CCircuitNet &TestNet,
        const SCircuitTraceEdge &Expected){
        
        EXPECT_FALSE(HasEdge(TestNet, Expected, false));
    }
};

// Construction

TEST_F(CCircuitNetTest, DefaultConstruction){
    EXPECT_EQ(Net.Name(), "");
    EXPECT_EQ(Net.PinCount(), 0);
    EXPECT_EQ(Net.TraceCount(), 0);
    EXPECT_TRUE(Net.IsBroken());
}

// Name

TEST_F(CCircuitNetTest, SetName){
    TCircuitName Name = "TestCircuit";

    Net.Name(Name);

    EXPECT_EQ(Net.Name(), Name);
}

TEST_F(CCircuitNetTest, SetNameReturnsNewName){
    Net.Name("First");

    EXPECT_EQ(Net.Name(), "First");

    Net.Name("Second");

    EXPECT_EQ(Net.Name(), "Second");
}

TEST_F(CCircuitNetTest, SetNameEmpty){
    Net.Name("TestCircuit");
    Net.Name("");

    EXPECT_EQ(Net.Name(), "TestCircuit");
}

// Pins

TEST_F(CCircuitNetTest, PinCountInitiallyZero){
    EXPECT_EQ(Net.PinCount(), 0);
}

TEST_F(CCircuitNetTest, PinLocationOutOfBounds){
    SCircuitLocation Location{100, 100};

    EXPECT_FALSE(Net.PinLocation(0, Location));

    ExpectLocation(Location, SCircuitLocation{100, 100});
}

TEST_F(CCircuitNetTest, AddFirstPin){
    EXPECT_TRUE(Net.AddPinLocation(A));

    EXPECT_EQ(Net.PinCount(), 1);
    EXPECT_TRUE(HasPin(Net, A));
}

TEST_F(CCircuitNetTest, AddMultiplePins){
    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_TRUE(Net.AddPinLocation(B));
    EXPECT_TRUE(Net.AddPinLocation(C));

    EXPECT_EQ(Net.PinCount(), 3);

    EXPECT_TRUE(HasPin(Net, A));
    EXPECT_TRUE(HasPin(Net, B));
    EXPECT_TRUE(HasPin(Net, C));
}

TEST_F(CCircuitNetTest, AddDuplicatePin){
    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_FALSE(Net.AddPinLocation(A));

    EXPECT_EQ(Net.PinCount(), 1);
    EXPECT_TRUE(HasPin(Net, A));
}

TEST_F(CCircuitNetTest, PinLocationsAreNotDuplicated){
    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_TRUE(Net.AddPinLocation(B));
    EXPECT_FALSE(Net.AddPinLocation(A));
    EXPECT_FALSE(Net.AddPinLocation(B));

    EXPECT_EQ(Net.PinCount(), 2);
}

TEST_F(CCircuitNetTest, PinRetrievalDoesNotRequireInsertionOrder){
    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_TRUE(Net.AddPinLocation(B));
    EXPECT_TRUE(Net.AddPinLocation(C));

    EXPECT_EQ(Net.PinCount(), 3);

    EXPECT_TRUE(HasPin(Net, A));
    EXPECT_TRUE(HasPin(Net, B));
    EXPECT_TRUE(HasPin(Net, C));
}

// Traces

TEST_F(CCircuitNetTest, TraceCountInitiallyZero){
    EXPECT_EQ(Net.TraceCount(), 0);
}

TEST_F(CCircuitNetTest, TraceOutOfBounds){
    CCircuitTrace Trace;
    Trace.Name(Net.Name());

    EXPECT_FALSE(Net.Trace(0, Trace));

    EXPECT_EQ(Trace.Name(), Net.Name());
    EXPECT_EQ(Trace.EdgeCount(), 0);
}

TEST_F(CCircuitNetTest, AddFirstTrace){
    auto Trace = MakeTrace(Net.Name(), {EdgeAB});

    EXPECT_TRUE(Net.AddTrace(Trace));

    EXPECT_EQ(Net.TraceCount(), 1);
    EXPECT_EQ(TotalEdgeCount(Net), 1);

    EXPECT_TRUE(HasEdge(Net, EdgeAB));
}

TEST_F(CCircuitNetTest, AddMultipleTraces){
    auto Trace1 = MakeTrace(Net.Name(), {EdgeAB});
    auto Trace2 = MakeTrace(Net.Name(), {EdgeCD});

    EXPECT_TRUE(Net.AddTrace(Trace1));
    EXPECT_TRUE(Net.AddTrace(Trace2));

    EXPECT_EQ(Net.TraceCount(), 2);
    EXPECT_EQ(TotalEdgeCount(Net), 2);

    EXPECT_TRUE(HasEdge(Net, EdgeAB));
    EXPECT_TRUE(HasEdge(Net, EdgeCD));
}

TEST_F(CCircuitNetTest, TraceNameMustMatchNetName){
    Net.Name("TestCircuit");

    auto Trace = MakeTrace("DifferentCircuit", {EdgeAB});

    EXPECT_FALSE(Net.AddTrace(Trace));

    EXPECT_EQ(Net.TraceCount(), 0);
}

TEST_F(CCircuitNetTest, TraceNameMustMatchEmptyNetName){
    auto Trace = MakeTrace("TestCircuit", {EdgeAB});

    EXPECT_FALSE(Net.AddTrace(Trace));

    EXPECT_EQ(Net.TraceCount(), 0);
}

TEST_F(CCircuitNetTest, AddDuplicateTrace){
    auto Trace = MakeTrace(Net.Name(), {EdgeAB, EdgeBC});

    EXPECT_TRUE(Net.AddTrace(Trace));
    EXPECT_FALSE(Net.AddTrace(Trace));

    EXPECT_EQ(Net.TraceCount(), 1);
    EXPECT_EQ(TotalEdgeCount(Net), 2);
}

TEST_F(CCircuitNetTest, AddTraceWithDifferentEdges){
    auto Trace1 = MakeTrace(Net.Name(), {EdgeAB});
    auto Trace2 = MakeTrace(Net.Name(), {EdgeBC});

    EXPECT_TRUE(Net.AddTrace(Trace1));
    EXPECT_TRUE(Net.AddTrace(Trace2));

    EXPECT_EQ(Net.TraceCount(), 2);
}

TEST_F(CCircuitNetTest, AddTraceWithDuplicateEdgesFromAnotherTrace){
    auto Trace1 = MakeTrace(Net.Name(), {EdgeAB});
    auto Trace2 = MakeTrace(Net.Name(), {EdgeAB, EdgeBC});

    EXPECT_TRUE(Net.AddTrace(Trace1));
    EXPECT_TRUE(Net.AddTrace(Trace2));

    EXPECT_EQ(Net.TraceCount(), 2);
    EXPECT_EQ(TotalEdgeCount(Net), 3);

    EXPECT_TRUE(HasEdge(Net, EdgeAB));
    EXPECT_TRUE(HasEdge(Net, EdgeBC));
}

TEST_F(CCircuitNetTest, AddBrokenTrace){
    SCircuitTraceEdge EdgeDE{D, E};

    auto Trace = MakeTrace(Net.Name(), {EdgeAB, EdgeDE});

    EXPECT_TRUE(Trace.IsBroken());
    EXPECT_TRUE(Net.AddTrace(Trace));

    EXPECT_EQ(Net.TraceCount(), 1);
    EXPECT_EQ(TotalEdgeCount(Net), 2);
}

TEST_F(CCircuitNetTest, TraceRetrieval){
    auto Trace1 = MakeTrace(Net.Name(), {EdgeAB});
    auto Trace2 = MakeTrace(Net.Name(), {EdgeCD});

    EXPECT_TRUE(Net.AddTrace(Trace1));
    EXPECT_TRUE(Net.AddTrace(Trace2));

    EXPECT_EQ(Net.TraceCount(), 2);

    bool FoundAB = false;
    bool FoundCD = false;

    for(std::size_t Index = 0; Index < Net.TraceCount(); Index++){
        CCircuitTrace Trace;
        ASSERT_TRUE(Net.Trace(Index, Trace));

        EXPECT_EQ(Trace.Name(), Net.Name());

        for(std::size_t EdgeIndex = 0; EdgeIndex < Trace.EdgeCount(); EdgeIndex++){
            SCircuitTraceEdge Edge;
            ASSERT_TRUE(Trace.Edge(EdgeIndex, Edge));

            if(SameEdge(Edge, EdgeAB)){
                FoundAB = true;
            }

            if(SameEdge(Edge, EdgeCD)){
                FoundCD = true;
            }
        }
    }

    EXPECT_TRUE(FoundAB);
    EXPECT_TRUE(FoundCD);
}

// Broken State

TEST_F(CCircuitNetTest, NetWithoutTracesIsBroken){
    EXPECT_TRUE(Net.IsBroken());
}

TEST_F(CCircuitNetTest, NetWithPinsButNoTracesIsBroken){
    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_TRUE(Net.AddPinLocation(B));

    EXPECT_TRUE(Net.IsBroken());
}

TEST_F(CCircuitNetTest, NetWithOneConnectedTraceIsNotBroken){
    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_TRUE(Net.AddPinLocation(B));

    auto Trace = MakeTrace(Net.Name(), {EdgeAB});

    EXPECT_TRUE(Net.AddTrace(Trace));

    EXPECT_FALSE(Net.IsBroken());
}

TEST_F(CCircuitNetTest, NetWithUnconnectedPinIsBroken){
    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_TRUE(Net.AddPinLocation(B));
    EXPECT_TRUE(Net.AddPinLocation(C));

    auto Trace = MakeTrace(Net.Name(), {EdgeAB});

    EXPECT_TRUE(Net.AddTrace(Trace));

    EXPECT_TRUE(Net.IsBroken());
}

TEST_F(CCircuitNetTest, NetWithConnectedMultipleTracesIsNotBroken){
    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_TRUE(Net.AddPinLocation(B));
    EXPECT_TRUE(Net.AddPinLocation(C));

    auto Trace1 = MakeTrace(Net.Name(), {EdgeAB});
    auto Trace2 = MakeTrace(Net.Name(), {EdgeBC});

    EXPECT_TRUE(Net.AddTrace(Trace1));
    EXPECT_TRUE(Net.AddTrace(Trace2));

    EXPECT_FALSE(Net.IsBroken());
}

TEST_F(CCircuitNetTest, DisconnectedTracesWithoutAdditionalPinsAreNotBroken){
    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_TRUE(Net.AddPinLocation(B));

    SCircuitTraceEdge EdgeDE{D, E};

    auto Trace1 = MakeTrace(Net.Name(), {EdgeAB});
    auto Trace2 = MakeTrace(Net.Name(), {EdgeDE});

    EXPECT_TRUE(Net.AddTrace(Trace1));
    EXPECT_TRUE(Net.AddTrace(Trace2));

    EXPECT_FALSE(Net.IsBroken());
}

TEST_F(CCircuitNetTest, DisconnectedPinGroupsAreBroken){
    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_TRUE(Net.AddPinLocation(B));
    EXPECT_TRUE(Net.AddPinLocation(C));
    EXPECT_TRUE(Net.AddPinLocation(D));

    SCircuitTraceEdge EdgeCD{C, D};

    auto Trace1 = MakeTrace(Net.Name(), {EdgeAB});
    auto Trace2 = MakeTrace(Net.Name(), {EdgeCD});

    EXPECT_TRUE(Net.AddTrace(Trace1));
    EXPECT_TRUE(Net.AddTrace(Trace2));

    EXPECT_TRUE(Net.IsBroken());
}

TEST_F(CCircuitNetTest, ConnectedPinsThroughIntermediateJunctionAreNotBroken){
    SCircuitLocation X{15, 15};

    SCircuitTraceEdge EdgeAX{A, X};
    SCircuitTraceEdge EdgeXB{X, B};

    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_TRUE(Net.AddPinLocation(B));

    auto Trace1 = MakeTrace(Net.Name(), {EdgeAX});
    auto Trace2 = MakeTrace(Net.Name(), {EdgeXB});

    EXPECT_TRUE(Net.AddTrace(Trace1));
    EXPECT_TRUE(Net.AddTrace(Trace2));

    EXPECT_FALSE(Net.IsBroken());
}

TEST_F(CCircuitNetTest, BrokenIndividualTraceMakesNetBroken){
    SCircuitTraceEdge EdgeDE{D, E};

    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_TRUE(Net.AddPinLocation(B));

    auto Trace = MakeTrace(Net.Name(), {EdgeAB, EdgeDE});

    EXPECT_TRUE(Trace.IsBroken());
    EXPECT_TRUE(Net.AddTrace(Trace));

    EXPECT_TRUE(Net.IsBroken());
}

// Fix

TEST_F(CCircuitNetTest, FixAlreadyValidNet){
    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_TRUE(Net.AddPinLocation(B));

    auto Trace = MakeTrace(Net.Name(), {EdgeAB});

    EXPECT_TRUE(Net.AddTrace(Trace));

    EXPECT_FALSE(Net.IsBroken());
    EXPECT_TRUE(Net.Fix());
    EXPECT_FALSE(Net.IsBroken());
}

TEST_F(CCircuitNetTest, FixBrokenIndividualTrace){
    SCircuitLocation F{50, 50};
    SCircuitLocation G{60, 60};

    SCircuitTraceEdge EdgeFG{F, G};

    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_TRUE(Net.AddPinLocation(G));

    auto Trace = MakeTrace(Net.Name(), {EdgeAB, EdgeFG});

    EXPECT_TRUE(Net.AddTrace(Trace));
    EXPECT_TRUE(Net.IsBroken());

    EXPECT_TRUE(Net.Fix());
    EXPECT_FALSE(Net.IsBroken());
}

TEST_F(CCircuitNetTest, FixDoesNotRouteBetweenSeparateTraces){
    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_TRUE(Net.AddPinLocation(C));

    auto Trace1 = MakeTrace(Net.Name(), {EdgeAB});
    auto Trace2 = MakeTrace(Net.Name(), {EdgeCD});

    EXPECT_TRUE(Net.AddTrace(Trace1));
    EXPECT_TRUE(Net.AddTrace(Trace2));

    EXPECT_TRUE(Net.IsBroken());

    EXPECT_FALSE(Net.Fix());

    EXPECT_TRUE(Net.IsBroken());
}

TEST_F(CCircuitNetTest, FixRepairsIndividualTracesButDoesNotConnectPinGroups){
    SCircuitLocation F{50, 50};
    SCircuitLocation G{60, 60};
    SCircuitLocation H{70, 70};
    SCircuitLocation I{80, 80};

    SCircuitTraceEdge EdgeFG{F, G};
    SCircuitTraceEdge EdgeHI{H, I};

    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_TRUE(Net.AddPinLocation(C));
    EXPECT_TRUE(Net.AddPinLocation(F));
    EXPECT_TRUE(Net.AddPinLocation(I));

    auto Trace1 = MakeTrace(Net.Name(), {EdgeAB});
    auto Trace2 = MakeTrace(Net.Name(), {EdgeCD});
    auto Trace3 = MakeTrace(Net.Name(), {EdgeFG, EdgeHI});

    EXPECT_TRUE(Net.AddTrace(Trace1));
    EXPECT_TRUE(Net.AddTrace(Trace2));
    EXPECT_TRUE(Net.AddTrace(Trace3));

    EXPECT_TRUE(Net.IsBroken());

    // Individual traces should be repaired.
    // The separate pin groups should remain disconnected.
    EXPECT_FALSE(Net.Fix());

    EXPECT_TRUE(Net.IsBroken());

    for(std::size_t Index = 0; Index < Net.TraceCount(); Index++){
        CCircuitTrace Trace;
        ASSERT_TRUE(Net.Trace(Index, Trace));

        EXPECT_FALSE(Trace.IsBroken());
    }
}

// Simplify

TEST_F(CCircuitNetTest, SimplifyEmptyNet){
    EXPECT_FALSE(Net.SimplifyTraces());

    EXPECT_EQ(Net.TraceCount(), 0);
}

TEST_F(CCircuitNetTest, SimplifySingleTrace){
    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_TRUE(Net.AddPinLocation(D));

    auto Trace = MakeTrace(Net.Name(), {EdgeAB, EdgeBC, EdgeCD});

    EXPECT_TRUE(Net.AddTrace(Trace));

    EXPECT_TRUE(Net.SimplifyTraces());

    EXPECT_EQ(Net.TraceCount(), 1);
    EXPECT_EQ(TotalEdgeCount(Net), 1);

    ExpectEdgeExists(Net, SCircuitTraceEdge{A, D});
}

TEST_F(CCircuitNetTest, SimplifyPreservesSeparateTraces){
    SCircuitLocation F{50, 50};
    SCircuitLocation G{60, 60};

    SCircuitTraceEdge EdgeFG{F, G};

    auto Trace1 = MakeTrace(Net.Name(), {EdgeAB});
    auto Trace2 = MakeTrace(Net.Name(), {EdgeFG});

    EXPECT_TRUE(Net.AddTrace(Trace1));
    EXPECT_TRUE(Net.AddTrace(Trace2));

    EXPECT_TRUE(Net.SimplifyTraces());

    EXPECT_EQ(Net.TraceCount(), 2);
}

TEST_F(CCircuitNetTest, SimplifySplitsIntersectingTraces){
    SCircuitLocation X{10, 10};
    SCircuitLocation TopLeft{0, 20};
    SCircuitLocation BottomRight{20, 0};

    SCircuitTraceEdge EdgeAC{A, C};
    SCircuitTraceEdge EdgeBD{TopLeft, BottomRight};

    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_TRUE(Net.AddPinLocation(C));
    EXPECT_TRUE(Net.AddPinLocation(TopLeft));
    EXPECT_TRUE(Net.AddPinLocation(BottomRight));

    auto Trace1 = MakeTrace(Net.Name(), {EdgeAC});
    auto Trace2 = MakeTrace(Net.Name(), {EdgeBD});

    EXPECT_TRUE(Net.AddTrace(Trace1));
    EXPECT_TRUE(Net.AddTrace(Trace2));

    EXPECT_TRUE(Net.SimplifyTraces());

    EXPECT_EQ(Net.TraceCount(), 4);
    EXPECT_EQ(TotalEdgeCount(Net), 4);

    ExpectEdgeExists(Net, SCircuitTraceEdge{A, X}, false);
    ExpectEdgeExists(Net, SCircuitTraceEdge{X, C}, false);
    ExpectEdgeExists(Net, SCircuitTraceEdge{TopLeft, X}, false);
    ExpectEdgeExists(Net, SCircuitTraceEdge{X, BottomRight}, false);
}

TEST_F(CCircuitNetTest, SimplifyMergesTracesAtNonPinJunction){
    SCircuitLocation X{15, 15};

    SCircuitTraceEdge EdgeAX{A, X};
    SCircuitTraceEdge EdgeXB{X, B};

    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_TRUE(Net.AddPinLocation(B));

    auto Trace1 = MakeTrace(Net.Name(), {EdgeAX});
    auto Trace2 = MakeTrace(Net.Name(), {EdgeXB});

    EXPECT_TRUE(Net.AddTrace(Trace1));
    EXPECT_TRUE(Net.AddTrace(Trace2));

    EXPECT_TRUE(Net.SimplifyTraces());

    EXPECT_EQ(Net.TraceCount(), 2);
    EXPECT_EQ(TotalEdgeCount(Net), 2);

    ExpectEdgeExists(Net, SCircuitTraceEdge{A, B}, false);
}

TEST_F(CCircuitNetTest, SimplifyPreservesPinJunction){
    SCircuitLocation X{15, 15};

    SCircuitTraceEdge EdgeAX{A, X};
    SCircuitTraceEdge EdgeXB{X, B};

    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_TRUE(Net.AddPinLocation(X));
    EXPECT_TRUE(Net.AddPinLocation(B));

    auto Trace1 = MakeTrace(Net.Name(), {EdgeAX});
    auto Trace2 = MakeTrace(Net.Name(), {EdgeXB});

    EXPECT_TRUE(Net.AddTrace(Trace1));
    EXPECT_TRUE(Net.AddTrace(Trace2));

    EXPECT_TRUE(Net.SimplifyTraces());

    EXPECT_EQ(Net.TraceCount(), 2);
    EXPECT_EQ(TotalEdgeCount(Net), 2);

    ExpectEdgeExists(Net, EdgeAB, false);
    ExpectEdgeExists(Net, EdgeXB, false);
}

TEST_F(CCircuitNetTest, SimplifyOverlappingTraces){

    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_TRUE(Net.AddPinLocation(D));

    auto Trace1 = MakeTrace(Net.Name(), {EdgeAB, EdgeBC});
    auto Trace2 = MakeTrace(Net.Name(), {EdgeBC, EdgeCD});

    EXPECT_TRUE(Net.AddTrace(Trace1));
    EXPECT_TRUE(Net.AddTrace(Trace2));

    EXPECT_TRUE(Net.SimplifyTraces());

    EXPECT_EQ(Net.TraceCount(), 1);
    EXPECT_EQ(TotalEdgeCount(Net), 1);

    ExpectEdgeExists(Net, {A,D}, false);
}


TEST_F(CCircuitNetTest, SimplifySeparatedTraces){
    SCircuitLocation F{50,50};

    SCircuitTraceEdge EdgeED{E,D};
    SCircuitTraceEdge EdgeFE{F,E};
    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_TRUE(Net.AddPinLocation(F));

    auto Trace1 = MakeTrace(Net.Name(), {EdgeAB});
    auto Trace2 = MakeTrace(Net.Name(), {EdgeBC});
    auto Trace3 = MakeTrace(Net.Name(), {EdgeCD});
    auto Trace4 = MakeTrace(Net.Name(), {EdgeED});
    auto Trace5 = MakeTrace(Net.Name(), {EdgeFE});

    EXPECT_TRUE(Net.AddTrace(Trace1));
    EXPECT_TRUE(Net.AddTrace(Trace3));
    EXPECT_TRUE(Net.AddTrace(Trace5));
    EXPECT_TRUE(Net.AddTrace(Trace2));
    EXPECT_TRUE(Net.AddTrace(Trace4));

    EXPECT_TRUE(Net.SimplifyTraces());

    EXPECT_EQ(Net.TraceCount(), 1);
    EXPECT_EQ(TotalEdgeCount(Net), 1);

    ExpectEdgeExists(Net, {A,F}, false);
}

// Elaborate

TEST_F(CCircuitNetTest, ElaborateEmptyNet){
    EXPECT_FALSE(Net.ElaborateTraces());

    EXPECT_EQ(Net.TraceCount(), 0);
}

TEST_F(CCircuitNetTest, ElaborateSingleTrace){
    SCircuitLocation End{3, 0};

    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_TRUE(Net.AddPinLocation(End));

    auto Trace = MakeTrace(Net.Name(), {
        SCircuitTraceEdge{A, End}
    });

    EXPECT_TRUE(Net.AddTrace(Trace));

    EXPECT_TRUE(Net.ElaborateTraces());

    EXPECT_EQ(Net.TraceCount(), 1);
    EXPECT_EQ(TotalEdgeCount(Net), 3);

    ExpectEdgeExists(Net, SCircuitTraceEdge{{0, 0}, {1, 0}});
    ExpectEdgeExists(Net, SCircuitTraceEdge{{1, 0}, {2, 0}});
    ExpectEdgeExists(Net, SCircuitTraceEdge{{2, 0}, {3, 0}});
}

TEST_F(CCircuitNetTest, ElaborateMultipleTraces){
    SCircuitLocation HorizontalEnd{3, 0};
    SCircuitLocation VerticalEnd{0, 3};

    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_TRUE(Net.AddPinLocation(HorizontalEnd));
    EXPECT_TRUE(Net.AddPinLocation(VerticalEnd));

    auto Trace1 = MakeTrace(Net.Name(), {
        SCircuitTraceEdge{A, HorizontalEnd}
    });

    auto Trace2 = MakeTrace(Net.Name(), {
        SCircuitTraceEdge{A, VerticalEnd}
    });

    EXPECT_TRUE(Net.AddTrace(Trace1));
    EXPECT_TRUE(Net.AddTrace(Trace2));

    EXPECT_TRUE(Net.ElaborateTraces());

    EXPECT_EQ(Net.TraceCount(), 2);
    EXPECT_EQ(TotalEdgeCount(Net), 6);
}

TEST_F(CCircuitNetTest, ElaboratePreservesTraceNames){
    SCircuitLocation End{3, 0};

    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_TRUE(Net.AddPinLocation(End));

    auto Trace = MakeTrace(Net.Name(), {
        SCircuitTraceEdge{A, End}
    });

    EXPECT_TRUE(Net.AddTrace(Trace));
    EXPECT_TRUE(Net.ElaborateTraces());

    for(std::size_t Index = 0; Index < Net.TraceCount(); Index++){
        CCircuitTrace Result;
        ASSERT_TRUE(Net.Trace(Index, Result));

        EXPECT_EQ(Result.Name(), Net.Name());
    }
}

// Copy Construction

TEST_F(CCircuitNetTest, CopyConstruction){
    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_TRUE(Net.AddPinLocation(B));

    auto Trace = MakeTrace(Net.Name(), {EdgeAB});

    EXPECT_TRUE(Net.AddTrace(Trace));

    CCircuitNet Copy(Net);

    EXPECT_EQ(Copy.Name(), Net.Name());
    EXPECT_EQ(Copy.PinCount(), Net.PinCount());
    EXPECT_EQ(Copy.TraceCount(), Net.TraceCount());

    EXPECT_TRUE(HasPin(Copy, A));
    EXPECT_TRUE(HasPin(Copy, B));
    EXPECT_TRUE(HasEdge(Copy, EdgeAB));

    EXPECT_EQ(Copy.IsBroken(), Net.IsBroken());
}

TEST_F(CCircuitNetTest, CopyConstructionCreatesIndependentCopy){
    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_TRUE(Net.AddPinLocation(B));

    auto Trace = MakeTrace(Net.Name(), {EdgeAB});

    EXPECT_TRUE(Net.AddTrace(Trace));

    CCircuitNet Copy(Net);

    EXPECT_TRUE(Copy.AddPinLocation(C));

    EXPECT_EQ(Copy.PinCount(), 3);
    EXPECT_EQ(Net.PinCount(), 2);

    EXPECT_TRUE(HasPin(Copy, C));
    EXPECT_FALSE(HasPin(Net, C));
}

TEST_F(CCircuitNetTest, CopyConstructionPreservesMultipleTraces){
    auto Trace1 = MakeTrace(Net.Name(), {EdgeAB});
    auto Trace2 = MakeTrace(Net.Name(), {EdgeCD});

    EXPECT_TRUE(Net.AddTrace(Trace1));
    EXPECT_TRUE(Net.AddTrace(Trace2));

    CCircuitNet Copy(Net);

    EXPECT_EQ(Copy.TraceCount(), 2);
    EXPECT_EQ(TotalEdgeCount(Copy), 2);

    EXPECT_TRUE(HasEdge(Copy, EdgeAB));
    EXPECT_TRUE(HasEdge(Copy, EdgeCD));
}

// Copy Assignment

TEST_F(CCircuitNetTest, CopyAssignment){
    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_TRUE(Net.AddPinLocation(B));

    auto Trace = MakeTrace(Net.Name(), {EdgeAB});

    EXPECT_TRUE(Net.AddTrace(Trace));

    CCircuitNet Copy;
    Copy = Net;

    EXPECT_EQ(Copy.Name(), Net.Name());
    EXPECT_EQ(Copy.PinCount(), Net.PinCount());
    EXPECT_EQ(Copy.TraceCount(), Net.TraceCount());

    EXPECT_TRUE(HasPin(Copy, A));
    EXPECT_TRUE(HasPin(Copy, B));
    EXPECT_TRUE(HasEdge(Copy, EdgeAB));
}

TEST_F(CCircuitNetTest, CopyAssignmentCreatesIndependentCopy){
    EXPECT_TRUE(Net.AddPinLocation(A));

    CCircuitNet Copy;
    Copy = Net;

    EXPECT_TRUE(Copy.AddPinLocation(B));

    EXPECT_EQ(Copy.PinCount(), 2);
    EXPECT_EQ(Net.PinCount(), 1);

    EXPECT_FALSE(HasPin(Net, B));
}

TEST_F(CCircuitNetTest, CopyAssignmentOverwritesExistingContents){
    EXPECT_TRUE(Net.AddPinLocation(A));

    auto Trace = MakeTrace(Net.Name(), {EdgeAB});
    EXPECT_TRUE(Net.AddTrace(Trace));

    CCircuitNet Copy;
    Copy.Name("DifferentName");
    EXPECT_TRUE(Copy.AddPinLocation(C));

    Copy = Net;

    EXPECT_EQ(Copy.Name(), Net.Name());
    EXPECT_EQ(Copy.PinCount(), 1);
    EXPECT_EQ(Copy.TraceCount(), 1);

    EXPECT_TRUE(HasPin(Copy, A));
    EXPECT_FALSE(HasPin(Copy, C));
    EXPECT_TRUE(HasEdge(Copy, EdgeAB));
}

// Move Construction

TEST_F(CCircuitNetTest, MoveConstruction){
    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_TRUE(Net.AddPinLocation(B));

    auto Trace = MakeTrace(Net.Name(), {EdgeAB});
    EXPECT_TRUE(Net.AddTrace(Trace));

    CCircuitNet Moved(std::move(Net));

    EXPECT_EQ(Moved.PinCount(), 2);
    EXPECT_EQ(Moved.TraceCount(), 1);

    EXPECT_TRUE(HasPin(Moved, A));
    EXPECT_TRUE(HasPin(Moved, B));
    EXPECT_TRUE(HasEdge(Moved, EdgeAB));
}

// Move Assignment

TEST_F(CCircuitNetTest, MoveAssignment){
    EXPECT_TRUE(Net.AddPinLocation(A));
    EXPECT_TRUE(Net.AddPinLocation(B));

    auto Trace = MakeTrace(Net.Name(), {EdgeAB});
    EXPECT_TRUE(Net.AddTrace(Trace));

    CCircuitNet Moved;
    Moved.Name("DifferentName");
    EXPECT_TRUE(Moved.AddPinLocation(C));

    Moved = std::move(Net);

    EXPECT_EQ(Moved.PinCount(), 2);
    EXPECT_EQ(Moved.TraceCount(), 1);

    EXPECT_TRUE(HasPin(Moved, A));
    EXPECT_TRUE(HasPin(Moved, B));
    EXPECT_FALSE(HasPin(Moved, C));
    EXPECT_TRUE(HasEdge(Moved, EdgeAB));
}
