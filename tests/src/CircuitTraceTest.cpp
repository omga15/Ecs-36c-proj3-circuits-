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
        const SCircuitTraceEdge &Expected) {
        EXPECT_EQ(Actual.DSource, Expected.DSource);
        EXPECT_EQ(Actual.DDestination, Expected.DDestination);
    }
};

// Construction

TEST_F(CCircuitTraceTest, DefaultConstruction) {
    EXPECT_EQ(Trace.Name(), "");
    EXPECT_EQ(Trace.LocationCount(), 0);
    EXPECT_EQ(Trace.EdgeCount(), 0);
}

// Name

TEST_F(CCircuitTraceTest, SetName) {
    TCircuitName Name = "TestCircuit";

    Trace.Name(Name);

    EXPECT_EQ(Trace.Name(), Name);
}

TEST_F(CCircuitTraceTest, SetNameReturnsPreviousName) {
    EXPECT_EQ(Trace.Name("First"), "First");
    EXPECT_EQ(Trace.Name(), "First");
    auto PreviousName = Trace.Name("Second");

    EXPECT_EQ(PreviousName, "Second");
    EXPECT_EQ(Trace.Name(), "Second");
}

TEST_F(CCircuitTraceTest, SetNameEmpty) {
    Trace.Name("TestCircuit");
    Trace.Name("");

    EXPECT_EQ(Trace.Name(), "TestCircuit");
}

// Locations

TEST_F(CCircuitTraceTest, LocationCountInitiallyZero) {
    EXPECT_EQ(Trace.LocationCount(), 0);
}

TEST_F(CCircuitTraceTest, LocationOutOfBounds) {
    EXPECT_EQ(Trace.Location(0), SCircuitLocation());
}

// Edges

TEST_F(CCircuitTraceTest, EdgeCountInitiallyZero) {
    EXPECT_EQ(Trace.EdgeCount(), 0);
}

TEST_F(CCircuitTraceTest, EdgeOutOfBounds) {
    EXPECT_EQ(Trace.Edge(0), SCircuitTraceEdge());
}

TEST_F(CCircuitTraceTest, AddFirstEdge) {
    EXPECT_TRUE(Trace.AddEdge(EdgeAB));

    EXPECT_EQ(Trace.EdgeCount(), 1);
    EXPECT_EQ(Trace.LocationCount(), 2);

    ExpectEdge(Trace.Edge(0), EdgeAB);

    EXPECT_EQ(Trace.Location(0), A);
    EXPECT_EQ(Trace.Location(1), B);
}

TEST_F(CCircuitTraceTest, AddMultipleEdges) {
    EXPECT_TRUE(Trace.AddEdge(EdgeAB));
    EXPECT_TRUE(Trace.AddEdge(EdgeBC));
    EXPECT_TRUE(Trace.AddEdge(EdgeCD));

    EXPECT_EQ(Trace.EdgeCount(), 3);
    EXPECT_EQ(Trace.LocationCount(), 4);
}

TEST_F(CCircuitTraceTest, AddEdgeWithExistingLocations) {
    EXPECT_TRUE(Trace.AddEdge(EdgeAB));
    EXPECT_TRUE(Trace.AddEdge(EdgeBC));

    EXPECT_EQ(Trace.LocationCount(), 3);
    EXPECT_EQ(Trace.EdgeCount(), 2);
}

TEST_F(CCircuitTraceTest, AddEdgeWithOneExistingLocation) {
    EXPECT_TRUE(Trace.AddEdge(EdgeAB));

    SCircuitTraceEdge EdgeAC{A, C};

    EXPECT_TRUE(Trace.AddEdge(EdgeAC));

    EXPECT_EQ(Trace.LocationCount(), 3);
    EXPECT_EQ(Trace.EdgeCount(), 2);
}

TEST_F(CCircuitTraceTest, AddDuplicateEdge) {
    EXPECT_TRUE(Trace.AddEdge(EdgeAB));
    EXPECT_FALSE(Trace.AddEdge(EdgeAB));

    EXPECT_EQ(Trace.LocationCount(), 2);
    EXPECT_EQ(Trace.EdgeCount(), 1);
}

TEST_F(CCircuitTraceTest, ReverseEdgeIsDifferent) {
    EXPECT_TRUE(Trace.AddEdge(EdgeAB));

    SCircuitTraceEdge EdgeBA{B, A};

    EXPECT_FALSE(Trace.AddEdge(EdgeBA));

    EXPECT_EQ(Trace.EdgeCount(), 1);
    EXPECT_EQ(Trace.LocationCount(), 2);
}

TEST_F(CCircuitTraceTest, EdgeRetrievalPreservesInsertionOrder) {
    EXPECT_TRUE(Trace.AddEdge(EdgeAB));
    EXPECT_TRUE(Trace.AddEdge(EdgeBC));
    EXPECT_TRUE(Trace.AddEdge(EdgeCD));

    ExpectEdge(Trace.Edge(0), EdgeAB);
    ExpectEdge(Trace.Edge(1), EdgeBC);
    ExpectEdge(Trace.Edge(2), EdgeCD);
}

TEST_F(CCircuitTraceTest, LocationsAreNotDuplicated) {
    SCircuitTraceEdge EdgeBA{B, A};
    EXPECT_TRUE(Trace.AddEdge(EdgeAB));
    EXPECT_FALSE(Trace.AddEdge(EdgeBA));

    EXPECT_EQ(Trace.EdgeCount(), 1);
    EXPECT_EQ(Trace.LocationCount(), 2);
}

// Simplify

TEST_F(CCircuitTraceTest, SimplifyStraightLine) {
    EXPECT_TRUE(Trace.AddEdge(EdgeAB));
    EXPECT_TRUE(Trace.AddEdge(EdgeBC));
    EXPECT_TRUE(Trace.AddEdge(EdgeCD));

    auto SimplifiedTrace = Trace.Simplify();

    EXPECT_EQ(SimplifiedTrace.EdgeCount(), 1);
    ExpectEdge(SimplifiedTrace.Edge(0), SCircuitTraceEdge{A, D});
}

TEST_F(CCircuitTraceTest, SimplifyPreservesTurns) {
    SCircuitLocation E{30, 20};

    EXPECT_TRUE(Trace.AddEdge(EdgeAB));
    EXPECT_TRUE(Trace.AddEdge(EdgeBC));
    EXPECT_TRUE(Trace.AddEdge(SCircuitTraceEdge{C, E}));

    auto SimplifiedTrace = Trace.Simplify();

    EXPECT_EQ(SimplifiedTrace.EdgeCount(), 2);
    ExpectEdge(SimplifiedTrace.Edge(0), SCircuitTraceEdge{A, C});
    ExpectEdge(SimplifiedTrace.Edge(1), SCircuitTraceEdge{C, E});
}

TEST_F(CCircuitTraceTest, SimplifyAlreadySimplified) {
    EXPECT_TRUE(Trace.AddEdge(SCircuitTraceEdge{A, D}));

    auto SimplifiedTrace = Trace.Simplify();

    EXPECT_EQ(SimplifiedTrace.EdgeCount(), 1);
    ExpectEdge(SimplifiedTrace.Edge(0), SCircuitTraceEdge{A, D});
}

TEST_F(CCircuitTraceTest, SimplifySingleEdge) {
    EXPECT_TRUE(Trace.AddEdge(EdgeAB));

    auto SimplifiedTrace = Trace.Simplify();

    EXPECT_EQ(SimplifiedTrace.EdgeCount(), 1);
    ExpectEdge(SimplifiedTrace.Edge(0), EdgeAB);
}

TEST_F(CCircuitTraceTest, SimplifyEmptyTrace) {
    auto SimplifiedTrace = Trace.Simplify();

    EXPECT_EQ(SimplifiedTrace.EdgeCount(), 0);
    EXPECT_EQ(SimplifiedTrace.LocationCount(), 0);
}


// Elaborate

TEST_F(CCircuitTraceTest, ElaborateHorizontalEdge) {
    SCircuitLocation End{3, 0};

    EXPECT_TRUE(Trace.AddEdge(SCircuitTraceEdge{{0, 0}, End}));

    auto ElaboratedTrace = Trace.Elaborate();

    EXPECT_EQ(ElaboratedTrace.EdgeCount(), 3);
    ExpectEdge(ElaboratedTrace.Edge(0), SCircuitTraceEdge{{0, 0}, {1, 0}});
    ExpectEdge(ElaboratedTrace.Edge(1), SCircuitTraceEdge{{1, 0}, {2, 0}});
    ExpectEdge(ElaboratedTrace.Edge(2), SCircuitTraceEdge{{2, 0}, {3, 0}});
}

TEST_F(CCircuitTraceTest, ElaborateVerticalEdge) {
    SCircuitLocation End{0, 3};

    EXPECT_TRUE(Trace.AddEdge(SCircuitTraceEdge{{0, 0}, End}));

    auto ElaboratedTrace = Trace.Elaborate();

    EXPECT_EQ(ElaboratedTrace.EdgeCount(), 3);
    ExpectEdge(ElaboratedTrace.Edge(0), SCircuitTraceEdge{{0, 0}, {0, 1}});
    ExpectEdge(ElaboratedTrace.Edge(1), SCircuitTraceEdge{{0, 1}, {0, 2}});
    ExpectEdge(ElaboratedTrace.Edge(2), SCircuitTraceEdge{{0, 2}, {0, 3}});
}

TEST_F(CCircuitTraceTest, ElaborateDiagonalEdge) {
    SCircuitLocation Start{0, 0};
    SCircuitLocation End{3, 3};

    EXPECT_TRUE(Trace.AddEdge(SCircuitTraceEdge{Start, End}));

    auto ElaboratedTrace = Trace.Elaborate();

    EXPECT_EQ(ElaboratedTrace.EdgeCount(), 3);
    ExpectEdge(ElaboratedTrace.Edge(0), SCircuitTraceEdge{{0, 0}, {1, 1}});
    ExpectEdge(ElaboratedTrace.Edge(1), SCircuitTraceEdge{{1, 1}, {2, 2}});
    ExpectEdge(ElaboratedTrace.Edge(2), SCircuitTraceEdge{{2, 2}, {3, 3}});
}

TEST_F(CCircuitTraceTest, ElaborateReverseDiagonalEdge) {
    SCircuitLocation Start{3, 3};
    SCircuitLocation End{0, 0};

    EXPECT_TRUE(Trace.AddEdge(SCircuitTraceEdge{Start, End}));

    auto ElaboratedTrace = Trace.Elaborate();

    EXPECT_EQ(ElaboratedTrace.EdgeCount(), 3);
    ExpectEdge(ElaboratedTrace.Edge(0), SCircuitTraceEdge{{3, 3}, {2, 2}});
    ExpectEdge(ElaboratedTrace.Edge(1), SCircuitTraceEdge{{2, 2}, {1, 1}});
    ExpectEdge(ElaboratedTrace.Edge(2), SCircuitTraceEdge{{1, 1}, {0, 0}});
}