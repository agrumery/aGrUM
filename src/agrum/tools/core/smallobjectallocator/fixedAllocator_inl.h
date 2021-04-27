/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Inlines of gum::FixedAllocator
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(@AMU)
 *
 */
// ============================================================================
#include <agrum/tools/core/smallobjectallocator/fixedAllocator.h>
// ============================================================================

namespace gum {

  // ============================================================================
  // Initializes a Chunk object
  // ============================================================================
  INLINE void FixedAllocator::Chunk__::init__(const std::size_t&   blockSize,
                                              const unsigned char& numBlocks) {
    // Chunk memory space allocation. A chunk allocates a memory of blockSize *
    // numBlocks size.
    // The chunk will then give us numBlocks distinct blocks of blockSize from
    // that space.
    pData__ = new unsigned char[blockSize * numBlocks];

    // The first available block of memory is logically at the beginning.
    firstAvailableBlock__ = 0;

    // The number of block still available is all the blocks at the beginning.
    blocksAvailable__ = numBlocks;

    // For each unallocated block, the first byte contains a number.
    // That number is the index of the next available block
    // Since we're at the beginning, next free block is the next one simply.
    // Following code initiate those number for each block
    unsigned char* p = pData__;
    for (unsigned char indexBlock = 0; indexBlock != numBlocks; p += blockSize)
      *p = ++indexBlock;
  }

  // ============================================================================
  // Allocates a block of memory
  // ============================================================================
  INLINE void* FixedAllocator::Chunk__::allocate__(const std::size_t& blockSize) {
    if (!blocksAvailable__)
      // If no block is available return nullptr
      return NULL;

    // pData__ points to the beginning of allocated space.
    // firstAvailableBlock__ gives us how many block to pass before getting
    // the good one. We have to multiply by blockSize to get the good memory
    // emplacement
    unsigned char* pResult = pData__ + (firstAvailableBlock__ * blockSize);

    // Remember that the first byte of each block gives us the index of next
    // available slot.
    // The new first availble block will be at the index indicating in this
    // block.
    firstAvailableBlock__ = *pResult;

    // We lose one block
    --blocksAvailable__;

    return pResult;
  }

  // ============================================================================
  // Deallocates a block of memory
  // ============================================================================
  INLINE void FixedAllocator::Chunk__::deallocat__(void* pDeallocatedBlock,
                                                   const std::size_t& blockSize) {
    // first, ensure that deallocated is in this chunk
    GUM_ASSERT(pDeallocatedBlock >= pData__);

    // Conversion pf pointer for handling
    unsigned char* toRelease = static_cast< unsigned char* >(pDeallocatedBlock);

    // Alignement check
    GUM_ASSERT((toRelease - pData__) % blockSize == 0);

    // First byte of toRelease has now to give the index of current first
    // available block
    *toRelease = firstAvailableBlock__;

    // So that first available block points to it
    firstAvailableBlock__
       = static_cast< unsigned char >((toRelease - pData__) / blockSize);

    // Truncation check
    GUM_ASSERT(firstAvailableBlock__ == (toRelease - pData__) / blockSize);

    // We gain one block, yeah
    ++blocksAvailable__;
  }

  // ============================================================================
  // Releases the allocated memory
  // ============================================================================
  INLINE void FixedAllocator::Chunk__::release__() { delete[] pData__; }


  // ############################################################################
  // @name Constructors / Destructors
  // ############################################################################

  // ============================================================================
  // Constructor.
  // ============================================================================
  INLINE FixedAllocator::FixedAllocator(const std::size_t&   blockSize,
                                        const unsigned char& numBlocks) {
    //    GUM_CONSTRUCTOR(FixedAllocator)
    blockSize__    = blockSize;
    numBlocks__    = numBlocks;
    allocChunk__   = chunks__.begin();
    deallocChunk__ = chunks__.begin();
  }

  // ============================================================================
  // Destructor.
  // ============================================================================
  INLINE FixedAllocator::~FixedAllocator() {
    for (Chunks__::iterator chunkIter = chunks__.begin();
         chunkIter != chunks__.end();
         ++chunkIter)
      chunkIter->release__();
    //    GUM_DESTRUCTOR(FixedAllocator)
  }

  // ############################################################################
  // @name Allocator / Deallocator
  // ############################################################################

  // ============================================================================
  // Allocates a block
  // ============================================================================
  INLINE void* FixedAllocator::allocate() {
    if (chunks__.empty() || allocChunk__->blocksAvailable__ == 0) {
      // no available memory in this chunk
      // Try to find one with memory available
      for (Chunks__::iterator chunksIter = chunks__.begin();; ++chunksIter) {
        if (chunksIter == chunks__.end()) {
          // All chunks are filled up. Adding a new one
          chunks__.reserve(chunks__.size() + 1);
          Chunk__ newChunk;
          newChunk.init__(blockSize__, numBlocks__);
          chunks__.push_back(newChunk);
          allocChunk__ = chunks__.end();
          --allocChunk__;
          deallocChunk__ = allocChunk__;
          break;
        }
        if (chunksIter->blocksAvailable__ > 0) {
          // Found a chunk
          allocChunk__ = chunksIter;
          break;
        }
      }
    }
    return allocChunk__->allocate__(blockSize__);
  }

  // ============================================================================
  // Deallocates a block
  // ============================================================================
  INLINE void FixedAllocator::deallocate(void* pDeallocatedBlock) {
    if (deallocChunk__->pData__ > pDeallocatedBlock
        || pDeallocatedBlock
              > (deallocChunk__->pData__ + (numBlocks__ * blockSize__))) {
      // If not things get ugly
      // We have to find where the Chunk containing this pointer is
      std::ptrdiff_t offset = 0;

      // We perform a bidirectionnal search from deallocChunk__
      while (true) {
        ++offset;
        // First we look for the one going to the end of the vector
        if ((deallocChunk__ + offset) < chunks__.end()) {
          if ((deallocChunk__ + offset)->pData__ <= pDeallocatedBlock
              && pDeallocatedBlock < ((deallocChunk__ + offset)->pData__
                                      + (numBlocks__ * blockSize__))) {
            // If pointed chunk contains this pointer, deallocation find the
            // place
            deallocChunk__ = (deallocChunk__ + offset);
            break;
          }
        }

        // Then we look for the one going to the beginning of the vector
        if ((deallocChunk__ - offset) >= chunks__.begin()) {
          if ((deallocChunk__ - offset)->pData__ <= pDeallocatedBlock
              && pDeallocatedBlock < ((deallocChunk__ - offset)->pData__
                                      + (numBlocks__ * blockSize__))) {
            // If pointed chunk contains this pointer, deallocation find the
            // place
            deallocChunk__ = (deallocChunk__ - offset);
            break;
          }
        }
      }
    }
    deallocChunk__->deallocat__(pDeallocatedBlock, blockSize__);
  }

}   // namespace gum
