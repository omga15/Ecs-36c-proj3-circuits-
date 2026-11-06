#include <gtest/gtest.h>
#include "CircuitTrace.h"

class CCircuitTraceTest : public ::testing::Test {
protected:
    CCircuitTrace Trace;

    SCircuitLocation A{0, 0};
    SCircuitLocation B{10, 10};
    SCircuitLocation C{20, 20};
    SCircuitLocation D{30, 30};

    SCircuitTraceEdge EdgeAB{A, B};
    SCircuitTraceEdge EdgeBC{B, C};
    SCircuitTraceEdge EdgeCD{C, D};

    void ExpectEdge(
        const SCircuitTraceEdge &Actual,
        const SCircuitTraceEdge &Expected){
        EXPECT_EQ(Actual.DSource, Expected.DSource);
        EXPECT_EQ(Actual.DDestination, Expected.DDestination);
    }
};

// Construction

TEST_F(CCircuitTraceTest, DefaultConstruction){
    EXPECT_EQ(Trace.Name(), "");
    EXPECT_EQ(Trace.LocationCount(), 0);
    EXPECT_EQ(Trace.EdgeCount(), 0);
}

// Name

TEST_F(CCircuitTraceTest, SetName){
    TCircuitName Name = "TestCircuit";

    Trace.Name(Name);

    EXPECT_EQ(Trace.Name(), Name);
}

TEST_F(CCircuitTraceTest, SetNameReturnsPreviousName){
    EXPECT_EQ(Trace.Name("First"), "First");
    EXPECT_EQ(Trace.Name(), "First");
    auto PreviousName = Trace.Name("Second");

    EXPECT_EQ(PreviousName, "Second");
    EXPECT_EQ(Trace.Name(), "Second");
}

TEST_F(CCircuitTraceTest, SetNameEmpty){
    Trace.Name("TestCircuit");
    Trace.Name("");

    EXPECT_EQ(Trace.Name(), "TestCircuit");
}

// Locations

TEST_F(CCircuitTraceTest, LocationCountInitiallyZero){
    EXPECT_EQ(Trace.LocationCount(), 0);
}

TEST_F(CCircuitTraceTest, LocationOutOfBounds){
    SCircuitLocation Location;
    EXPECT_FALSE(Trace.Location(0, Location));
}

// Edges

TEST_F(CCircuitTraceTest, EdgeCountInitiallyZero){
    EXPECT_EQ(Trace.EdgeCount(), 0);
}

TEST_F(CCircuitTraceTest, EdgeOutOfBounds){
    SCircuitTraceEdge Edge;
    EXPECT_FALSE(Trace.Edge(0, Edge));
}

TEST_F(CCircuitTraceTest, AddFirstEdge){
    EXPECT_TRUE(Trace.AddEdge(EdgeAB));

    EXPECT_EQ(Trace.EdgeCount(), 1);
    EXPECT_EQ(Trace.LocationCount(), 2);

    SCircuitTraceEdge Edge;
    EXPECT_TRUE(Trace.Edge(0,Edge));
    ExpectEdge(Edge, EdgeAB);

    SCircuitLocation LocationA, LocationB;
    EXPECT_TRUE(Trace.Location(0,LocationA));
    EXPECT_EQ(LocationA, A);
    EXPECT_TRUE(Trace.Location(1,LocationB));
    EXPECT_EQ(LocationB, B);
}

TEST_F(CCircuitTraceTest, AddMultipleEdges){
    EXPECT_TRUE(Trace.AddEdge(EdgeAB));
    EXPECT_TRUE(Trace.AddEdge(EdgeBC));
    EXPECT_TRUE(Trace.AddEdge(EdgeCD));

    EXPECT_EQ(Trace.EdgeCount(), 3);
    EXPECT_EQ(Trace.LocationCount(), 4);
}

TEST_F(CCircuitTraceTest, AddEdgeWithExistingLocations){
    EXPECT_TRUE(Trace.AddEdge(EdgeAB));
    EXPECT_TRUE(Trace.AddEdge(EdgeBC));

    EXPECT_EQ(Trace.LocationCount(), 3);
    EXPECT_EQ(Trace.EdgeCount(), 2);
}

TEST_F(CCircuitTraceTest, AddEdgeWithOneExistingLocation){
    EXPECT_TRUE(Trace.AddEdge(EdgeAB));

    SCircuitTraceEdge EdgeAC{A, C};

    EXPECT_TRUE(Trace.AddEdge(EdgeAC));

    EXPECT_EQ(Trace.LocationCount(), 3);
    EXPECT_EQ(Trace.EdgeCount(), 2);
}

TEST_F(CCircuitTraceTest, AddDuplicateEdge){
    EXPECT_TRUE(Trace.AddEdge(EdgeAB));
    EXPECT_FALSE(Trace.AddEdge(EdgeAB));

    EXPECT_EQ(Trace.LocationCount(), 2);
    EXPECT_EQ(Trace.EdgeCount(), 1);
}

TEST_F(CCircuitTraceTest, ReverseEdgeIsDifferent){
    EXPECT_TRUE(Trace.AddEdge(EdgeAB));

    SCircuitTraceEdge EdgeBA{B, A};

    EXPECT_FALSE(Trace.AddEdge(EdgeBA));

    EXPECT_EQ(Trace.EdgeCount(), 1);
    EXPECT_EQ(Trace.LocationCount(), 2);
}

TEST_F(CCircuitTraceTest, EdgeRetrievalPreservesInsertionOrder){
    EXPECT_TRUE(Trace.AddEdge(EdgeAB));
    EXPECT_TRUE(Trace.AddEdge(EdgeBC));
    EXPECT_TRUE(Trace.AddEdge(EdgeCD));

    SCircuitTraceEdge Edge0, Edge1, Edge2;
    EXPECT_TRUE(Trace.Edge(0,Edge0));
    EXPECT_TRUE(Trace.Edge(1,Edge1));
    EXPECT_TRUE(Trace.Edge(2,Edge2));
    ExpectEdge(Edge0, EdgeAB);
    ExpectEdge(Edge1, EdgeBC);
    ExpectEdge(Edge2, EdgeCD);
}

TEST_F(CCircuitTraceTest, LocationsAreNotDuplicated){
    SCircuitTraceEdge EdgeBA{B, A};
    EXPECT_TRUE(Trace.AddEdge(EdgeAB));
    EXPECT_FALSE(Trace.AddEdge(EdgeBA));

    EXPECT_EQ(Trace.EdgeCount(), 1);
    EXPECT_EQ(Trace.LocationCount(), 2);
}

TEST_F(CCircuitTraceTest, ConcatenateTraceEmpty){
    CCircuitTrace OtherTrace;

    EXPECT_FALSE(Trace.ConcatenateTrace(OtherTrace));
}

TEST_F(CCircuitTraceTest, ConcatenateTraceAppend){
    CCircuitTrace OtherTrace;

    EXPECT_TRUE(Trace.AddEdge(EdgeAB));
    EXPECT_TRUE(OtherTrace.AddEdge(EdgeBC));
    EXPECT_TRUE(Trace.ConcatenateTrace(OtherTrace));

    EXPECT_EQ(Trace.EdgeCount(), 2);
    EXPECT_EQ(Trace.LocationCount(), 3);
    SCircuitTraceEdge Edge0, Edge1;
    EXPECT_TRUE(Trace.Edge(0,Edge0));
    EXPECT_TRUE(Trace.Edge(1,Edge1));
    ExpectEdge(Edge0,EdgeAB);
    ExpectEdge(Edge1,EdgeBC);
}

TEST_F(CCircuitTraceTest, ConcatenateTraceAppendReverse){
    CCircuitTrace OtherTrace;
    SCircuitTraceEdge EdgeCB{C, B};

    EXPECT_TRUE(Trace.AddEdge(EdgeAB));
    EXPECT_TRUE(OtherTrace.AddEdge(EdgeCB));
    EXPECT_TRUE(Trace.ConcatenateTrace(OtherTrace));

    EXPECT_EQ(Trace.EdgeCount(), 2);
    EXPECT_EQ(Trace.LocationCount(), 3);
    SCircuitTraceEdge Edge0, Edge1;
    EXPECT_TRUE(Trace.Edge(0,Edge0));
    EXPECT_TRUE(Trace.Edge(1,Edge1));
    ExpectEdge(Edge0,EdgeAB);
    ExpectEdge(Edge1,EdgeBC);
}

TEST_F(CCircuitTraceTest, ConcatenateTracePrepend){
    CCircuitTrace OtherTrace;
    SCircuitTraceEdge EdgeBA{B, A};

    EXPECT_TRUE(Trace.AddEdge(EdgeBC));
    EXPECT_TRUE(OtherTrace.AddEdge(EdgeBA));
    EXPECT_TRUE(Trace.ConcatenateTrace(OtherTrace));

    EXPECT_EQ(Trace.EdgeCount(), 2);
    EXPECT_EQ(Trace.LocationCount(), 3);
    SCircuitTraceEdge Edge0, Edge1;
    EXPECT_TRUE(Trace.Edge(0,Edge0));
    EXPECT_TRUE(Trace.Edge(1,Edge1));
    ExpectEdge(Edge0,EdgeAB);
    ExpectEdge(Edge1,EdgeBC);
}

TEST_F(CCircuitTraceTest, ConcatenateTracePrependReverse){
    CCircuitTrace OtherTrace;

    EXPECT_TRUE(Trace.AddEdge(EdgeBC));
    EXPECT_TRUE(OtherTrace.AddEdge(EdgeAB));
    EXPECT_TRUE(Trace.ConcatenateTrace(OtherTrace));

    EXPECT_EQ(Trace.EdgeCount(), 2);
    EXPECT_EQ(Trace.LocationCount(), 3);
    SCircuitTraceEdge Edge0, Edge1;
    EXPECT_TRUE(Trace.Edge(0,Edge0));
    EXPECT_TRUE(Trace.Edge(1,Edge1));
    ExpectEdge(Edge0,EdgeAB);
    ExpectEdge(Edge1,EdgeBC);
}

TEST_F(CCircuitTraceTest, ConcatenateTraceDisjoint){
    CCircuitTrace OtherTrace;

    EXPECT_TRUE(Trace.AddEdge(EdgeAB));
    EXPECT_TRUE(OtherTrace.AddEdge(EdgeCD));
    EXPECT_FALSE(Trace.ConcatenateTrace(OtherTrace));

    EXPECT_EQ(Trace.EdgeCount(), 1);
    EXPECT_EQ(Trace.LocationCount(), 2);
    SCircuitTraceEdge Edge0;
    EXPECT_TRUE(Trace.Edge(0,Edge0));
    ExpectEdge(Edge0,EdgeAB);
}

TEST_F(CCircuitTraceTest, UnbrokenTrace){
    EXPECT_TRUE(Trace.AddEdge(EdgeAB));
    EXPECT_TRUE(Trace.AddEdge(EdgeBC));
    EXPECT_TRUE(Trace.AddEdge(EdgeCD));

    SCircuitTraceEdge Edge0, Edge1, Edge2;
    EXPECT_TRUE(Trace.Edge(0,Edge0));
    EXPECT_TRUE(Trace.Edge(1,Edge1));
    EXPECT_TRUE(Trace.Edge(2,Edge2));
    ExpectEdge(Edge0, EdgeAB);
    ExpectEdge(Edge1, EdgeBC);
    ExpectEdge(Edge2, EdgeCD);
    EXPECT_FALSE(Trace.IsBroken());
}

TEST_F(CCircuitTraceTest, BrokenTrace){
    EXPECT_TRUE(Trace.AddEdge(EdgeAB));
    EXPECT_TRUE(Trace.AddEdge(EdgeCD));

    SCircuitTraceEdge Edge0, Edge1;
    EXPECT_TRUE(Trace.Edge(0,Edge0));
    EXPECT_TRUE(Trace.Edge(1,Edge1));
    ExpectEdge(Edge0, EdgeAB);
    ExpectEdge(Edge1, EdgeCD);
    EXPECT_TRUE(Trace.IsBroken());
}

TEST_F(CCircuitTraceTest, FixBrokenTrace){
    SCircuitLocation E{40, 50};
    SCircuitLocation F{50, 50};
    SCircuitLocation G{60, 70};
    SCircuitLocation H{70, 70};
    SCircuitLocation DME{40, 40};
    SCircuitLocation FMG{60, 60};
    SCircuitTraceEdge EdgeEF{E,F}, EdgeGH{G,H}, EdgeDM{D,DME}, EdgeME{DME,E}, EdgeFM{F,FMG}, EdgeMG{FMG,G};
    EXPECT_TRUE(Trace.AddEdge(EdgeAB));
    EXPECT_TRUE(Trace.AddEdge(EdgeCD));
    EXPECT_TRUE(Trace.AddEdge(EdgeEF));
    EXPECT_TRUE(Trace.AddEdge(EdgeGH));

    SCircuitTraceEdge Edge0, Edge1, Edge2, Edge3;
    EXPECT_TRUE(Trace.Edge(0,Edge0));
    EXPECT_TRUE(Trace.Edge(1,Edge1));
    EXPECT_TRUE(Trace.Edge(2,Edge2));
    EXPECT_TRUE(Trace.Edge(3,Edge3));
    ExpectEdge(Edge0, EdgeAB);
    ExpectEdge(Edge1, EdgeCD);
    ExpectEdge(Edge2, EdgeEF);
    ExpectEdge(Edge3, EdgeGH);
    EXPECT_TRUE(Trace.IsBroken());
    EXPECT_TRUE(Trace.Fix());
    EXPECT_FALSE(Trace.IsBroken());
    EXPECT_EQ(Trace.EdgeCount(),9);
    SCircuitTraceEdge Edge4, Edge5, Edge6, Edge7, Edge8;
    EXPECT_TRUE(Trace.Edge(0,Edge0));
    EXPECT_TRUE(Trace.Edge(1,Edge1));
    EXPECT_TRUE(Trace.Edge(2,Edge2));
    EXPECT_TRUE(Trace.Edge(3,Edge3));
    EXPECT_TRUE(Trace.Edge(4,Edge4));
    EXPECT_TRUE(Trace.Edge(5,Edge5));
    EXPECT_TRUE(Trace.Edge(6,Edge6));
    EXPECT_TRUE(Trace.Edge(7,Edge7));
    EXPECT_TRUE(Trace.Edge(8,Edge8));
    ExpectEdge(Edge0, EdgeAB);
    ExpectEdge(Edge1, EdgeBC);
    ExpectEdge(Edge2, EdgeCD);
    ExpectEdge(Edge3, EdgeDM);
    ExpectEdge(Edge4, EdgeME);
    ExpectEdge(Edge5, EdgeEF);
    ExpectEdge(Edge6, EdgeFM);
    ExpectEdge(Edge7, EdgeMG);
    ExpectEdge(Edge8, EdgeGH);
}

// Simplify

TEST_F(CCircuitTraceTest, SimplifyStraightLine){
    EXPECT_TRUE(Trace.AddEdge(EdgeAB));
    EXPECT_TRUE(Trace.AddEdge(EdgeBC));
    EXPECT_TRUE(Trace.AddEdge(EdgeCD));

    auto SimplifiedTrace = Trace.Simplify();

    EXPECT_EQ(SimplifiedTrace.EdgeCount(), 1);
    SCircuitTraceEdge Edge0;
    EXPECT_TRUE(SimplifiedTrace.Edge(0,Edge0));
    ExpectEdge(Edge0, SCircuitTraceEdge{A, D});
}

TEST_F(CCircuitTraceTest, SimplifyPreservesTurns){
    SCircuitLocation E{30, 20};

    EXPECT_TRUE(Trace.AddEdge(EdgeAB));
    EXPECT_TRUE(Trace.AddEdge(EdgeBC));
    EXPECT_TRUE(Trace.AddEdge(SCircuitTraceEdge{C, E}));

    auto SimplifiedTrace = Trace.Simplify();

    EXPECT_EQ(SimplifiedTrace.EdgeCount(), 2);
    SCircuitTraceEdge Edge0, Edge1;
    EXPECT_TRUE(SimplifiedTrace.Edge(0, Edge0));
    EXPECT_TRUE(SimplifiedTrace.Edge(1, Edge1));
    ExpectEdge(Edge0, SCircuitTraceEdge{A, C});
    ExpectEdge(Edge1, SCircuitTraceEdge{C, E});
}

TEST_F(CCircuitTraceTest, SimplifyAlreadySimplified){
    EXPECT_TRUE(Trace.AddEdge(SCircuitTraceEdge{A, D}));

    auto SimplifiedTrace = Trace.Simplify();

    EXPECT_EQ(SimplifiedTrace.EdgeCount(), 1);
    SCircuitTraceEdge Edge0;
    EXPECT_TRUE(SimplifiedTrace.Edge(0, Edge0));
    ExpectEdge(Edge0, SCircuitTraceEdge{A, D});
}

TEST_F(CCircuitTraceTest, SimplifySingleEdge){
    EXPECT_TRUE(Trace.AddEdge(EdgeAB));

    auto SimplifiedTrace = Trace.Simplify();

    EXPECT_EQ(SimplifiedTrace.EdgeCount(), 1);
    SCircuitTraceEdge Edge0;
    EXPECT_TRUE(SimplifiedTrace.Edge(0, Edge0));
    ExpectEdge(Edge0, EdgeAB);
}

TEST_F(CCircuitTraceTest, SimplifyEmptyTrace){
    auto SimplifiedTrace = Trace.Simplify();

    EXPECT_EQ(SimplifiedTrace.EdgeCount(), 0);
    EXPECT_EQ(SimplifiedTrace.LocationCount(), 0);
}


// Elaborate

TEST_F(CCircuitTraceTest, ElaborateHorizontalEdge){
    SCircuitLocation End{3, 0};

    EXPECT_TRUE(Trace.AddEdge(SCircuitTraceEdge{{0, 0}, End}));

    auto ElaboratedTrace = Trace.Elaborate();

    EXPECT_EQ(ElaboratedTrace.EdgeCount(), 3);
    SCircuitTraceEdge Edge0, Edge1, Edge2;
    EXPECT_TRUE(ElaboratedTrace.Edge(0, Edge0));
    EXPECT_TRUE(ElaboratedTrace.Edge(1, Edge1));
    EXPECT_TRUE(ElaboratedTrace.Edge(2, Edge2));
    ExpectEdge(Edge0, SCircuitTraceEdge{{0, 0}, {1, 0}});
    ExpectEdge(Edge1, SCircuitTraceEdge{{1, 0}, {2, 0}});
    ExpectEdge(Edge2, SCircuitTraceEdge{{2, 0}, {3, 0}});
}

TEST_F(CCircuitTraceTest, ElaborateVerticalEdge){
    SCircuitLocation End{0, 3};

    EXPECT_TRUE(Trace.AddEdge(SCircuitTraceEdge{{0, 0}, End}));

    auto ElaboratedTrace = Trace.Elaborate();

    EXPECT_EQ(ElaboratedTrace.EdgeCount(), 3);
    SCircuitTraceEdge Edge0, Edge1, Edge2;
    EXPECT_TRUE(ElaboratedTrace.Edge(0, Edge0));
    EXPECT_TRUE(ElaboratedTrace.Edge(1, Edge1));
    EXPECT_TRUE(ElaboratedTrace.Edge(2, Edge2));
    ExpectEdge(Edge0, SCircuitTraceEdge{{0, 0}, {0, 1}});
    ExpectEdge(Edge1, SCircuitTraceEdge{{0, 1}, {0, 2}});
    ExpectEdge(Edge2, SCircuitTraceEdge{{0, 2}, {0, 3}});
}

TEST_F(CCircuitTraceTest, ElaborateDiagonalEdge){
    SCircuitLocation Start{0, 0};
    SCircuitLocation End{3, 3};

    EXPECT_TRUE(Trace.AddEdge(SCircuitTraceEdge{Start, End}));

    auto ElaboratedTrace = Trace.Elaborate();

    EXPECT_EQ(ElaboratedTrace.EdgeCount(), 3);
    SCircuitTraceEdge Edge0, Edge1, Edge2;
    EXPECT_TRUE(ElaboratedTrace.Edge(0, Edge0));
    EXPECT_TRUE(ElaboratedTrace.Edge(1, Edge1));
    EXPECT_TRUE(ElaboratedTrace.Edge(2, Edge2));
    ExpectEdge(Edge0, SCircuitTraceEdge{{0, 0}, {1, 1}});
    ExpectEdge(Edge1, SCircuitTraceEdge{{1, 1}, {2, 2}});
    ExpectEdge(Edge2, SCircuitTraceEdge{{2, 2}, {3, 3}});
}

TEST_F(CCircuitTraceTest, ElaborateReverseDiagonalEdge){
    SCircuitLocation Start{3, 3};
    SCircuitLocation End{0, 0};

    EXPECT_TRUE(Trace.AddEdge(SCircuitTraceEdge{Start, End}));

    auto ElaboratedTrace = Trace.Elaborate();

    EXPECT_EQ(ElaboratedTrace.EdgeCount(), 3);
    SCircuitTraceEdge Edge0, Edge1, Edge2;
    EXPECT_TRUE(ElaboratedTrace.Edge(0, Edge0));
    EXPECT_TRUE(ElaboratedTrace.Edge(1, Edge1));
    EXPECT_TRUE(ElaboratedTrace.Edge(2, Edge2));
    ExpectEdge(Edge0, SCircuitTraceEdge{{3, 3}, {2, 2}});
    ExpectEdge(Edge1, SCircuitTraceEdge{{2, 2}, {1, 1}});
    ExpectEdge(Edge2, SCircuitTraceEdge{{1, 1}, {0, 0}});
}