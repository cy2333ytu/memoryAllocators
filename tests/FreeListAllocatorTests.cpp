#include <gtest/gtest.h>
#include "FreeListAllocator.h"

TEST(FreeListAllocator, AllocateAndFree) {
    FreeListAllocator allocator(1024, FreeListAllocator::FIND_FIRST);
    allocator.Init();

    void* ptr1 = allocator.Allocate(16, 8);
    ASSERT_NE(ptr1, nullptr);

    void* ptr2 = allocator.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);

    allocator.Free(ptr1);
    allocator.Free(ptr2);
}

TEST(FreeListAllocator, AlignmentPadding) {
    FreeListAllocator allocator(1024, FreeListAllocator::FIND_FIRST);
    allocator.Init();

    void* ptr = allocator.Allocate(16, 16);
    ASSERT_EQ(reinterpret_cast<std::size_t>(ptr) % 16, 0);

    allocator.Free(ptr);
}

TEST(FreeListAllocator, FindFirst) {
    FreeListAllocator allocator(1024, FreeListAllocator::FIND_FIRST);
    allocator.Init();

    void* ptr1 = allocator.Allocate(100, 8);
    void* ptr2 = allocator.Allocate(200, 8);
    allocator.Free(ptr1);

    void* ptr3 = allocator.Allocate(50, 8);
    ASSERT_EQ(ptr3, ptr1);

    allocator.Free(ptr2);
    allocator.Free(ptr3);
}

TEST(FreeListAllocator, FindBest) {
    FreeListAllocator allocator(1024, FreeListAllocator::FIND_BEST);
    allocator.Init();

    void* ptr1 = allocator.Allocate(100, 8);
    void* ptr2 = allocator.Allocate(200, 8);
    allocator.Free(ptr1);
    allocator.Free(ptr2);

    void* ptr3 = allocator.Allocate(150, 8);
    ASSERT_EQ(ptr3, ptr2);

    allocator.Free(ptr3);
}

TEST(FreeListAllocator, Coalescence) {
    FreeListAllocator allocator(1024, FreeListAllocator::FIND_FIRST);
    allocator.Init();

    void* ptr1 = allocator.Allocate(100, 8);
    void* ptr2 = allocator.Allocate(200, 8);
    void* ptr3 = allocator.Allocate(300, 8);
    allocator.Free(ptr2);
    allocator.Free(ptr1);
    allocator.Free(ptr3);

    void* ptr4 = allocator.Allocate(500, 8);
    ASSERT_NE(ptr4, nullptr);

    allocator.Free(ptr4);
}

TEST(FreeListAllocator, Reset) {
    FreeListAllocator allocator(1024, FreeListAllocator::FIND_FIRST);
    allocator.Init();

    void* ptr1 = allocator.Allocate(100, 8);
    void* ptr2 = allocator.Allocate(200, 8);
    allocator.Free(ptr1);
    allocator.Free(ptr2);

    allocator.Reset();

    void* ptr3 = allocator.Allocate(1024, 8);
    ASSERT_NE(ptr3, nullptr);

    allocator.Free(ptr3);
}
