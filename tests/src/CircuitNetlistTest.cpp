
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "CircuitNetlist.h"

class CCircuitNetlistTest : public ::testing::Test {
    protected:
        CCircuitNetlist Netlist;
};

// ------------------------------------------------------------
// Initial state
// ------------------------------------------------------------

TEST_F(CCircuitNetlistTest, InitiallyEmpty) {
    EXPECT_EQ(0, Netlist.CircuitCount());
}

TEST_F(CCircuitNetlistTest, CopyConstruct) {
    CCircuitNetlist Moved(Netlist);
    EXPECT_EQ(0, Moved.CircuitCount());
}

TEST_F(CCircuitNetlistTest, MoveConstruct) {
    CCircuitNetlist Moved(std::move(Netlist));
    EXPECT_EQ(0, Moved.CircuitCount());
}

TEST_F(CCircuitNetlistTest, MoveAssignment) {
    CCircuitNetlist Moved;
    Moved = std::move(Netlist);
    EXPECT_EQ(0, Moved.CircuitCount());
}


// ------------------------------------------------------------
// Adding pins
// ------------------------------------------------------------

TEST_F(CCircuitNetlistTest, AddFirstPin) {
    TCircuitName Circuit = "Circuit1";
    SCircuitLocation Location{0, 0};

    EXPECT_TRUE(Netlist.AddPin(Circuit, Location));

    EXPECT_EQ(1, Netlist.CircuitCount());
    EXPECT_EQ(1, Netlist.PinCount(0));
    TCircuitName Circuit0;
    SCircuitLocation Location0;
    EXPECT_TRUE(Netlist.CircuitName(0,Circuit0));
    EXPECT_EQ(Circuit,Circuit0);
    EXPECT_TRUE(Netlist.PinLocation(0,0,Location0));
    EXPECT_EQ(Location, Location0);
}

TEST_F(CCircuitNetlistTest, AddMultiplePinsToSameCircuit) {
    TCircuitName Circuit = "Circuit1";

    SCircuitLocation Location1{0, 0};
    SCircuitLocation Location2{0, 1};
    SCircuitLocation Location3{1, 0};

    EXPECT_TRUE(Netlist.AddPin(Circuit, Location1));
    EXPECT_TRUE(Netlist.AddPin(Circuit, Location2));
    EXPECT_TRUE(Netlist.AddPin(Circuit, Location3));

    EXPECT_EQ(1, Netlist.CircuitCount());
    EXPECT_EQ(3, Netlist.PinCount(0));

    SCircuitLocation PinLocation00, PinLocation01, PinLocation02;
    EXPECT_TRUE(Netlist.PinLocation(0,0,PinLocation00));
    EXPECT_TRUE(Netlist.PinLocation(0,1,PinLocation01));
    EXPECT_TRUE(Netlist.PinLocation(0,2,PinLocation02));


    EXPECT_EQ(Location1, PinLocation00);
    EXPECT_EQ(Location2, PinLocation01);
    EXPECT_EQ(Location3, PinLocation02);
}

TEST_F(CCircuitNetlistTest, AddMultipleCircuits) {
    TCircuitName Circuit1 = "Circuit1";
    TCircuitName Circuit2 = "Circuit2";
    TCircuitName Circuit3 = "Circuit3";

    SCircuitLocation Location{0, 0};

    EXPECT_TRUE(Netlist.AddPin(Circuit1, Location));
    EXPECT_TRUE(Netlist.AddPin(Circuit2, Location));
    EXPECT_TRUE(Netlist.AddPin(Circuit3, Location));

    EXPECT_EQ(3, Netlist.CircuitCount());
    TCircuitName CircuitName0, CircuitName1, CircuitName2;
    EXPECT_TRUE(Netlist.CircuitName(0,CircuitName0));
    EXPECT_TRUE(Netlist.CircuitName(1,CircuitName1));
    EXPECT_TRUE(Netlist.CircuitName(2,CircuitName2));
    EXPECT_EQ(Circuit1, CircuitName0);
    EXPECT_EQ(Circuit2, CircuitName1);
    EXPECT_EQ(Circuit3, CircuitName2);

    EXPECT_EQ(1, Netlist.PinCount(0));
    EXPECT_EQ(1, Netlist.PinCount(1));
    EXPECT_EQ(1, Netlist.PinCount(2));
}

TEST_F(CCircuitNetlistTest, AddPinToEarlierCircuit) {
    EXPECT_TRUE(Netlist.AddPin("Circuit1", {0, 0}));
    EXPECT_TRUE(Netlist.AddPin("Circuit2", {0, 1}));
    EXPECT_TRUE(Netlist.AddPin("Circuit1", {0, 2}));

    EXPECT_EQ(2, Netlist.CircuitCount());
    EXPECT_EQ(2, Netlist.PinCount(0));
    EXPECT_EQ(1, Netlist.PinCount(1));

    SCircuitLocation PinLocation;
    EXPECT_TRUE(Netlist.PinLocation(0,1,PinLocation));
    EXPECT_EQ((SCircuitLocation{0, 2}),PinLocation);
}

// ------------------------------------------------------------
// Duplicate pins and locations
// ------------------------------------------------------------

TEST_F(CCircuitNetlistTest, RejectDuplicatePin) {
    TCircuitName Circuit = "Circuit1";
    SCircuitLocation Location{0, 0};

    EXPECT_TRUE(Netlist.AddPin(Circuit, Location));
    EXPECT_FALSE(Netlist.AddPin(Circuit, Location));

    EXPECT_EQ(1, Netlist.CircuitCount());
    EXPECT_EQ(1, Netlist.PinCount(0));
}

TEST_F(CCircuitNetlistTest, SameLocationDifferentCircuits) {
    SCircuitLocation Location{0, 0};

    EXPECT_TRUE(Netlist.AddPin("Circuit1", Location));
    EXPECT_TRUE(Netlist.AddPin("Circuit2", Location));

    EXPECT_EQ(2, Netlist.CircuitCount());
    EXPECT_EQ(1, Netlist.PinCount(0));
    EXPECT_EQ(1, Netlist.PinCount(1));
}

TEST_F(CCircuitNetlistTest, DifferentLocationsSameCircuit) {
    EXPECT_TRUE(Netlist.AddPin("Circuit1", {0, 0}));
    EXPECT_TRUE(Netlist.AddPin("Circuit1", {0, 1}));

    EXPECT_EQ(2, Netlist.PinCount(0));
}

// ------------------------------------------------------------
// Circuit names
// ------------------------------------------------------------

TEST_F(CCircuitNetlistTest, RepeatedCircuitName) {
    EXPECT_TRUE(Netlist.AddPin("Circuit1", {0, 0}));
    EXPECT_TRUE(Netlist.AddPin("Circuit1", {0, 1}));

    EXPECT_EQ(1, Netlist.CircuitCount());
    TCircuitName CircuitName0;
    EXPECT_TRUE(Netlist.CircuitName(0,CircuitName0));
    EXPECT_EQ("Circuit1", CircuitName0);
}

TEST_F(CCircuitNetlistTest, CircuitOrdering) {
    EXPECT_TRUE(Netlist.AddPin("Alpha", {0, 0}));
    EXPECT_TRUE(Netlist.AddPin("Charlie", {0, 1}));
    EXPECT_TRUE(Netlist.AddPin("Bravo", {0, 2}));
    TCircuitName CircuitName0, CircuitName1, CircuitName2;
    EXPECT_TRUE(Netlist.CircuitName(0,CircuitName0));
    EXPECT_TRUE(Netlist.CircuitName(1,CircuitName1));
    EXPECT_TRUE(Netlist.CircuitName(2,CircuitName2));

    EXPECT_EQ("Alpha",   CircuitName0);
    EXPECT_EQ("Charlie", CircuitName1);
    EXPECT_EQ("Bravo",   CircuitName2);
}

// ------------------------------------------------------------
// Invalid indices
// ------------------------------------------------------------

TEST_F(CCircuitNetlistTest, InvalidCircuitIndexPinCount) {
    EXPECT_EQ(0, Netlist.PinCount(0));
    EXPECT_EQ(0, Netlist.PinCount(100));
}

TEST_F(CCircuitNetlistTest, InvalidCircuitIndexCircuitName) {
    TCircuitName CircuitName0, CircuitName100;
    EXPECT_FALSE(Netlist.CircuitName(0,CircuitName0));
    EXPECT_FALSE(Netlist.CircuitName(1000,CircuitName100));
    EXPECT_EQ(TCircuitName{}, CircuitName0);
    EXPECT_EQ(TCircuitName{}, CircuitName100);
}

TEST_F(CCircuitNetlistTest, InvalidCircuitIndexPinLocation) {
    SCircuitLocation PinLocation00, PinLocation1000;
    EXPECT_FALSE(Netlist.PinLocation(0,0,PinLocation00));
    EXPECT_FALSE(Netlist.PinLocation(100,0,PinLocation1000));
    EXPECT_EQ(SCircuitLocation{}, PinLocation00);
    EXPECT_EQ(SCircuitLocation{}, PinLocation1000);
}

TEST_F(CCircuitNetlistTest, InvalidPinIndex) {
    EXPECT_TRUE(Netlist.AddPin("Circuit1", {0, 0}));

    SCircuitLocation PinLocation01, PinLocation0100;
    EXPECT_FALSE(Netlist.PinLocation(0,1,PinLocation01));
    EXPECT_FALSE(Netlist.PinLocation(0,100,PinLocation0100));
    EXPECT_EQ(SCircuitLocation{}, PinLocation01);
    EXPECT_EQ(SCircuitLocation{}, PinLocation0100);
}

// ------------------------------------------------------------
// Empty and invalid names
// ------------------------------------------------------------

TEST_F(CCircuitNetlistTest, EmptyCircuitName) {
    EXPECT_FALSE(Netlist.AddPin("", {0, 0}));

    EXPECT_EQ(0, Netlist.CircuitCount());
}

TEST_F(CCircuitNetlistTest, EmptyCircuitNameDoesNotAffectExistingCircuits) {
    EXPECT_TRUE(Netlist.AddPin("Circuit1", {0, 0}));

    EXPECT_FALSE(Netlist.AddPin("", {0, 1}));

    EXPECT_EQ(1, Netlist.CircuitCount());
    EXPECT_EQ(1, Netlist.PinCount(0));
}

// ------------------------------------------------------------
// Large-scale tests
// ------------------------------------------------------------

TEST_F(CCircuitNetlistTest, LargeNumberOfPins) {
    constexpr std::size_t PinTotal = 10000;

    for(std::size_t Index = 0; Index < PinTotal; Index++) {
        EXPECT_TRUE(Netlist.AddPin(
            "Circuit1",
            {Index, Index}));
    }

    EXPECT_EQ(1, Netlist.CircuitCount());
    EXPECT_EQ(PinTotal, Netlist.PinCount(0));

    for(std::size_t Index = 0; Index < PinTotal; Index++) {
        SCircuitLocation PinLocation;
        EXPECT_TRUE(Netlist.PinLocation(0, Index, PinLocation));
        EXPECT_EQ((SCircuitLocation{Index, Index}),PinLocation);
    }
}

TEST_F(CCircuitNetlistTest, LargeNumberOfCircuits) {
    constexpr std::size_t CircuitTotal = 1000;

    for(std::size_t Index = 0; Index < CircuitTotal; Index++) {
        EXPECT_TRUE(Netlist.AddPin(
            "Circuit" + std::to_string(Index),
            {Index, 0}));
    }

    EXPECT_EQ(CircuitTotal, Netlist.CircuitCount());

    for(std::size_t Index = 0; Index < CircuitTotal; Index++) {
        TCircuitName CircuitName;
        EXPECT_TRUE(Netlist.CircuitName(Index,CircuitName));
        EXPECT_EQ(
            "Circuit" + std::to_string(Index),
            CircuitName);

        EXPECT_EQ(1, Netlist.PinCount(Index));

        SCircuitLocation PinLocation;
        EXPECT_TRUE(Netlist.PinLocation(Index, 0, PinLocation));
        ASSERT_EQ((SCircuitLocation{Index, 0}),
                  PinLocation);
    }
}

// ------------------------------------------------------------
// Stress and consistency tests
// ------------------------------------------------------------

TEST_F(CCircuitNetlistTest, DuplicatePinDoesNotChangeCounts) {
    EXPECT_TRUE(Netlist.AddPin("Circuit1", {0, 0}));
    EXPECT_TRUE(Netlist.AddPin("Circuit1", {0, 1}));
    EXPECT_TRUE(Netlist.AddPin("Circuit2", {0, 0}));

    EXPECT_FALSE(Netlist.AddPin("Circuit1", {0, 0}));
    EXPECT_FALSE(Netlist.AddPin("Circuit1", {0, 1}));
    EXPECT_FALSE(Netlist.AddPin("Circuit2", {0, 0}));

    EXPECT_EQ(2, Netlist.CircuitCount());
    EXPECT_EQ(2, Netlist.PinCount(0));
    EXPECT_EQ(1, Netlist.PinCount(1));
}

TEST_F(CCircuitNetlistTest, InterleavedCircuitInsertion) {
    EXPECT_TRUE(Netlist.AddPin("A", {0, 0}));
    EXPECT_TRUE(Netlist.AddPin("B", {0, 0}));
    EXPECT_TRUE(Netlist.AddPin("C", {0, 0}));
    EXPECT_TRUE(Netlist.AddPin("A", {0, 1}));
    EXPECT_TRUE(Netlist.AddPin("B", {0, 1}));
    EXPECT_TRUE(Netlist.AddPin("A", {0, 2}));

    EXPECT_EQ(3, Netlist.CircuitCount());

    EXPECT_EQ(3, Netlist.PinCount(0));
    EXPECT_EQ(2, Netlist.PinCount(1));
    EXPECT_EQ(1, Netlist.PinCount(2));
}

