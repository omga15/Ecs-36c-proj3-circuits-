#include <gtest/gtest.h>
#include "Graph.h"
#include "GraphFactory.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// -----------------------------------------------------------------------------
// Shared types and factory declaration
// Define SGraphFactory specializations separately.
// -----------------------------------------------------------------------------

using TGraphVertexID = CGraph::TVertexID;
using TGraphVertexTag = CGraph::TVertexTag;


template <typename T>
struct SGraphFactory;

// -----------------------------------------------------------------------------
// Typed-test fixture
// -----------------------------------------------------------------------------

template <typename T>
class GraphInterfaceTest : public ::testing::Test {
protected:
    static std::unique_ptr<CGraph> CreateGraph(const std::vector<CGraph::TVertexTag>& tags,
        const std::vector<TGraphEdge>& edges = {}){
        return SGraphFactory<T>::Create(tags, edges);
    }
};

// Replace with your concrete graph implementation(s).
using GraphImplementations = ::testing::Types<
    CAdjacencyListGraph
>;

TYPED_TEST_SUITE(GraphInterfaceTest, GraphImplementations);

// -----------------------------------------------------------------------------
// Basic graph tests
// -----------------------------------------------------------------------------

TYPED_TEST(GraphInterfaceTest, EmptyGraph){
    auto graph = TestFixture::CreateGraph({});

    ASSERT_NE(graph, nullptr);
    EXPECT_EQ(graph->VertexCount(), 0U);
}

TYPED_TEST(GraphInterfaceTest, GraphWithIsolatedVertices){
    const std::vector<TGraphVertexTag> tags = {10, 20, 30, 40, 50};
    auto graph = TestFixture::CreateGraph(tags);

    ASSERT_NE(graph, nullptr);
    EXPECT_EQ(graph->VertexCount(), tags.size());

    for(TGraphVertexID id = 0; id < tags.size(); ++id){
        std::vector<TGraphVertexID> neighbors;

        ASSERT_TRUE(graph->GetVertexNeighbors(id, neighbors));
        EXPECT_TRUE(neighbors.empty());
    }
}

// -----------------------------------------------------------------------------
// Vertex tag tests
// -----------------------------------------------------------------------------

TYPED_TEST(GraphInterfaceTest, VertexTagsMatchCreationVector){
    const std::vector<TGraphVertexTag> tags = {100, 200, 300, 400};
    auto graph = TestFixture::CreateGraph(tags);

    ASSERT_NE(graph, nullptr);
    ASSERT_EQ(graph->VertexCount(), tags.size());

    for(TGraphVertexID id = 0; id < tags.size(); ++id){
        TGraphVertexTag actual = 0;

        ASSERT_TRUE(graph->GetVertexTag(id, actual));
        EXPECT_EQ(actual, tags[id]);
    }
}

// -----------------------------------------------------------------------------
// Edge tests
// -----------------------------------------------------------------------------

TYPED_TEST(GraphInterfaceTest, DirectedEdgeQueriesAndWeights){
    const std::vector<TGraphVertexTag> tags = {10, 20, 30};
    const std::vector<TGraphEdge> edges = {
        {0, 1, 2.5, true},
        {1, 2, 4.0, true}
    };
    auto graph = TestFixture::CreateGraph(tags, edges);
    ASSERT_NE(graph, nullptr);
    EXPECT_EQ(graph->VertexCount(), 3U);
    EXPECT_TRUE(graph->EdgeExists(0, 1));
    EXPECT_TRUE(graph->EdgeExists(1, 2));
    EXPECT_TRUE(graph->EdgeExists(1, 0));
    EXPECT_FALSE(graph->EdgeExists(0, 2));
    double weight = CGraph::EdgeWeightInvalid;

    ASSERT_TRUE(graph->GetEdgeWeight(0, 1, weight));
    EXPECT_DOUBLE_EQ(weight, 2.5);

    ASSERT_TRUE(graph->GetEdgeWeight(1, 2, weight));
    EXPECT_DOUBLE_EQ(weight, 4.0);

    EXPECT_FALSE(graph->GetEdgeWeight(0, 2, weight));
}

TYPED_TEST(GraphInterfaceTest, NeighborQueriesReturnAdjacentVertices){
    const std::vector<TGraphVertexTag> tags = {10, 20, 30, 40};
    const std::vector<TGraphEdge> edges = {
        {0, 1, 1.0, true},
        {0, 2, 1.0, true},
        {1, 3, 1.0, true}
    };

    auto graph = TestFixture::CreateGraph(tags, edges);
    ASSERT_NE(graph, nullptr);

    std::vector<TGraphVertexID> neighbors;
    ASSERT_TRUE(graph->GetVertexNeighbors(0, neighbors));

    std::sort(neighbors.begin(), neighbors.end());
    EXPECT_EQ(neighbors, (std::vector<TGraphVertexID>{1, 2}));

    neighbors.clear();
    ASSERT_TRUE(graph->GetVertexNeighbors(1, neighbors));
    EXPECT_EQ(neighbors, (std::vector<TGraphVertexID>{0,3}));
}

// -----------------------------------------------------------------------------
// Unweighted breadth-first search
// -----------------------------------------------------------------------------

TYPED_TEST(GraphInterfaceTest, BreadthFirstSearchUnweightedReportsDepthAndParent){
    const std::vector<TGraphVertexTag> tags = {10, 20, 30, 40};
    const std::vector<TGraphEdge> edges = {
        {0, 1, 1.0, true},
        {0, 2, 1.0, true},
        {1, 3, 1.0, true}
    };

    auto graph = TestFixture::CreateGraph(tags, edges);
    ASSERT_NE(graph, nullptr);

    struct SVisit {
        TGraphVertexID parent;
        std::size_t depth;
        double distance;
    };

    std::unordered_map<TGraphVertexID, SVisit> visits;

    graph->BreadthFirstSearchUnweighted(
        0,
        [&](TGraphVertexID vertex, TGraphVertexID parent,
            std::size_t depth, double distance){
            EXPECT_EQ(visits.count(vertex), 0U);
            visits.emplace(vertex, SVisit{parent, depth, distance});
            return CGraph::EGraphTraversal::Continue;
        });

    ASSERT_EQ(visits.size(), 4U);

    ASSERT_EQ(visits.count(0), 1U);
    EXPECT_EQ(visits.at(0).parent, CGraph::VertexIDInvalid);
    EXPECT_EQ(visits.at(0).depth, 0U);
    EXPECT_TRUE(std::isnan(visits.at(0).distance));

    ASSERT_EQ(visits.count(1), 1U);
    EXPECT_EQ(visits.at(1).parent, 0U);
    EXPECT_EQ(visits.at(1).depth, 1U);
    EXPECT_TRUE(std::isnan(visits.at(1).distance));

    ASSERT_EQ(visits.count(2), 1U);
    EXPECT_EQ(visits.at(2).parent, 0U);
    EXPECT_EQ(visits.at(2).depth, 1U);
    EXPECT_TRUE(std::isnan(visits.at(2).distance));

    ASSERT_EQ(visits.count(3), 1U);
    EXPECT_EQ(visits.at(3).parent, 1U);
    EXPECT_EQ(visits.at(3).depth, 2U);
    EXPECT_TRUE(std::isnan(visits.at(3).distance));
}

TYPED_TEST(GraphInterfaceTest, BreadthFirstSearchUnweightedSkipNeighbors){
    const std::vector<TGraphVertexTag> tags = {10, 20, 30, 40};
    const std::vector<TGraphEdge> edges = {
        {0, 1, 1.0, true},
        {1, 2, 1.0, true},
        {0, 3, 1.0, true}
    };

    auto graph = TestFixture::CreateGraph(tags, edges);
    ASSERT_NE(graph, nullptr);

    std::unordered_set<TGraphVertexID> visited;

    graph->BreadthFirstSearchUnweighted(
        0,
        [&](TGraphVertexID vertex, TGraphVertexID,
            std::size_t, double){
            visited.insert(vertex);

            if(vertex == 1){
                return CGraph::EGraphTraversal::SkipNeighbors;
            }

            return CGraph::EGraphTraversal::Continue;
        });

    EXPECT_EQ(visited.count(0), 1U);
    EXPECT_EQ(visited.count(1), 1U);
    EXPECT_EQ(visited.count(3), 1U);
    EXPECT_EQ(visited.count(2), 0U);
}

TYPED_TEST(GraphInterfaceTest, BreadthFirstSearchUnweightedStopTerminates){
    const std::vector<TGraphVertexTag> tags = {10, 20, 30};
    const std::vector<TGraphEdge> edges = {
        {0, 1, 1.0, true},
        {0, 2, 1.0, true}
    };

    auto graph = TestFixture::CreateGraph(tags, edges);
    ASSERT_NE(graph, nullptr);

    std::size_t callbackCount = 0;

    graph->BreadthFirstSearchUnweighted(
        0,
        [&](TGraphVertexID, TGraphVertexID, std::size_t, double){
            ++callbackCount;
            return CGraph::EGraphTraversal::Stop;
        });

    EXPECT_EQ(callbackCount, 1U);
}

// -----------------------------------------------------------------------------
// Depth-first search
// -----------------------------------------------------------------------------

TYPED_TEST(GraphInterfaceTest, DepthFirstSearchVisitsReachableVerticesOnce){
    const std::vector<TGraphVertexTag> tags = {10, 20, 30, 40, 50};
    const std::vector<TGraphEdge> edges = {
        {0, 1, 1.0, true},
        {0, 2, 1.0, true},
        {1, 3, 1.0, true}
    };

    auto graph = TestFixture::CreateGraph(tags, edges);
    ASSERT_NE(graph, nullptr);

    std::vector<TGraphVertexID> visited;

    graph->DepthFirstSearch(
        0,
        [&](TGraphVertexID vertex, TGraphVertexID,
            std::size_t, double){
            visited.push_back(vertex);
            return CGraph::EGraphTraversal::Continue;
        });

    std::sort(visited.begin(), visited.end());
    EXPECT_EQ(visited, (std::vector<TGraphVertexID>{0, 1, 2, 3}));
}

TYPED_TEST(GraphInterfaceTest, DepthFirstSearchStopTerminates){
    const std::vector<TGraphVertexTag> tags = {10, 20, 30};
    const std::vector<TGraphEdge> edges = {
        {0, 1, 1.0, true},
        {0, 2, 1.0, true}
    };

    auto graph = TestFixture::CreateGraph(tags, edges);
    ASSERT_NE(graph, nullptr);

    std::size_t callbackCount = 0;

    graph->DepthFirstSearch(
        0,
        [&](TGraphVertexID, TGraphVertexID, std::size_t, double){
            ++callbackCount;
            return CGraph::EGraphTraversal::Stop;
        });

    EXPECT_EQ(callbackCount, 1U);
}

// -----------------------------------------------------------------------------
// Weighted breadth-first search
// Assumes this operation computes shortest paths for nonnegative edge weights.
// -----------------------------------------------------------------------------

TYPED_TEST(GraphInterfaceTest, BreadthFirstSearchWeightedReportsShortestDistances){
    const std::vector<TGraphVertexTag> tags = {10, 20, 30, 40};
    const std::vector<TGraphEdge> edges = {
        {0, 1, 5.0, true},
        {0, 2, 1.0, true},
        {2, 1, 1.0, true},
        {1, 3, 1.0, true},
        {2, 3, 10.0, true}
    };

    auto graph = TestFixture::CreateGraph(tags, edges);
    ASSERT_NE(graph, nullptr);

    std::unordered_map<TGraphVertexID, double> distances;

    const bool success = graph->BreadthFirstSearchWeighted(
        0,
        [&](TGraphVertexID vertex, TGraphVertexID,
            std::size_t, double distance){
            distances[vertex] = distance;
            return CGraph::EGraphTraversal::Continue;
        });

    ASSERT_TRUE(success);
    ASSERT_EQ(distances.size(), 4U);
    EXPECT_DOUBLE_EQ(distances.at(0), 0.0);
    EXPECT_DOUBLE_EQ(distances.at(2), 1.0);
    EXPECT_DOUBLE_EQ(distances.at(1), 2.0);
    EXPECT_DOUBLE_EQ(distances.at(3), 3.0);
}


TYPED_TEST(GraphInterfaceTest, BreadthFirstSearchWeightedFailsOnNegativeEdgeWeight)
{
    const std::vector<TGraphVertexTag> tags = {10, 20, 30};
    const std::vector<TGraphEdge> edges = {
        {0, 1, 2.0, true},
        {1, 2, -1.0, true}
    };

    auto graph = TestFixture::CreateGraph(tags, edges);
    ASSERT_NE(graph, nullptr);

    const bool success = graph->BreadthFirstSearchWeighted(
        0,
        [](TGraphVertexID, TGraphVertexID, std::size_t, double){
            return CGraph::EGraphTraversal::Continue;
        });

    EXPECT_FALSE(success);
}


// -----------------------------------------------------------------------------
// ForEach
// Assumes ForEach visits all vertices, including disconnected ones.
// Assumes Stop makes ForEach return false.
// -----------------------------------------------------------------------------

TYPED_TEST(GraphInterfaceTest, ForEachVisitsEveryVertexIncludingDisconnectedOnes){
    const std::vector<TGraphVertexTag> tags = {10, 20, 30, 40, 50};
    const std::vector<TGraphEdge> edges = {
        {0, 1, 1.0, true},
        {2, 3, 1.0, true}
    };

    auto graph = TestFixture::CreateGraph(tags, edges);
    ASSERT_NE(graph, nullptr);

    std::vector<TGraphVertexID> visited;

    const bool success = graph->ForEach(
        [&](TGraphVertexID vertex, TGraphVertexID,
            std::size_t, double){
            visited.push_back(vertex);
            return CGraph::EGraphTraversal::Continue;
        });

    EXPECT_TRUE(success);

    std::sort(visited.begin(), visited.end());
    EXPECT_EQ(visited, (std::vector<TGraphVertexID>{0, 1, 2, 3, 4}));
}

TYPED_TEST(GraphInterfaceTest, ForEachStopReturnsFailure){
    const std::vector<TGraphVertexTag> tags = {10, 20, 30};
    auto graph = TestFixture::CreateGraph(tags);
    ASSERT_NE(graph, nullptr);

    std::size_t callbackCount = 0;

    const bool success = graph->ForEach(
        [&](TGraphVertexID, TGraphVertexID, std::size_t, double){
            ++callbackCount;
            return CGraph::EGraphTraversal::Stop;
        });

    EXPECT_FALSE(success);
    EXPECT_EQ(callbackCount, 1U);
}

