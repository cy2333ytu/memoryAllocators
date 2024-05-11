cmake_minimum_required (VERSION 3.5)
project(AllocatorTest)
set(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../src/Allocator.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/../src/CAllocator.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/../src/LinearAllocator.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/../src/StackAllocator.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/../src/PoolAllocator.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/../src/FreeListAllocator.cpp)
enable_testing()
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
include_directories(../includes)

add_executable(FreeListAllocatorTests FreeListAllocatorTests.cpp ${SOURCES})
target_link_libraries(FreeListAllocatorTests gtest gtest_main pthread)

add_executable(LinearAllocatorTests LinearAllocatorTests.cpp ${SOURCES})
target_link_libraries(LinearAllocatorTests gtest gtest_main pthread)

add_executable(PoolAllocatorTest PoolAllocatorTest.cpp ${SOURCES})
target_link_libraries(PoolAllocatorTest gtest gtest_main pthread)

add_executable(StackAllocatorTests StackAllocatorTests.cpp ${SOURCES})
target_link_libraries(StackAllocatorTests gtest gtest_main pthread)
