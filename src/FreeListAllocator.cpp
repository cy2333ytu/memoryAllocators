#include "FreeListAllocator.h"
#include "Utils.h"  /* CalculatePaddingWithHeader */
#include <stdlib.h>     /* malloc, free */
#include <cassert>   /* assert		*/
#include <limits>  /* limits_max */
#include <algorithm>    // std::max

#ifdef _DEBUG
#include <iostream>
#endif

FreeListAllocator::FreeListAllocator(const std::size_t totalSize, const PlacementPolicy pPolicy)
: Allocator(totalSize) {
    m_pPolicy = pPolicy;
}

void FreeListAllocator::Init() {
    if (m_start_ptr != nullptr) {
        free(m_start_ptr);
        m_start_ptr = nullptr;
    }
    m_start_ptr = malloc(m_totalSize);

    this->Reset();
}

FreeListAllocator::~FreeListAllocator() {
    free(m_start_ptr);
    m_start_ptr = nullptr;
}

/// Allocates a block of memory from the free list that can accommodate the requested allocation.
///
/// The function first searches the free list for a free block that is large enough to accommodate the requested allocation,
/// taking into account the required alignment. If a suitable free block is found, the function may split the block into
/// a data block and a remaining free block, and removes the data block from the free list.
///
/// The function then sets up the allocation header for the data block, updates the used memory and peak memory usage statistics,
/// and returns a pointer to the start of the data block.
///
/// @param size The size of the requested allocation.
/// @param alignment The alignment requirement for the requested allocation.
/// @return A pointer to the start of the allocated data block.

void *FreeListAllocator::Allocate(const std::size_t size, const std::size_t alignment)
{
    const std::size_t allocationHeaderSize = sizeof(FreeListAllocator::AllocationHeader);
    const std::size_t freeHeaderSize = sizeof(FreeListAllocator::FreeHeader);
    assert("Allocation size must be bigger" && size >= sizeof(Node));
    assert("Alignment must be 8 at least" && alignment >= 8);

    // Search through the free list for a free block that has enough space to allocate our data
    std::size_t padding;
    Node *affectedNode,
        *previousNode;
    this->Find(size, alignment, padding, previousNode, affectedNode);
    assert(affectedNode != nullptr && "Not enough memory");

    const std::size_t alignmentPadding = padding - allocationHeaderSize;
    const std::size_t requiredSize = size + padding;

    const std::size_t rest = affectedNode->data.blockSize - requiredSize;

    if (rest > sizeof(Node))
    {
        // We have to split the block into the data block and a free block of size 'rest'
        Node *newFreeNode = (Node *)((std::size_t)affectedNode + requiredSize);
        newFreeNode->data.blockSize = rest;
        m_freeList.insert(affectedNode, newFreeNode);
    }
    m_freeList.remove(previousNode, affectedNode);

    // Setup data block
    const std::size_t headerAddress = (std::size_t)affectedNode + alignmentPadding;
    const std::size_t dataAddress = headerAddress + allocationHeaderSize;
    ((FreeListAllocator::AllocationHeader *)headerAddress)->blockSize = requiredSize;
    ((FreeListAllocator::AllocationHeader *)headerAddress)->padding = alignmentPadding;

    m_used += requiredSize;
    m_peak = std::max(m_peak, m_used);

#ifdef _DEBUG
    std::cout << "A" << "\t@H " << (void *)headerAddress << "\tD@ " << (void *)dataAddress << "\tS " << ((FreeListAllocator::AllocationHeader *)headerAddress)->blockSize << "\tAP " << alignmentPadding << "\tP " << padding << "\tM " << m_used << "\tR " << rest << std::endl;
#endif

    return (void *)dataAddress;
}

/// Finds a free block in the free list that can accommodate the requested allocation.
///
/// The behavior of the find operation is determined by the current allocation policy, which can be either FIND_FIRST or FIND_BEST.
///
/// If the FIND_FIRST policy is used, the function will return the first free block in the list that is large enough to accommodate the requested allocation.
/// If the FIND_BEST policy is used, the function will return the free block in the list that best fits the requested allocation (i.e., the block with the smallest difference between its size and the requested size).
///
/// The function updates the `padding`, `previousNode`, and `foundNode` parameters to provide information about the found free block.
///
/// @param size The size of the requested allocation.
/// @param alignment The alignment requirement for the requested allocation.
/// @param[out] padding The padding required to align the allocation within the found free block.
/// @param[out] previousNode A pointer to the node in the free list that precedes the found free block.
/// @param[out] foundNode A pointer to the found free block.
void FreeListAllocator::Find(const std::size_t size, const std::size_t alignment, std::size_t &padding, Node *&previousNode, Node *&foundNode)
{
    switch (m_pPolicy)
    {
    case FIND_FIRST:
        FindFirst(size, alignment, padding, previousNode, foundNode);
        break;
    case FIND_BEST:
        FindBest(size, alignment, padding, previousNode, foundNode);
        break;
    }
}

void FreeListAllocator::FindFirst(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node *& previousNode, Node *& foundNode) {
    //Iterate list and return the first free block with a size >= than given size
    Node * it = m_freeList.head,
         * itPrev = nullptr;
    
    while (it != nullptr) {
        padding = Utils::CalculatePaddingWithHeader((std::size_t)it, alignment, sizeof (FreeListAllocator::AllocationHeader));
        const std::size_t requiredSpace = size + padding;
        if (it->data.blockSize >= requiredSpace) {
            break;
        }
        itPrev = it;
        it = it->next;
    }
    previousNode = itPrev;
    foundNode = it;
}

void FreeListAllocator::FindBest(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node *& previousNode, Node *& foundNode) {
    // Iterate WHOLE list keeping a pointer to the best fit
    std::size_t smallestDiff = std::numeric_limits<std::size_t>::max();
    Node * bestBlock = nullptr;
    Node * it = m_freeList.head,
         * itPrev = nullptr;
    while (it != nullptr) {
        padding = Utils::CalculatePaddingWithHeader((std::size_t)it, alignment, sizeof (FreeListAllocator::AllocationHeader));
        const std::size_t requiredSpace = size + padding;
        if (it->data.blockSize >= requiredSpace && (it->data.blockSize - requiredSpace < smallestDiff)) {
            bestBlock = it;
        }
        itPrev = it;
        it = it->next;
    }
    previousNode = itPrev;
    foundNode = bestBlock;
}

void FreeListAllocator::Free(void* ptr) {
    // Insert it in a sorted position by the address number
    const std::size_t currentAddress = (std::size_t) ptr;
    const std::size_t headerAddress = currentAddress - sizeof (FreeListAllocator::AllocationHeader);
    const FreeListAllocator::AllocationHeader * allocationHeader{ (FreeListAllocator::AllocationHeader *) headerAddress};

    Node * freeNode = (Node *) (headerAddress);
    freeNode->data.blockSize = allocationHeader->blockSize + allocationHeader->padding;
    freeNode->next = nullptr;

    Node * it = m_freeList.head;
    Node * itPrev = nullptr;
    while (it != nullptr) {
        if (ptr < it) {
            m_freeList.insert(itPrev, freeNode);
            break;
        }
        itPrev = it;
        it = it->next;
    }
    
    m_used -= freeNode->data.blockSize;

    // Merge contiguous nodes
    Coalescence(itPrev, freeNode);  

#ifdef _DEBUG
    std::cout << "F" << "\t@ptr " <<  ptr <<"\tH@ " << (void*) freeNode << "\tS " << freeNode->data.blockSize << "\tM " << m_used << std::endl;
#endif
}

void FreeListAllocator::Coalescence(Node* previousNode, Node * freeNode) {   
    if (freeNode->next != nullptr && 
            (std::size_t) freeNode + freeNode->data.blockSize == (std::size_t) freeNode->next) {
        freeNode->data.blockSize += freeNode->next->data.blockSize;
        m_freeList.remove(freeNode, freeNode->next);
#ifdef _DEBUG
    std::cout << "\tMerging(n) " << (void*) freeNode << " & " << (void*) freeNode->next << "\tS " << freeNode->data.blockSize << std::endl;
#endif
    }
    
    if (previousNode != nullptr &&
            (std::size_t) previousNode + previousNode->data.blockSize == (std::size_t) freeNode) {
        previousNode->data.blockSize += freeNode->data.blockSize;
        m_freeList.remove(previousNode, freeNode);
#ifdef _DEBUG
    std::cout << "\tMerging(p) " << (void*) previousNode << " & " << (void*) freeNode << "\tS " << previousNode->data.blockSize << std::endl;
#endif
    }
}

void FreeListAllocator::Reset() {
    m_used = 0;
    m_peak = 0;
    Node * firstNode = (Node *) m_start_ptr;
    firstNode->data.blockSize = m_totalSize;
    firstNode->next = nullptr;
    m_freeList.head = nullptr;
    m_freeList.insert(nullptr, firstNode);
}

std::size_t FreeListAllocator::GetPeakMemoryUsage() const {
    return m_peak;
}