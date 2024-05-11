/**
 * @brief Abstract base class for memory allocators.
 *
 * The `Allocator` class provides an interface for dynamic memory allocation and
 * deallocation, with support for alignment. Derived classes must implement the
 * pure virtual functions `Allocate()`, `Free()`, and `Init()`.
 *
 * The class also tracks the total size of the allocator, the amount of memory
 * currently in use, and the peak memory usage.
 */
#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <cstddef> // size_t

class Allocator
{
protected:
    std::size_t m_totalSize;
    std::size_t m_used;
    std::size_t m_peak;

public:
    Allocator(const std::size_t totalSize) : m_totalSize{totalSize}, m_used{0}, m_peak{0} {}

    virtual ~Allocator() { m_totalSize = 0; }

    virtual void *Allocate(const std::size_t size, const std::size_t alignment = 0) = 0;

    virtual void Free(void *ptr) = 0;

    virtual void Init() = 0;

    friend class Benchmark;
    
    std::size_t GetOffset() const { return m_totalSize; }
    std::size_t GetUsed() const { return m_used; }
    std::size_t GetPeak() const { return m_peak; }
};

#endif /* ALLOCATOR_H */

