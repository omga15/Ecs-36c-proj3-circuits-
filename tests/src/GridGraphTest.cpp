#include "GridGraph.h"
#include "GridGraphFactory.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <set>
#include <tuple>
#include <unordered_map>
#include <vector>

// Replace this with your concrete Grid implementation types.
using GridGraphImplementations = ::testing::Types<YOUR_GRAPH_HERE>;

// -----------------------------------------------------------------------------
// Typed test fixture
// -----------------------------------------------------------------------------

template <typename T>
class GridGraphInterfaceTest : public ::testing::Test{
protected:
    using TTraversalFunction = CGraph::TTraversalFunction;
    using ETraversal = CGraph::EGraphTraversal;
    using TVertexID = CGraph::TVertexID;

    struct SDirection{
        EGridDirection DDirection;
        int DX;
        int DY;
    };

    static constexpr std::array<SDirection, 8> Directions = {{
        {EGridDirection::N,   0, -1},
        {EGridDirection::NE,  1, -1},
        {EGridDirection::E,   1,  0},
        {EGridDirection::SE,  1,  1},
        {EGridDirection::S,   0,  1},
        {EGridDirection::SW, -1,  1},
        {EGridDirection::W,  -1,  0},
        {EGridDirection::NW, -1, -1}
    }};

    static std::unique_ptr<CGraph> CreateGrid(
        TGridCoordinate width,
        TGridCoordinate height,
        const SGridConnectionPattern& pattern = {}){
        return SGridGraphFactory<T>::Create(width, height, pattern);
    }

    static CGridGraph* Grid(CGraph* graph){
        return dynamic_cast<CGridGraph*>(graph);
    }

    static CMutableGridGraph* MutableGrid(CGraph* graph){
        return dynamic_cast<CMutableGridGraph*>(graph);
    }

    static bool Step(
        TGridCoordinate x,
        TGridCoordinate y,
        EGridDirection direction,
        TGridCoordinate width,
        TGridCoordinate height,
        TGridCoordinate& nx,
        TGridCoordinate& ny){
        int DX = 0;
        int DY = 0;

        for(const auto& Entry : Directions){
            if(Entry.DDirection == direction){
                DX = Entry.DX;
                DY = Entry.DY;
                break;
            }
        }

        if(DX < 0 && x == 0){
            return false;
        }

        if(DY < 0 && y == 0){
            return false;
        }

        if(DX > 0 && (x >= width || x + 1 >= width)){
            return false;
        }

        if(DY > 0 && (y >= height || y + 1 >= height)){
            return false;
        }

        nx = DX < 0 ? x - 1 : DX > 0 ? x + 1 : x;
        ny = DY < 0 ? y - 1 : DY > 0 ? y + 1 : y;

        return nx < width && ny < height;
    }

    static TGridVertexConnectionMask PatternMask(
        const SGridConnectionPattern& pattern,
        TGridCoordinate x,
        TGridCoordinate y){
        const bool EvenRow = (y % 2) == 0;
        const bool EvenColumn = (x % 2) == 0;

        if(EvenRow && EvenColumn){
            return pattern.DEvenRowEvenColumn;
        }

        if(EvenRow && !EvenColumn){
            return pattern.DEvenRowOddColumn;
        }

        if(!EvenRow && EvenColumn){
            return pattern.DOddRowEvenColumn;
        }

        return pattern.DOddRowOddColumn;
    }

    static TGridVertexConnectionMask BoundaryMask(
        TGridCoordinate x,
        TGridCoordinate y,
        TGridCoordinate width,
        TGridCoordinate height){
        TGridVertexConnectionMask mask = GridVertexConnectionNone;

        for(const auto& Entry : Directions){
            TGridCoordinate NX = 0;
            TGridCoordinate NY = 0;

            if(Step(x, y, Entry.DDirection, width, height, NX, NY)){
                mask |= GridDirectionBit(Entry.DDirection);
            }
        }

        return mask;
    }

    static TGridVertexConnectionMask EffectivePatternMask(
        const SGridConnectionPattern& pattern,
        TGridCoordinate x,
        TGridCoordinate y,
        TGridCoordinate width,
        TGridCoordinate height){
        return PatternMask(pattern, x, y) &
               BoundaryMask(x, y, width, height);
    }

    static std::set<TVertexID> ExpectedNeighbors(
        CGridGraph* grid,
        TVertexID id){
        std::set<TVertexID> Result;

        SGridLocation Location{};
        if(!grid->GetVertexLocation(id, Location)){
            return Result;
        }

        const auto Mask = grid->GetVertexConnectionMask(id);

        for(const auto& Entry : Directions){
            if((Mask & GridDirectionBit(Entry.DDirection)) == 0){
                continue;
            }

            TGridCoordinate NX = 0;
            TGridCoordinate NY = 0;

            if(!Step(Location.DX, Location.DY, Entry.DDirection, grid->Width(), grid->Height(), NX, NY)){
                continue;
            }

            const auto Neighbor = grid->GetVertex(NX, NY);

            if(Neighbor != CGraph::VertexIDInvalid){
                Result.insert(Neighbor);
            }
        }

        return Result;
    }

    static std::set<TVertexID> ToSet(const std::vector<TVertexID>& values){
        return {values.begin(), values.end()};
    }
};

TYPED_TEST_SUITE(GridGraphInterfaceTest, GridGraphImplementations);

// -----------------------------------------------------------------------------
// Dimensions and coordinates
// -----------------------------------------------------------------------------

TYPED_TEST(GridGraphInterfaceTest, DimensionsDetermineVertexCount){
    constexpr TGridCoordinate Width = 5;
    constexpr TGridCoordinate Height = 4;

    auto Graph = TestFixture::CreateGrid(Width, Height);
    ASSERT_NE(Graph, nullptr);

    auto* Grid = TestFixture::Grid(Graph.get());
    ASSERT_NE(Grid, nullptr);

    EXPECT_EQ(Grid->Width(), Width);
    EXPECT_EQ(Grid->Height(), Height);
    EXPECT_EQ(Graph->VertexCount(), Width * Height);
}

TYPED_TEST(GridGraphInterfaceTest, ZeroWidthGridHasNoVertices){
    auto Graph = TestFixture::CreateGrid(0, 4);
    ASSERT_NE(Graph, nullptr);
    
    auto* Grid = TestFixture::Grid(Graph.get());
    ASSERT_NE(Grid, nullptr);
    
    EXPECT_EQ(Grid->Width(), 0U);
    EXPECT_EQ(Grid->Height(), 4U);
    EXPECT_EQ(Graph->VertexCount(), 0U);
}

TYPED_TEST(GridGraphInterfaceTest, ZeroHeightGridHasNoVertices){
    auto Graph = TestFixture::CreateGrid(4, 0);
    ASSERT_NE(Graph, nullptr);

    auto* Grid = TestFixture::Grid(Graph.get());
    ASSERT_NE(Grid, nullptr);

    EXPECT_EQ(Grid->Width(), 4U);
    EXPECT_EQ(Grid->Height(), 0U);
    EXPECT_EQ(Graph->VertexCount(), 0U);
}

TYPED_TEST(GridGraphInterfaceTest, EveryValidCoordinateMapsToAVertex){
    constexpr TGridCoordinate Width = 5;
    constexpr TGridCoordinate Height = 4;

    auto Graph = TestFixture::CreateGrid(Width, Height);
    ASSERT_NE(Graph, nullptr);

    auto* Grid = TestFixture::Grid(Graph.get());
    ASSERT_NE(Grid, nullptr);

    std::set<CGraph::TVertexID> ids;

    for(TGridCoordinate y = 0; y < Height; ++y){
        for(TGridCoordinate x = 0; x < Width; ++x){
            const auto id = Grid->GetVertex(x, y);

            ASSERT_NE(id, CGraph::VertexIDInvalid);
            EXPECT_LT(id, Graph->VertexCount());
            EXPECT_TRUE(ids.insert(id).second)
                << "Duplicate ID at (" << x << ", " << y << ")";
        }
    }

    EXPECT_EQ(ids.size(), Width * Height);
}

TYPED_TEST(GridGraphInterfaceTest, CoordinateOverloadsAgree){
    auto Graph = TestFixture::CreateGrid(4, 3);
    ASSERT_NE(Graph, nullptr);

    auto* Grid = TestFixture::Grid(Graph.get());
    ASSERT_NE(Grid, nullptr);

    for(TGridCoordinate y = 0; y < Grid->Height(); ++y){
        for(TGridCoordinate x = 0; x < Grid->Width(); ++x){
            EXPECT_EQ(
                Grid->GetVertex(x, y),
                Grid->GetVertex(SGridLocation{x, y}));
        }
    }
}

TYPED_TEST(GridGraphInterfaceTest, OutOfBoundsCoordinatesReturnInvalidID){
    auto Graph = TestFixture::CreateGrid(4, 3);
    ASSERT_NE(Graph, nullptr);

    auto* Grid = TestFixture::Grid(Graph.get());
    ASSERT_NE(Grid, nullptr);

    EXPECT_EQ(Grid->GetVertex(4, 0), CGraph::VertexIDInvalid);
    EXPECT_EQ(Grid->GetVertex(0, 3), CGraph::VertexIDInvalid);
    EXPECT_EQ(Grid->GetVertex(4, 3), CGraph::VertexIDInvalid);

    EXPECT_EQ(
        Grid->GetVertex(SGridLocation{4, 0}),
        CGraph::VertexIDInvalid);

    EXPECT_EQ(
        Grid->GetVertex(SGridLocation{0, 3}),
        CGraph::VertexIDInvalid);
}

TYPED_TEST(GridGraphInterfaceTest, VertexLocationsRoundTrip){
    constexpr TGridCoordinate Width = 5;
    constexpr TGridCoordinate Height = 4;

    auto Graph = TestFixture::CreateGrid(Width, Height);
    ASSERT_NE(Graph, nullptr);

    auto* Grid = TestFixture::Grid(Graph.get());
    ASSERT_NE(Grid, nullptr);

    for(TGridCoordinate y = 0; y < Height; ++y){
        for(TGridCoordinate x = 0; x < Width; ++x){
            const auto id = Grid->GetVertex(x, y);
            ASSERT_NE(id, CGraph::VertexIDInvalid);

            SGridLocation location{};
            ASSERT_TRUE(Grid->GetVertexLocation(id, location));

            EXPECT_EQ(location.DX, x);
            EXPECT_EQ(location.DY, y);
            EXPECT_EQ(Grid->GetVertex(location), id);
        }
    }
}

TYPED_TEST(GridGraphInterfaceTest, InvalidVertexHasNoLocation){
    auto Graph = TestFixture::CreateGrid(3, 3);
    ASSERT_NE(Graph, nullptr);

    auto* Grid = TestFixture::Grid(Graph.get());
    ASSERT_NE(Grid, nullptr);

    SGridLocation location{};

    EXPECT_FALSE(
        Grid->GetVertexLocation(CGraph::VertexIDInvalid, location));

    EXPECT_FALSE(Grid->GetVertexLocation(1000, location));
}

// -----------------------------------------------------------------------------
// Direction bits and masks
// -----------------------------------------------------------------------------

TYPED_TEST(GridGraphInterfaceTest, DirectionBitsAreDistinct){
    TGridVertexConnectionMask combined = GridVertexConnectionNone;

    for(const auto& Entry : TestFixture::Directions){
        const auto bit = GridDirectionBit(Entry.DDirection);

        EXPECT_NE(bit, GridVertexConnectionNone);
        EXPECT_EQ(combined & bit, 0);
        combined |= bit;
    }

    EXPECT_EQ(combined, GridVertexConnectionAll);
}

TYPED_TEST(GridGraphInterfaceTest, DefaultPatternEnablesAllDirections){
    auto Graph = TestFixture::CreateGrid(3, 3);
    ASSERT_NE(Graph, nullptr);

    auto* Grid = TestFixture::Grid(Graph.get());
    ASSERT_NE(Grid, nullptr);

    const auto pattern = Grid->GetGridConnectionPattern();

    EXPECT_EQ(pattern.DEvenRowEvenColumn, GridVertexConnectionAll);
    EXPECT_EQ(pattern.DEvenRowOddColumn, GridVertexConnectionAll);
    EXPECT_EQ(pattern.DOddRowEvenColumn, GridVertexConnectionAll);
    EXPECT_EQ(pattern.DOddRowOddColumn, GridVertexConnectionAll);
}

TYPED_TEST(GridGraphInterfaceTest, CornerMaskExcludesOutOfBoundsDirections){
    auto Graph = TestFixture::CreateGrid(3, 3);
    ASSERT_NE(Graph, nullptr);

    auto* Grid = TestFixture::Grid(Graph.get());
    ASSERT_NE(Grid, nullptr);

    const auto id = Grid->GetVertex(0, 0);
    ASSERT_NE(id, CGraph::VertexIDInvalid);

    const auto expected =
        GridDirectionBit(EGridDirection::E) |
        GridDirectionBit(EGridDirection::SE) |
        GridDirectionBit(EGridDirection::S);
    EXPECT_EQ(Grid->GetVertexConnectionMask(id), expected);
}

TYPED_TEST(GridGraphInterfaceTest, EdgeMaskExcludesOutOfBoundsDirections){
    auto Graph = TestFixture::CreateGrid(3, 3);
    ASSERT_NE(Graph, nullptr);

    auto* Grid = TestFixture::Grid(Graph.get());
    ASSERT_NE(Grid, nullptr);

    // Top edge, excluding corners.
    const auto id = Grid->GetVertex(1, 0);
    ASSERT_NE(id, CGraph::VertexIDInvalid);

    const auto expected =
        GridDirectionBit(EGridDirection::E)  |
        GridDirectionBit(EGridDirection::SE) |
        GridDirectionBit(EGridDirection::S)  |
        GridDirectionBit(EGridDirection::SW) |
        GridDirectionBit(EGridDirection::W);
    EXPECT_EQ(Grid->GetVertexConnectionMask(id), expected);
}

TYPED_TEST(GridGraphInterfaceTest, InteriorMaskAllowsAllDirections){
    auto Graph = TestFixture::CreateGrid(3, 3);
    ASSERT_NE(Graph, nullptr);

    auto* Grid = TestFixture::Grid(Graph.get());
    ASSERT_NE(Grid, nullptr);

    const auto id = Grid->GetVertex(1, 1);
    ASSERT_NE(id, CGraph::VertexIDInvalid);

    EXPECT_EQ(
        Grid->GetVertexConnectionMask(id),
        GridVertexConnectionAll);
}

TYPED_TEST(GridGraphInterfaceTest, SingleCellHasNoConnections){
    auto Graph = TestFixture::CreateGrid(1, 1);
    ASSERT_NE(Graph, nullptr);

    auto* Grid = TestFixture::Grid(Graph.get());
    ASSERT_NE(Grid, nullptr);

    const auto id = Grid->GetVertex(0, 0);
    ASSERT_NE(id, CGraph::VertexIDInvalid);

    EXPECT_EQ(
        Grid->GetVertexConnectionMask(id),
        GridVertexConnectionNone);
}

TYPED_TEST(GridGraphInterfaceTest, PatternSelectsMaskByParity){
    SGridConnectionPattern pattern{
        GridDirectionBit(EGridDirection::N),
        GridDirectionBit(EGridDirection::E),
        GridDirectionBit(EGridDirection::S),
        GridDirectionBit(EGridDirection::W)
    };

    auto Graph = TestFixture::CreateGrid(4, 4, pattern);
    ASSERT_NE(Graph, nullptr);

    auto* Grid = TestFixture::Grid(Graph.get());
    ASSERT_NE(Grid, nullptr);

    for(TGridCoordinate y = 0; y < Grid->Height(); ++y){
        for(TGridCoordinate x = 0; x < Grid->Width(); ++x){
            const auto id = Grid->GetVertex(x, y);
            ASSERT_NE(id, CGraph::VertexIDInvalid);

            EXPECT_EQ(
                Grid->GetVertexConnectionMask(id),
                TestFixture::EffectivePatternMask(
                    pattern, x, y, Grid->Width(), Grid->Height()));
        }
    }
}

// -----------------------------------------------------------------------------
// Mutable global pattern and per-vertex overrides
// -----------------------------------------------------------------------------

TYPED_TEST(GridGraphInterfaceTest, SetGridConnectionPatternUpdatesDefaultMasks){
    auto Graph = TestFixture::CreateGrid(4, 4);
    ASSERT_NE(Graph, nullptr);

    auto* Grid = TestFixture::Grid(Graph.get());
    auto* mutableGrid = TestFixture::MutableGrid(Graph.get());

    ASSERT_NE(Grid, nullptr);
    ASSERT_NE(mutableGrid, nullptr);

    SGridConnectionPattern pattern{
        GridDirectionBit(EGridDirection::E),
        GridDirectionBit(EGridDirection::S),
        GridDirectionBit(EGridDirection::W),
        GridDirectionBit(EGridDirection::N)
    };

    ASSERT_TRUE(mutableGrid->SetGridConnectionPattern(pattern));

    const auto stored = Grid->GetGridConnectionPattern();

    EXPECT_EQ(stored.DEvenRowEvenColumn, pattern.DEvenRowEvenColumn);
    EXPECT_EQ(stored.DEvenRowOddColumn, pattern.DEvenRowOddColumn);
    EXPECT_EQ(stored.DOddRowEvenColumn, pattern.DOddRowEvenColumn);
    EXPECT_EQ(stored.DOddRowOddColumn, pattern.DOddRowOddColumn);

    for(TGridCoordinate y = 0; y < Grid->Height(); ++y){
        for(TGridCoordinate x = 0; x < Grid->Width(); ++x){
            const auto id = Grid->GetVertex(x, y);
            ASSERT_NE(id, CGraph::VertexIDInvalid);

            EXPECT_EQ(
                Grid->GetVertexConnectionMask(id),
                TestFixture::EffectivePatternMask(
                    pattern, x, y, Grid->Width(), Grid->Height()));
        }
    }
}

TYPED_TEST(GridGraphInterfaceTest, SetVertexConnectionMaskOverridesPattern){
    auto Graph = TestFixture::CreateGrid(3, 3);
    ASSERT_NE(Graph, nullptr);

    auto* Grid = TestFixture::Grid(Graph.get());
    auto* mutableGrid = TestFixture::MutableGrid(Graph.get());

    ASSERT_NE(Grid, nullptr);
    ASSERT_NE(mutableGrid, nullptr);

    const auto id = Grid->GetVertex(1, 1);
    ASSERT_NE(id, CGraph::VertexIDInvalid);

    const auto overrideMask =
        GridDirectionBit(EGridDirection::N) |
        GridDirectionBit(EGridDirection::E);

    ASSERT_TRUE(mutableGrid->SetVertexConnectionMask(id, overrideMask));

    EXPECT_EQ(Grid->GetVertexConnectionMask(id), overrideMask);
}

TYPED_TEST(GridGraphInterfaceTest, PerVertexOverrideSurvivesPatternChange){
    auto Graph = TestFixture::CreateGrid(3, 3);
    ASSERT_NE(Graph, nullptr);

    auto* Grid = TestFixture::Grid(Graph.get());
    auto* mutableGrid = TestFixture::MutableGrid(Graph.get());

    ASSERT_NE(Grid, nullptr);
    ASSERT_NE(mutableGrid, nullptr);

    const auto id = Grid->GetVertex(1, 1);
    ASSERT_NE(id, CGraph::VertexIDInvalid);

    const auto overrideMask = GridDirectionBit(EGridDirection::NW);

    ASSERT_TRUE(mutableGrid->SetVertexConnectionMask(id, overrideMask));

    SGridConnectionPattern pattern{
        GridVertexConnectionNone,
        GridVertexConnectionNone,
        GridVertexConnectionNone,
        GridVertexConnectionNone
    };

    ASSERT_TRUE(mutableGrid->SetGridConnectionPattern(pattern));

    EXPECT_EQ(Grid->GetVertexConnectionMask(id), overrideMask);
}

TYPED_TEST(GridGraphInterfaceTest, ClearVertexConnectionMaskRestoresPattern){
    SGridConnectionPattern pattern{
        GridDirectionBit(EGridDirection::E),
        GridDirectionBit(EGridDirection::S),
        GridDirectionBit(EGridDirection::W),
        GridDirectionBit(EGridDirection::N)
    };

    auto Graph = TestFixture::CreateGrid(3, 3, pattern);
    ASSERT_NE(Graph, nullptr);

    auto* Grid = TestFixture::Grid(Graph.get());
    auto* mutableGrid = TestFixture::MutableGrid(Graph.get());

    ASSERT_NE(Grid, nullptr);
    ASSERT_NE(mutableGrid, nullptr);

    const auto id = Grid->GetVertex(1, 1);
    ASSERT_NE(id, CGraph::VertexIDInvalid);

    ASSERT_TRUE(
        mutableGrid->SetVertexConnectionMask(
            id, GridVertexConnectionNone));

    EXPECT_EQ(
        Grid->GetVertexConnectionMask(id),
        GridVertexConnectionNone);

    ASSERT_TRUE(mutableGrid->ClearVertexConnectionMask(id));

    EXPECT_EQ(
        Grid->GetVertexConnectionMask(id),
        TestFixture::EffectivePatternMask(
            pattern, 1, 1, Grid->Width(), Grid->Height()));
}

TYPED_TEST(GridGraphInterfaceTest, BoundaryFilteringAppliesToOverride){
    auto Graph = TestFixture::CreateGrid(3, 3);
    ASSERT_NE(Graph, nullptr);

    auto* Grid = TestFixture::Grid(Graph.get());
    auto* mutableGrid = TestFixture::MutableGrid(Graph.get());

    ASSERT_NE(Grid, nullptr);
    ASSERT_NE(mutableGrid, nullptr);

    const auto id = Grid->GetVertex(0, 0);
    ASSERT_NE(id, CGraph::VertexIDInvalid);

    ASSERT_TRUE(
        mutableGrid->SetVertexConnectionMask(
            id, GridVertexConnectionAll));

    const auto expected =
        GridDirectionBit(EGridDirection::E) |
        GridDirectionBit(EGridDirection::SE) |
        GridDirectionBit(EGridDirection::S);

    EXPECT_EQ(Grid->GetVertexConnectionMask(id), expected);
}

TYPED_TEST(GridGraphInterfaceTest, InvalidVertexMaskMutationFails){
    auto Graph = TestFixture::CreateGrid(3, 3);
    ASSERT_NE(Graph, nullptr);

    auto* mutableGrid = TestFixture::MutableGrid(Graph.get());
    ASSERT_NE(mutableGrid, nullptr);

    EXPECT_FALSE(mutableGrid->SetVertexConnectionMask(
        CGraph::VertexIDInvalid, GridVertexConnectionNone));

    EXPECT_FALSE(mutableGrid->SetVertexConnectionMask(
        1000, GridVertexConnectionNone));

    EXPECT_FALSE(
        mutableGrid->ClearVertexConnectionMask(CGraph::VertexIDInvalid));

    EXPECT_FALSE(mutableGrid->ClearVertexConnectionMask(1000));
}

// -----------------------------------------------------------------------------
// CGraph tags and topology queries
// -----------------------------------------------------------------------------

TYPED_TEST(GridGraphInterfaceTest, EveryGridVertexHasAReadableTag){
    auto Graph = TestFixture::CreateGrid(3, 2);
    ASSERT_NE(Graph, nullptr);

    for(CGraph::TVertexID id = 0; id < Graph->VertexCount(); ++id){
        CGraph::TVertexTag tag{};
        EXPECT_TRUE(Graph->GetVertexTag(id, tag));
    }

    CGraph::TVertexTag tag{};
    EXPECT_FALSE(
        Graph->GetVertexTag(CGraph::VertexIDInvalid, tag));
}

TYPED_TEST(GridGraphInterfaceTest, NeighborQueriesMatchEffectiveMasks){
    auto Graph = TestFixture::CreateGrid(4, 3);
    ASSERT_NE(Graph, nullptr);

    auto* Grid = TestFixture::Grid(Graph.get());
    ASSERT_NE(Grid, nullptr);

    for(TGridCoordinate y = 0; y < Grid->Height(); ++y){
        for(TGridCoordinate x = 0; x < Grid->Width(); ++x){
            const auto id = Grid->GetVertex(x, y);
            ASSERT_NE(id, CGraph::VertexIDInvalid);

            std::vector<CGraph::TVertexID> neighbors;
            ASSERT_TRUE(Graph->GetVertexNeighbors(id, neighbors));

            EXPECT_EQ(
                TestFixture::ToSet(neighbors),
                TestFixture::ExpectedNeighbors(Grid, id))
                << "Coordinate (" << x << ", " << y << ")";
        }
    }
}

TYPED_TEST(GridGraphInterfaceTest, EdgeExistenceMatchesMasks){
    auto Graph = TestFixture::CreateGrid(4, 3);
    ASSERT_NE(Graph, nullptr);

    auto* Grid = TestFixture::Grid(Graph.get());
    ASSERT_NE(Grid, nullptr);

    for(TGridCoordinate y = 0; y < Grid->Height(); ++y){
        for(TGridCoordinate x = 0; x < Grid->Width(); ++x){
            const auto src = Grid->GetVertex(x, y);
            ASSERT_NE(src, CGraph::VertexIDInvalid);

            const auto mask = Grid->GetVertexConnectionMask(src);

            for(const auto& Entry : TestFixture::Directions){
                TGridCoordinate nx = 0;
                TGridCoordinate ny = 0;

                const bool inBounds = TestFixture::Step(
                    x, y, Entry.DDirection,
                    Grid->Width(), Grid->Height(), nx, ny);

                const bool permitted =
                    (mask & GridDirectionBit(Entry.DDirection)) != 0;

                if(!inBounds){
                    continue;
                }

                const auto dest = Grid->GetVertex(nx, ny);
                ASSERT_NE(dest, CGraph::VertexIDInvalid);

                EXPECT_EQ(Graph->EdgeExists(src, dest),permitted)
                    << "Source (" << x << ", " << y << ")";
            }
        }
    }
}

TYPED_TEST(GridGraphInterfaceTest, EdgeWeightsMatchDirection){
    auto Graph = TestFixture::CreateGrid(4, 4);
    ASSERT_NE(Graph, nullptr);

    auto* Grid = TestFixture::Grid(Graph.get());
    ASSERT_NE(Grid, nullptr);

    for(TGridCoordinate y = 0; y < Grid->Height(); ++y){
        for(TGridCoordinate x = 0; x < Grid->Width(); ++x){
            const auto src = Grid->GetVertex(x, y);
            ASSERT_NE(src, CGraph::VertexIDInvalid);

            const auto mask = Grid->GetVertexConnectionMask(src);

            for(const auto& Entry : TestFixture::Directions){
                if((mask & GridDirectionBit(Entry.DDirection)) == 0){
                    continue;
                }

                TGridCoordinate nx = 0;
                TGridCoordinate ny = 0;

                if(!TestFixture::Step(
                        x, y, Entry.DDirection,
                        Grid->Width(), Grid->Height(), nx, ny)){
                    continue;
                }

                const auto dest = Grid->GetVertex(nx, ny);
                ASSERT_NE(dest, CGraph::VertexIDInvalid);

                double weight = 0.0;
                ASSERT_TRUE(Graph->GetEdgeWeight(src, dest, weight));

                const bool diagonal =
                    Entry.DX != 0 && Entry.DY != 0;

                EXPECT_DOUBLE_EQ(
                    weight, diagonal ? std::sqrt(2.0) : 1.0);
            }
        }
    }
}

TYPED_TEST(GridGraphInterfaceTest, EdgeCountMatchesMaskDefinedEdges){
    auto Graph = TestFixture::CreateGrid(4, 3);
    ASSERT_NE(Graph, nullptr);

    auto* Grid = TestFixture::Grid(Graph.get());
    ASSERT_NE(Grid, nullptr);

    std::size_t expectedCount = 0;

    for(TGridCoordinate y = 0; y < Grid->Height(); ++y){
        for(TGridCoordinate x = 0; x < Grid->Width(); ++x){
            const auto id = Grid->GetVertex(x, y);
            ASSERT_NE(id, CGraph::VertexIDInvalid);

            const auto mask = Grid->GetVertexConnectionMask(id);

            for(const auto& Entry : TestFixture::Directions){
                if((mask & GridDirectionBit(Entry.DDirection)) == 0){
                    continue;
                }

                TGridCoordinate nx = 0;
                TGridCoordinate ny = 0;

                if(TestFixture::Step(
                        x, y, Entry.DDirection,
                        Grid->Width(), Grid->Height(), nx, ny)){
                    ++expectedCount;
                }
            }
        }
    }
}

TYPED_TEST(GridGraphInterfaceTest, DirectedConnectionsNeedNotBeReciprocal){
    SGridConnectionPattern pattern{
        GridDirectionBit(EGridDirection::E),
        GridVertexConnectionNone,
        GridDirectionBit(EGridDirection::E),
        GridVertexConnectionNone
    };

    auto Graph = TestFixture::CreateGrid(2, 2, pattern);
    ASSERT_NE(Graph, nullptr);

    auto* Grid = TestFixture::Grid(Graph.get());
    ASSERT_NE(Grid, nullptr);

    const auto left = Grid->GetVertex(0, 0);
    const auto right = Grid->GetVertex(1, 0);

    ASSERT_NE(left, CGraph::VertexIDInvalid);
    ASSERT_NE(right, CGraph::VertexIDInvalid);

    EXPECT_TRUE(Graph->EdgeExists(left, right));
    EXPECT_FALSE(Graph->EdgeExists(right, left));
}

// -----------------------------------------------------------------------------
// Breadth-first traversal
// -----------------------------------------------------------------------------

TYPED_TEST(GridGraphInterfaceTest, BreadthFirstSearchVisitsReachableVertices){
    auto Graph = TestFixture::CreateGrid(3, 3);
    ASSERT_NE(Graph, nullptr);

    std::set<CGraph::TVertexID> visited;

    Graph->BreadthFirstSearchUnweighted(
        0,
        [&visited](CGraph::TVertexID id,
                   CGraph::TVertexID,
                   std::size_t,
                   double){
            visited.insert(id);
            return CGraph::EGraphTraversal::Continue;
        });

    EXPECT_EQ(visited.size(), 9U);
}

TYPED_TEST(GridGraphInterfaceTest, BreadthFirstSearchReportsDepthAndParent){
    auto Graph = TestFixture::CreateGrid(3, 3);
    ASSERT_NE(Graph, nullptr);

    auto* Grid = TestFixture::Grid(Graph.get());
    ASSERT_NE(Grid, nullptr);

    const auto start = Grid->GetVertex(0, 0);
    ASSERT_NE(start, CGraph::VertexIDInvalid);

    std::unordered_map<CGraph::TVertexID, std::size_t> depths;
    std::unordered_map<CGraph::TVertexID, CGraph::TVertexID> parents;

    Graph->BreadthFirstSearchUnweighted(
        start,
        [&depths, &parents](
            CGraph::TVertexID id,
            CGraph::TVertexID parent,
            std::size_t depth,
            double){
            depths[id] = depth;
            parents[id] = parent;
            return CGraph::EGraphTraversal::Continue;
        });

    ASSERT_EQ(depths.size(), 9U);
    EXPECT_EQ(depths.at(start), 0U);
    EXPECT_EQ(parents.at(start), CGraph::VertexIDInvalid);

    for(const auto& [id, depth] : depths){
        if(id == start){
            continue;
        }

        const auto parent = parents.at(id);

        ASSERT_NE(parent, CGraph::VertexIDInvalid);
        ASSERT_TRUE(depths.contains(parent));
        EXPECT_EQ(depths.at(parent) + 1, depth);
        EXPECT_TRUE(Graph->EdgeExists(parent, id));
    }
}

TYPED_TEST(GridGraphInterfaceTest, BreadthFirstSearchSkipNeighbors){
    auto Graph = TestFixture::CreateGrid(3, 3);
    ASSERT_NE(Graph, nullptr);

    std::set<CGraph::TVertexID> visited;

    Graph->BreadthFirstSearchUnweighted(
        0,
        [&visited](CGraph::TVertexID id,
                   CGraph::TVertexID,
                   std::size_t,
                   double){
            visited.insert(id);

            if(id == 0){
                return CGraph::EGraphTraversal::SkipNeighbors;
            }

            return CGraph::EGraphTraversal::Continue;
        });

    EXPECT_EQ(visited, (std::set<CGraph::TVertexID>{0}));
}

TYPED_TEST(GridGraphInterfaceTest, BreadthFirstSearchStopStopsTraversal){
    auto Graph = TestFixture::CreateGrid(3, 3);
    ASSERT_NE(Graph, nullptr);

    std::size_t visits = 0;

    Graph->BreadthFirstSearchUnweighted(
        0,
        [&visits](CGraph::TVertexID,
                  CGraph::TVertexID,
                  std::size_t,
                  double){
            ++visits;
            return CGraph::EGraphTraversal::Stop;
        });

    EXPECT_EQ(visits, 1U);
}

// -----------------------------------------------------------------------------
// Depth-first traversal
// -----------------------------------------------------------------------------

TYPED_TEST(GridGraphInterfaceTest, DepthFirstSearchVisitsReachableVerticesOnce){
    auto Graph = TestFixture::CreateGrid(3, 3);
    ASSERT_NE(Graph, nullptr);

    std::vector<CGraph::TVertexID> visited;

    Graph->DepthFirstSearch(
        0,
        [&visited](CGraph::TVertexID id,
                   CGraph::TVertexID,
                   std::size_t,
                   double){
            visited.push_back(id);
            return CGraph::EGraphTraversal::Continue;
        });

    EXPECT_EQ(visited.size(), 9U);
    EXPECT_EQ(
        std::set<CGraph::TVertexID>(visited.begin(), visited.end()).size(),
        9U);
}

TYPED_TEST(GridGraphInterfaceTest, DepthFirstSearchStopStopsTraversal){
    auto Graph = TestFixture::CreateGrid(3, 3);
    ASSERT_NE(Graph, nullptr);

    std::size_t visits = 0;

    Graph->DepthFirstSearch(
        0,
        [&visits](CGraph::TVertexID,
                  CGraph::TVertexID,
                  std::size_t,
                  double){
            ++visits;
            return CGraph::EGraphTraversal::Stop;
        });

    EXPECT_EQ(visits, 1U);
}

// -----------------------------------------------------------------------------
// Weighted breadth-first traversal
// -----------------------------------------------------------------------------

TYPED_TEST(GridGraphInterfaceTest, WeightedSearchReportsDistances){
    auto Graph = TestFixture::CreateGrid(3, 3);
    ASSERT_NE(Graph, nullptr);

    auto* Grid = TestFixture::Grid(Graph.get());
    ASSERT_NE(Grid, nullptr);

    const auto start = Grid->GetVertex(0, 0);
    const auto target = Grid->GetVertex(2, 1);

    ASSERT_NE(start, CGraph::VertexIDInvalid);
    ASSERT_NE(target, CGraph::VertexIDInvalid);

    std::unordered_map<CGraph::TVertexID, double> distances;

    const bool success = Graph->BreadthFirstSearchWeighted(
        start,
        [&distances](
            CGraph::TVertexID id,
            CGraph::TVertexID,
            std::size_t,
            double distance){
            distances[id] = distance;
            return CGraph::EGraphTraversal::Continue;
        });

    EXPECT_TRUE(success);
    ASSERT_TRUE(distances.contains(start));
    ASSERT_TRUE(distances.contains(target));

    EXPECT_DOUBLE_EQ(distances.at(start), 0.0);
    EXPECT_NEAR(distances.at(target), 1.0 + std::sqrt(2.0), 1e-10);
}

TYPED_TEST(GridGraphInterfaceTest, WeightedSearchUsesDiagonalWeights){
    auto Graph = TestFixture::CreateGrid(2, 2);
    ASSERT_NE(Graph, nullptr);

    auto* Grid = TestFixture::Grid(Graph.get());
    ASSERT_NE(Grid, nullptr);

    const auto start = Grid->GetVertex(0, 0);
    const auto diagonal = Grid->GetVertex(1, 1);

    ASSERT_NE(start, CGraph::VertexIDInvalid);
    ASSERT_NE(diagonal, CGraph::VertexIDInvalid);

    std::unordered_map<CGraph::TVertexID, double> distances;

    const bool success = Graph->BreadthFirstSearchWeighted(
        start,
        [&distances](
            CGraph::TVertexID id,
            CGraph::TVertexID,
            std::size_t,
            double distance){
            distances[id] = distance;
            return CGraph::EGraphTraversal::Continue;
        });

    EXPECT_TRUE(success);
    ASSERT_TRUE(distances.contains(diagonal));
    EXPECT_NEAR(distances.at(diagonal), std::sqrt(2.0), 1e-10);
}

// -----------------------------------------------------------------------------
// ForEach
// -----------------------------------------------------------------------------

TYPED_TEST(GridGraphInterfaceTest, ForEachVisitsEveryVertexOnce){
    auto Graph = TestFixture::CreateGrid(4, 3);
    ASSERT_NE(Graph, nullptr);

    std::set<CGraph::TVertexID> visited;

    const bool completed = Graph->ForEach(
        [&visited](CGraph::TVertexID id,
                   CGraph::TVertexID,
                   std::size_t,
                   double){
            visited.insert(id);
            return CGraph::EGraphTraversal::Continue;
        });

    EXPECT_TRUE(completed);
    EXPECT_EQ(visited.size(), 12U);
}

TYPED_TEST(GridGraphInterfaceTest, ForEachStopReturnsFalse){
    auto Graph = TestFixture::CreateGrid(4, 3);
    ASSERT_NE(Graph, nullptr);

    std::size_t visits = 0;

    const bool completed = Graph->ForEach(
        [&visits](CGraph::TVertexID,
                  CGraph::TVertexID,
                  std::size_t,
                  double){
            ++visits;
            return CGraph::EGraphTraversal::Stop;
        });

    EXPECT_FALSE(completed);
    EXPECT_EQ(visits, 1U);
}