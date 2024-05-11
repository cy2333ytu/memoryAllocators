#include "LinearAllocator.h"
#include <gtest/gtest.h>

TEST(LinearAllocatorTests, AllocateAndReset) {
    LinearAllocator allocator(1024);
    allocator.Init();

    void* ptr1 = allocator.Allocate(16, 4);
    ASSERT_NE(ptr1, nullptr);

    void* ptr2 = allocator.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);

    allocator.Reset();

    void* ptr3 = allocator.Allocate(64, 16);
    ASSERT_NE(ptr3, nullptr);
}

TEST(LinearAllocatorTests, AllocateOutOfMemory) {
    LinearAllocator allocator(128);
    allocator.Init();

    void* ptr1 = allocator.Allocate(64, 8);
    ASSERT_NE(ptr1, nullptr);

    void* ptr2 = allocator.Allocate(128, 16);
    ASSERT_EQ(ptr2, nullptr);
}

TEST(LinearAllocatorTests, AlignmentPadding) {
    LinearAllocator allocator(1024);
    allocator.Init();

    void* ptr1 = allocator.Allocate(16, 4);
    ASSERT_NE(ptr1, nullptr);

    void* ptr2 = allocator.Allocate(64, 48);
    ASSERT_NE(ptr2, nullptr);

    std::size_t offset = reinterpret_cast<std::size_t>(ptr2) - reinterpret_cast<std::size_t>(ptr1);
    ASSERT_EQ(offset, 32);
}

TEST(LinearAllocatorTests, FreeNotAllowed) {
    LinearAllocator allocator(1024);
    allocator.Init();

    void* ptr = allocator.Allocate(16, 4);
    ASSERT_NE(ptr, nullptr);

    ASSERT_DEATH(allocator.Free(ptr), "Use Reset\\(\\) method");
}
