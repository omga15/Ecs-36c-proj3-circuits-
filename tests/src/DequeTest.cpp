#include <gtest/gtest.h>
#include "Deque.h"
#include "DequeFactory.h"
#include <iostream>

template <typename T>
class DequeTest : public ::testing::Test {
    protected:
        std::unique_ptr<CDeque> DDeque = SDequeFactory<T>::Create();
};


using GDequeImplementations = ::testing::Types<CMaxSizeDeque, CVariableSizeDeque>;


TYPED_TEST_SUITE(DequeTest, GDequeImplementations);

TYPED_TEST(DequeTest, EmptyTest) {
    EXPECT_EQ(this->DDeque->Size(),0);
    EXPECT_GT(this->DDeque->MaxSize(),0);
}

TYPED_TEST(DequeTest, SimplePushPop){
    EXPECT_EQ(this->DDeque->Size(),0);
    EXPECT_TRUE(this->DDeque->PushBack(7));
    EXPECT_EQ(this->DDeque->Size(),1);
    EXPECT_EQ(std::any_cast<int>(this->DDeque->Back()),7);
    EXPECT_TRUE(this->DDeque->PopBack());
    EXPECT_EQ(this->DDeque->Size(),0);
    EXPECT_TRUE(this->DDeque->PushFront(34));
    EXPECT_EQ(this->DDeque->Size(),1);
    EXPECT_EQ(std::any_cast<int>(this->DDeque->Front()),34);
    EXPECT_TRUE(this->DDeque->PopFront());
    EXPECT_EQ(this->DDeque->Size(),0);
}

TYPED_TEST(DequeTest, FlowThrough){
    TDequeSize FlowSize = this->DDeque->MaxSize() && (this->DDeque->MaxSize() != GDequeSizeVariable) ? this->DDeque->MaxSize() - 2 : 1024;
    for(TDequeSize Index = 0; Index < FlowSize; Index++){
        EXPECT_EQ(this->DDeque->Size(),Index);
        EXPECT_TRUE(this->DDeque->PushBack(Index));
    }
    for(TDequeSize Index = 0; Index < FlowSize; Index++){
        ASSERT_EQ(this->DDeque->Size(),FlowSize - Index);
            EXPECT_EQ(std::any_cast<TDequeSize>(this->DDeque->Front()), Index);
            EXPECT_TRUE(this->DDeque->PopFront());
        }
    EXPECT_EQ(this->DDeque->Size(),0);
    for(TDequeSize Index = 0; Index < FlowSize; Index++){
        EXPECT_EQ(this->DDeque->Size(),Index);
        EXPECT_TRUE(this->DDeque->PushFront(Index));
    }
    for(TDequeSize Index = 0; Index < FlowSize; Index++){
        EXPECT_EQ(this->DDeque->Size(),FlowSize - Index);
        EXPECT_EQ(std::any_cast<TDequeSize>(this->DDeque->Back()), Index);
        EXPECT_TRUE(this->DDeque->PopBack());
    }
    EXPECT_EQ(this->DDeque->Size(),0);
}

TYPED_TEST(DequeTest, LimitsTest){
    EXPECT_FALSE(this->DDeque->PopBack());
    EXPECT_FALSE(this->DDeque->PopFront());
    if(this->DDeque->MaxSize() != GDequeSizeVariable){
        TDequeSize ExpectedSize = 0;
        while(this->DDeque->Size() < this->DDeque->MaxSize()){
            EXPECT_EQ(this->DDeque->Size(),ExpectedSize);
            EXPECT_TRUE(this->DDeque->PushBack(ExpectedSize));
            ExpectedSize++;
        }
        EXPECT_FALSE(this->DDeque->PushBack(ExpectedSize));
        EXPECT_FALSE(this->DDeque->PushFront(ExpectedSize));
        while(this->DDeque->Size()){
            EXPECT_EQ(this->DDeque->Size(),ExpectedSize);
            ExpectedSize--;
            EXPECT_EQ(std::any_cast<TDequeSize>(this->DDeque->Back()),ExpectedSize);
            EXPECT_TRUE(this->DDeque->PopBack());
        }
    }
    
}