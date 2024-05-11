#include <gtest/gtest.h>
#include "PoolAllocator.h"

TEST(PoolAllocatorTest, AllocateAndFree) {
    const std::size_t totalSize = 1024;
    const std::size_t chunkSize = 8;
    PoolAllocator allocator(totalSize, chunkSize);
    allocator.Init();

    void* ptr1 = allocator.Allocate(chunkSize, 0);
    ASSERT_NE(ptr1, nullptr);

    void* ptr2 = allocator.Allocate(chunkSize, 0);
    ASSERT_NE(ptr2, nullptr);
    ASSERT_NE(ptr1, ptr2);

    allocator.Free(ptr1);
    allocator.Free(ptr2);
}


TEST(PoolAllocatorTest, Reset) {
    const std::size_t totalSize = 1024;
    const std::size_t chunkSize = 8;
    PoolAllocator allocator(totalSize, chunkSize);
    allocator.Init();

    void* ptr1 = allocator.Allocate(chunkSize, 0);
    void* ptr2 = allocator.Allocate(chunkSize, 0);

    allocator.Reset();

    void* ptr3 = allocator.Allocate(chunkSize, 0);
    void* ptr4 = allocator.Allocate(chunkSize, 0);

    ASSERT_NE(ptr1, ptr3);
    ASSERT_NE(ptr1, ptr4);
    ASSERT_NE(ptr2, ptr3);
    ASSERT_NE(ptr2, ptr4);

    allocator.Free(ptr3);
    allocator.Free(ptr4);
}

TEST(PoolAllocatorTest, InvalidChunkSize) {
    const std::size_t totalSize = 1024;
    const std::size_t chunkSize = 4;
    ASSERT_DEATH({ PoolAllocator allocator(totalSize, chunkSize); }, "");
}

TEST(PoolAllocatorTest, InvalidTotalSize) {
    const std::size_t totalSize = 1023;
    const std::size_t chunkSize = 8;
    ASSERT_DEATH({ PoolAllocator allocator(totalSize, chunkSize); }, "");
}

TEST(PoolAllocatorTest, InvalidAllocationSize) {
    const std::size_t totalSize = 1024;
    const std::size_t chunkSize = 8;
    PoolAllocator allocator(totalSize, chunkSize);
    allocator.Init();

    ASSERT_DEATH({ allocator.Allocate(chunkSize + 1, 0); }, "");
}
TEST(PoolAllocatorTest, AllocateWithAlignment){
    const std::size_t totalSize = 1024;
    const std::size_t chunkSize = 8;
    PoolAllocator allocator(totalSize, chunkSize);
    allocator.Init();

    const std::size_t alignment = 16;
    void *ptr = allocator.Allocate(chunkSize, alignment);
    ASSERT_NE(ptr, nullptr);
    ASSERT_EQ(reinterpret_cast<std::size_t>(ptr) % alignment, 8);

    allocator.Free(ptr);
}

TEST(PoolAllocatorTest, FreeNullptr)
{
    const std::size_t totalSize = 1024;
    const std::size_t chunkSize = 8;
    PoolAllocator allocator(totalSize, chunkSize);
    allocator.Init();

    ASSERT_NO_THROW(allocator.Free(nullptr));
}

TEST(PoolAllocatorTest, DoubleInit)
{
    const std::size_t totalSize = 1024;
    const std::size_t chunkSize = 8;
    PoolAllocator allocator(totalSize, chunkSize);
    allocator.Init();

    ASSERT_DEATH(allocator.Init(), "");
}

TEST(PoolAllocatorTest, DoubleFree)
{
    const std::size_t totalSize = 1024;
    const std::size_t chunkSize = 8;
    PoolAllocator allocator(totalSize, chunkSize);
    allocator.Init();

    void *ptr = allocator.Allocate(chunkSize, 0);
    allocator.Free(ptr);
    ASSERT_DEATH(allocator.Free(ptr), "");
}

TEST(PoolAllocatorTest, FreeInvalidPointer)
{
    const std::size_t totalSize = 1024;
    const std::size_t chunkSize = 8;
    PoolAllocator allocator(totalSize, chunkSize);
    allocator.Init();

    void *invalidPtr = reinterpret_cast<void *>(0x12345678);
    ASSERT_DEATH(allocator.Free(invalidPtr), "");
}
