#include <gtest/gtest.h>
#include "StackAllocator.h"

TEST(StackAllocatorTests, InitializesCorrectly)
{
    StackAllocator allocator(1024);
    allocator.Init();
    ASSERT_NE(allocator.GetStartPtr(), nullptr);
    ASSERT_EQ(allocator.GetOffset(), 0);
    ASSERT_EQ(allocator.GetUsed(), 0);
    ASSERT_EQ(allocator.GetPeak(), 0);
}

TEST(StackAllocatorTests, AllocatesAndFreesMemory)
{
    StackAllocator allocator(1024);
    allocator.Init();

    void *ptr1 = allocator.Allocate(16, 4);
    ASSERT_NE(ptr1, nullptr);
    ASSERT_EQ(allocator.GetUsed(), 20);
    ASSERT_EQ(allocator.GetPeak(), 20);

    void *ptr2 = allocator.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    ASSERT_EQ(allocator.GetUsed(), 60);
    ASSERT_EQ(allocator.GetPeak(), 60);

    allocator.Free(ptr2);
    ASSERT_EQ(allocator.GetUsed(), 20);
    ASSERT_EQ(allocator.GetPeak(), 60);

    allocator.Free(ptr1);
    ASSERT_EQ(allocator.GetUsed(), 0);
    ASSERT_EQ(allocator.GetPeak(), 60);
}

TEST(StackAllocatorTests, HandlesOutOfMemory)
{
    StackAllocator allocator(64);
    allocator.Init();

    void *ptr1 = allocator.Allocate(32, 4);
    ASSERT_NE(ptr1, nullptr);

    void *ptr2 = allocator.Allocate(64, 8);
    ASSERT_EQ(ptr2, nullptr);
}

TEST(StackAllocatorTests, ResetClearsMemory)
{
    StackAllocator allocator(1024);
    allocator.Init();

    void *ptr1 = allocator.Allocate(16, 4);
    void *ptr2 = allocator.Allocate(32, 8);

    ASSERT_NE(ptr1, nullptr);
    ASSERT_NE(ptr2, nullptr);
    ASSERT_EQ(allocator.GetUsed(), 0);
    ASSERT_EQ(allocator.GetPeak(), 0);

    allocator.Reset();

    ASSERT_EQ(allocator.GetOffset(), 0);
    ASSERT_EQ(allocator.GetUsed(), 0);
    ASSERT_EQ(allocator.GetPeak(), 0);
}
