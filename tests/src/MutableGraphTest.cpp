#include "MutableGraph.h"
#include "GraphFactory.h"
#include <gtest/gtest.h>

#include <cmath>
#include <memory>
#include <tuple>
#include <vector>

using TGraphVertexID = CGraph::TVertexID;
using TGraphVertexTag = CGraph::TVertexTag;
using TGraphEdge = std::tuple<TGraphVertexID, TGraphVertexID, double, bool>;

template <typename TGraph>
struct SGraphFactory;

// Specialize SGraphFactory<TGraph> separately for each implementation.
// Create(tags, edges) constructs the graph with tags.size() vertices.
template <typename TGraph>
class MutableGraphInterfaceTest : public ::testing::Test {
protected:
    static std::unique_ptr<CGraph> CreateGraph(
        const std::vector<TGraphVertexTag>& tags = {},
        const std::vector<TGraphEdge>& edges = {}){
        return SGraphFactory<TGraph>::Create(tags, edges);
    }

    static CVertexMutableGraph* VertexMutable(CGraph* graph){
        return dynamic_cast<CVertexMutableGraph*>(graph);
    }

    static CVertexTagMutableGraph* TagMutable(CGraph* graph){
        return dynamic_cast<CVertexTagMutableGraph*>(graph);
    }

    static CEdgeMutableGraph* EdgeMutable(CGraph* graph){
        return dynamic_cast<CEdgeMutableGraph*>(graph);
    }

    static CColorableMutableGraph* Colorable(CGraph* graph){
        return dynamic_cast<CColorableMutableGraph*>(graph);
    }

    static void VerifyColoring(
        CGraph* graph,
        std::size_t maxcolors){

        std::vector<TGraphVertexID> vertices;
        for(TGraphVertexID VertexID = 0; VertexID < graph->VertexCount(); VertexID++){
            vertices.push_back(VertexID);
        }
        

        for(auto vertex : vertices){
            TGraphVertexTag tag = 0;
            ASSERT_TRUE(graph->GetVertexTag(vertex, tag));
            EXPECT_LT(tag, maxcolors);

            std::vector<TGraphVertexID> neighbors;
            ASSERT_TRUE(graph->GetVertexNeighbors(vertex, neighbors));

            for(auto neighbor : neighbors){
                TGraphVertexTag neighborTag = 0;
                ASSERT_TRUE(graph->GetVertexTag(neighbor, neighborTag));
                EXPECT_NE(tag, neighborTag)
                    << "Adjacent vertices " << vertex
                    << " and " << neighbor
                    << " have the same color";
            }
        }
    }
};



// Replace CYourGraphImplementation with your concrete implementation types.
using MutableGraphImplementations =
    ::testing::Types<CAdjacencyListGraph>;

TYPED_TEST_SUITE(
    MutableGraphInterfaceTest,
    MutableGraphImplementations);

// -----------------------------------------------------------------------------
// CVertexMutableGraph
// -----------------------------------------------------------------------------

TYPED_TEST(MutableGraphInterfaceTest, AddVertexIncreasesVertexCount){
    auto graph = TestFixture::CreateGraph();
    ASSERT_NE(graph, nullptr);

    auto* mutableGraph = TestFixture::VertexMutable(graph.get());
    ASSERT_NE(mutableGraph, nullptr);

    EXPECT_EQ(graph->VertexCount(), 0U);

    const auto first = mutableGraph->AddVertex(10);
    EXPECT_NE(first, CGraph::VertexIDInvalid);
    EXPECT_EQ(graph->VertexCount(), 1U);

    const auto second = mutableGraph->AddVertex(20);
    EXPECT_NE(second, CGraph::VertexIDInvalid);
    EXPECT_NE(first, second);
    EXPECT_EQ(graph->VertexCount(), 2U);

    CGraph::TVertexTag tag = 0;
    EXPECT_TRUE(graph->GetVertexTag(first, tag));
    EXPECT_EQ(tag, 10);

    EXPECT_TRUE(graph->GetVertexTag(second, tag));
    EXPECT_EQ(tag, 20);
}

TYPED_TEST(MutableGraphInterfaceTest, RemoveVertexDecreasesVertexCount){
    auto graph = TestFixture::CreateGraph({10, 20, 30});
    ASSERT_NE(graph, nullptr);

    auto* mutableGraph = TestFixture::VertexMutable(graph.get());
    ASSERT_NE(mutableGraph, nullptr);

    ASSERT_EQ(graph->VertexCount(), 3U);

    EXPECT_TRUE(mutableGraph->RemoveVertex(1));
    EXPECT_EQ(graph->VertexCount(), 2U);

    CGraph::TVertexTag tag = 0;
    EXPECT_FALSE(graph->GetVertexTag(1, tag));
}

TYPED_TEST(MutableGraphInterfaceTest, RemoveVertexRemovesIncidentEdges){
    const std::vector<TGraphEdge> edges = {
        {0, 1, 2.0, false},
        {1, 2, 3.0, false},
        {0, 2, 4.0, false}
    };

    auto graph = TestFixture::CreateGraph({10, 20, 30}, edges);
    ASSERT_NE(graph, nullptr);

    auto* mutableGraph = TestFixture::VertexMutable(graph.get());
    ASSERT_NE(mutableGraph, nullptr);

    ASSERT_TRUE(graph->EdgeExists(0, 1));
    ASSERT_TRUE(graph->EdgeExists(1, 2));
    ASSERT_TRUE(graph->EdgeExists(0, 2));

    ASSERT_TRUE(mutableGraph->RemoveVertex(1));

    EXPECT_FALSE(graph->EdgeExists(0, 1));
    EXPECT_FALSE(graph->EdgeExists(1, 2));
    EXPECT_TRUE(graph->EdgeExists(0, 2));
}

TYPED_TEST(MutableGraphInterfaceTest, RemoveInvalidVertexFails){
    auto graph = TestFixture::CreateGraph({10, 20});
    ASSERT_NE(graph, nullptr);

    auto* mutableGraph = TestFixture::VertexMutable(graph.get());
    ASSERT_NE(mutableGraph, nullptr);

    const auto countBefore = graph->VertexCount();

    EXPECT_FALSE(mutableGraph->RemoveVertex(CGraph::VertexIDInvalid));
    EXPECT_FALSE(mutableGraph->RemoveVertex(1000));
    EXPECT_EQ(graph->VertexCount(), countBefore);
}

// -----------------------------------------------------------------------------
// CVertexTagMutableGraph
// -----------------------------------------------------------------------------

TYPED_TEST(MutableGraphInterfaceTest, SetVertexTagUpdatesTag){
    auto graph = TestFixture::CreateGraph({10, 20});
    ASSERT_NE(graph, nullptr);

    auto* mutableGraph = TestFixture::TagMutable(graph.get());
    ASSERT_NE(mutableGraph, nullptr);

    ASSERT_EQ(graph->VertexCount(), 2U);

    EXPECT_TRUE(mutableGraph->SetVertexTag(0, 99));

    CGraph::TVertexTag tag = 0;
    ASSERT_TRUE(graph->GetVertexTag(0, tag));
    EXPECT_EQ(tag, 99);

    // Changing one tag must not change the other vertex's tag.
    ASSERT_TRUE(graph->GetVertexTag(1, tag));
    EXPECT_EQ(tag, 20);

    EXPECT_EQ(graph->VertexCount(), 2U);
}

TYPED_TEST(MutableGraphInterfaceTest, SetVertexTagAllowsSameTag){
    auto graph = TestFixture::CreateGraph({10});
    ASSERT_NE(graph, nullptr);

    auto* mutableGraph = TestFixture::TagMutable(graph.get());
    ASSERT_NE(mutableGraph, nullptr);

    EXPECT_TRUE(mutableGraph->SetVertexTag(0, 10));

    CGraph::TVertexTag tag = 0;
    ASSERT_TRUE(graph->GetVertexTag(0, tag));
    EXPECT_EQ(tag, 10);
}

TYPED_TEST(MutableGraphInterfaceTest, SetVertexTagInvalidVertexFails){
    auto graph = TestFixture::CreateGraph({10});
    ASSERT_NE(graph, nullptr);

    auto* mutableGraph = TestFixture::TagMutable(graph.get());
    ASSERT_NE(mutableGraph, nullptr);

    EXPECT_FALSE(
        mutableGraph->SetVertexTag(CGraph::VertexIDInvalid, 99));
    EXPECT_FALSE(mutableGraph->SetVertexTag(1000, 99));

    CGraph::TVertexTag tag = 0;
    ASSERT_TRUE(graph->GetVertexTag(0, tag));
    EXPECT_EQ(tag, 10);
}

// -----------------------------------------------------------------------------
// CEdgeMutableGraph
// -----------------------------------------------------------------------------

TYPED_TEST(MutableGraphInterfaceTest, AddDirectedEdge){
    auto graph = TestFixture::CreateGraph({10, 20});
    ASSERT_NE(graph, nullptr);

    auto* mutableGraph = TestFixture::EdgeMutable(graph.get());
    ASSERT_NE(mutableGraph, nullptr);

    EXPECT_FALSE(graph->EdgeExists(0, 1));

    EXPECT_TRUE(mutableGraph->AddEdge(0, 1, 2.5, false));

    EXPECT_TRUE(graph->EdgeExists(0, 1));
    EXPECT_FALSE(graph->EdgeExists(1, 0));

    double weight = 0.0;
    ASSERT_TRUE(graph->GetEdgeWeight(0, 1, weight));
    EXPECT_DOUBLE_EQ(weight, 2.5);

    std::vector<TGraphVertexID> neighbors;
    ASSERT_TRUE(graph->GetVertexNeighbors(0, neighbors));
    EXPECT_EQ(neighbors, (std::vector<TGraphVertexID>{1}));

    ASSERT_TRUE(graph->GetVertexNeighbors(1, neighbors));
    EXPECT_TRUE(neighbors.empty());
}

TYPED_TEST(MutableGraphInterfaceTest, AddBidirectionalEdge){
    auto graph = TestFixture::CreateGraph({10, 20});
    ASSERT_NE(graph, nullptr);

    auto* mutableGraph = TestFixture::EdgeMutable(graph.get());
    ASSERT_NE(mutableGraph, nullptr);

    EXPECT_TRUE(mutableGraph->AddEdge(0, 1, 3.5, true));

    EXPECT_TRUE(graph->EdgeExists(0, 1));
    EXPECT_TRUE(graph->EdgeExists(1, 0));

    double weight = 0.0;
    ASSERT_TRUE(graph->GetEdgeWeight(0, 1, weight));
    EXPECT_DOUBLE_EQ(weight, 3.5);

    ASSERT_TRUE(graph->GetEdgeWeight(1, 0, weight));
    EXPECT_DOUBLE_EQ(weight, 3.5);
}

TYPED_TEST(MutableGraphInterfaceTest, AddEdgeWithInvalidVertexFails){
    auto graph = TestFixture::CreateGraph({10, 20});
    ASSERT_NE(graph, nullptr);

    auto* mutableGraph = TestFixture::EdgeMutable(graph.get());
    ASSERT_NE(mutableGraph, nullptr);

    EXPECT_FALSE(mutableGraph->AddEdge(1000, 1, 1.0, false));
    EXPECT_FALSE(mutableGraph->AddEdge(0, 1000, 1.0, false));
    EXPECT_FALSE(mutableGraph->AddEdge(
        CGraph::VertexIDInvalid, 1, 1.0, false));
}

TYPED_TEST(MutableGraphInterfaceTest, RemoveDirectedEdge){
    const std::vector<TGraphEdge> edges = {
        {0, 1, 2.0, false},
        {1, 0, 3.0, false},
        {1, 2, 4.0, false}
    };

    auto graph = TestFixture::CreateGraph({10, 20, 30}, edges);
    ASSERT_NE(graph, nullptr);

    auto* mutableGraph = TestFixture::EdgeMutable(graph.get());
    ASSERT_NE(mutableGraph, nullptr);

    EXPECT_TRUE(mutableGraph->RemoveEdge(0, 1));

    EXPECT_FALSE(graph->EdgeExists(0, 1));
    EXPECT_TRUE(graph->EdgeExists(1, 0));
    EXPECT_TRUE(graph->EdgeExists(1, 2));
}

TYPED_TEST(MutableGraphInterfaceTest, RemoveNonexistentEdgeFails){
    auto graph = TestFixture::CreateGraph({10, 20});
    ASSERT_NE(graph, nullptr);

    auto* mutableGraph = TestFixture::EdgeMutable(graph.get());
    ASSERT_NE(mutableGraph, nullptr);

    EXPECT_FALSE(mutableGraph->RemoveEdge(0, 1));
    EXPECT_FALSE(mutableGraph->RemoveEdge(1000, 1));
}

TYPED_TEST(MutableGraphInterfaceTest, SetEdgeWeightUpdatesWeight){
    const std::vector<TGraphEdge> edges = {
        {0, 1, 2.0, false}
    };

    auto graph = TestFixture::CreateGraph({10, 20}, edges);
    ASSERT_NE(graph, nullptr);

    auto* mutableGraph = TestFixture::EdgeMutable(graph.get());
    ASSERT_NE(mutableGraph, nullptr);

    EXPECT_TRUE(mutableGraph->SetEdgeWeight(0, 1, 7.25));

    double weight = 0.0;
    ASSERT_TRUE(graph->GetEdgeWeight(0, 1, weight));
    EXPECT_DOUBLE_EQ(weight, 7.25);
}

TYPED_TEST(MutableGraphInterfaceTest, SetEdgeWeightInvalidEdgeFails){
    auto graph = TestFixture::CreateGraph({10, 20});
    ASSERT_NE(graph, nullptr);

    auto* mutableGraph = TestFixture::EdgeMutable(graph.get());
    ASSERT_NE(mutableGraph, nullptr);

    EXPECT_FALSE(mutableGraph->SetEdgeWeight(0, 1, 5.0));
    EXPECT_FALSE(mutableGraph->SetEdgeWeight(1000, 1, 5.0));
}

TYPED_TEST(MutableGraphInterfaceTest, SetEdgeWeightDoesNotChangeEdgeCount){
    const std::vector<TGraphEdge> edges = {
        {0, 1, 2.0, false}
    };

    auto graph = TestFixture::CreateGraph({10, 20}, edges);
    ASSERT_NE(graph, nullptr);

    auto* mutableGraph = TestFixture::EdgeMutable(graph.get());
    ASSERT_NE(mutableGraph, nullptr);

    ASSERT_TRUE(mutableGraph->SetEdgeWeight(0, 1, 9.0));

    EXPECT_TRUE(graph->EdgeExists(0, 1));
}

// -----------------------------------------------------------------------------
// CColorableMutableGraph
// -----------------------------------------------------------------------------

TYPED_TEST(MutableGraphInterfaceTest, ColorVerticesSingleVertex){
    auto graph = TestFixture::CreateGraph({99});
    ASSERT_NE(graph, nullptr);

    auto* colorable = TestFixture::Colorable(graph.get());
    ASSERT_NE(colorable, nullptr);

    EXPECT_TRUE(colorable->ColorVertices(1));
    auto DefaultTag = std::numeric_limits<TGraphVertexTag>::max();
    CGraph::TVertexTag tag = DefaultTag;
    ASSERT_TRUE(graph->GetVertexTag(0, tag));
    EXPECT_EQ(tag, 0);
}

TYPED_TEST(MutableGraphInterfaceTest, ColorVerticesSimpleEdge){
    const std::vector<TGraphEdge> edges = {
        {0, 1, 1.0, true}
    };
    auto DefaultTag = std::numeric_limits<TGraphVertexTag>::max();
    auto graph = TestFixture::CreateGraph({DefaultTag,DefaultTag}, edges);
    ASSERT_NE(graph, nullptr);

    auto* colorable = TestFixture::Colorable(graph.get());
    ASSERT_NE(colorable, nullptr);

    EXPECT_TRUE(colorable->ColorVertices(2));

    TestFixture::VerifyColoring(graph.get(), 2);
}

TYPED_TEST(MutableGraphInterfaceTest, ColorVerticesTriangle){
    const std::vector<TGraphEdge> edges = {
        {0, 1, 1.0, true},
        {1, 2, 1.0, true},
        {2, 0, 1.0, true}
    };
    auto DefaultTag = std::numeric_limits<TGraphVertexTag>::max();
    auto graph = TestFixture::CreateGraph({DefaultTag, DefaultTag, DefaultTag}, edges);
    ASSERT_NE(graph, nullptr);

    auto* colorable = TestFixture::Colorable(graph.get());
    ASSERT_NE(colorable, nullptr);

    EXPECT_TRUE(colorable->ColorVertices(3));

    TestFixture::VerifyColoring(graph.get(), 3);
}

TYPED_TEST(MutableGraphInterfaceTest, ColorVerticesTriangleWithInsufficientColors){
    const std::vector<TGraphEdge> edges = {
        {0, 1, 1.0, true},
        {1, 2, 1.0, true},
        {2, 0, 1.0, true}
    };
    auto DefaultTag = std::numeric_limits<TGraphVertexTag>::max();
    auto graph = TestFixture::CreateGraph({DefaultTag, DefaultTag, DefaultTag}, edges);
    ASSERT_NE(graph, nullptr);

    auto* colorable = TestFixture::Colorable(graph.get());
    ASSERT_NE(colorable, nullptr);

    EXPECT_FALSE(colorable->ColorVertices(2));
}

TYPED_TEST(MutableGraphInterfaceTest, ColorVerticesZeroColors){
    auto graph = TestFixture::CreateGraph({std::numeric_limits<TGraphVertexTag>::max()});
    ASSERT_NE(graph, nullptr);

    auto* colorable = TestFixture::Colorable(graph.get());
    ASSERT_NE(colorable, nullptr);

    EXPECT_FALSE(colorable->ColorVertices(0));
}

TYPED_TEST(MutableGraphInterfaceTest, ColorVerticesDisconnectedGraph){
    const std::vector<TGraphEdge> edges = {
        {0, 1, 1.0, true},
        {2, 3, 1.0, true}
    };

    auto DefaultTag = std::numeric_limits<TGraphVertexTag>::max();
    auto graph = TestFixture::CreateGraph(
        {DefaultTag, DefaultTag, DefaultTag, DefaultTag}, edges);
    ASSERT_NE(graph, nullptr);

    auto* colorable = TestFixture::Colorable(graph.get());
    ASSERT_NE(colorable, nullptr);

    EXPECT_TRUE(colorable->ColorVertices(2));

    TestFixture::VerifyColoring(graph.get(), 2);
}

TYPED_TEST(MutableGraphInterfaceTest, ColorVerticesStartingVertexChangesOrder){
    const std::vector<TGraphEdge> edges = {
        {0, 1, 1.0, true},
        {1, 2, 1.0, true},
        {2, 3, 1.0, true}
    };

    auto DefaultTag = std::numeric_limits<TGraphVertexTag>::max();
    auto graph = TestFixture::CreateGraph(
        {DefaultTag, DefaultTag, DefaultTag, DefaultTag}, edges);
    ASSERT_NE(graph, nullptr);

    auto* colorable = TestFixture::Colorable(graph.get());
    ASSERT_NE(colorable, nullptr);

    EXPECT_TRUE(colorable->ColorVertices(2));

    TestFixture::VerifyColoring(graph.get(), 2);
}