/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 *
 */
// ============================================================================
#include <agrum/tools/core/smallobjectallocator/fixedAllocator.h>
// ============================================================================

namespace gum {

  // ============================================================================
  // Initializes a Chunk object
  // ============================================================================
  INLINE void FixedAllocator::_Chunk_::_init_(const std::size_t&   blockSize,
                                              const unsigned char& numBlocks) {
    // Chunk memory space allocation. A chunk allocates a memory of blockSize *
    // numBlocks size.
    // The chunk will then give us numBlocks distinct blocks of blockSize from
    // that space.
    _pData_ = new unsigned char[blockSize * numBlocks];

    // The first available block of memory is logically at the beginning.
    _firstAvailableBlock_ = 0;

    // The number of block still available is all the blocks at the beginning.
    _blocksAvailable_ = numBlocks;

    // For each unallocated block, the first byte contains a number.
    // That number is the index of the next available block
    // Since we're at the beginning, next free block is the next one simply.
    // Following code initiate those number for each block
    unsigned char* p = _pData_;
    for (unsigned char indexBlock = 0; indexBlock != numBlocks; p += blockSize)
      *p = ++indexBlock;
  }

  // ============================================================================
  // Allocates a block of memory
  // ============================================================================
  INLINE void* FixedAllocator::_Chunk_::_allocate_(const std::size_t& blockSize) {
    if (!_blocksAvailable_)
      // If no block is available return nullptr
      return NULL;

    //  _pData_ points to the beginning of allocated space.
    //  _firstAvailableBlock_ gives us how many block to pass before getting
    // the good one. We have to multiply by blockSize to get the good memory
    // emplacement
    unsigned char* pResult = _pData_ + (_firstAvailableBlock_ * blockSize);

    // Remember that the first byte of each block gives us the index of next
    // available slot.
    // The new first availble block will be at the index indicating in this
    // block.
    _firstAvailableBlock_ = *pResult;

    // We lose one block
    --_blocksAvailable_;

    return pResult;
  }

  // ============================================================================
  // Deallocates a block of memory
  // ============================================================================
  INLINE void FixedAllocator::_Chunk_::_deallocat_(void*              pDeallocatedBlock,
                                                   const std::size_t& blockSize) {
    // first, ensure that deallocated is in this chunk
    GUM_ASSERT(pDeallocatedBlock >= _pData_);

    // Conversion pf pointer for handling
    unsigned char* toRelease = static_cast< unsigned char* >(pDeallocatedBlock);

    // Alignement check
    GUM_ASSERT((toRelease - _pData_) % blockSize == 0);

    // First byte of toRelease has now to give the index of current first
    // available block
    *toRelease = _firstAvailableBlock_;

    // So that first available block points to it
    _firstAvailableBlock_ = static_cast< unsigned char >((toRelease - _pData_) / blockSize);

    // Truncation check
    GUM_ASSERT(_firstAvailableBlock_ == (toRelease - _pData_) / blockSize);

    // We gain one block, yeah
    ++_blocksAvailable_;
  }

  // ============================================================================
  // Releases the allocated memory
  // ============================================================================
  INLINE void FixedAllocator::_Chunk_::_release_() { delete[] _pData_; }


  // ############################################################################
  // @name Constructors / Destructors
  // ############################################################################

  // ============================================================================
  // Constructor.
  // ============================================================================
  INLINE FixedAllocator::FixedAllocator(const std::size_t&   blockSize,
                                        const unsigned char& numBlocks) {
    //    GUM_CONSTRUCTOR(FixedAllocator);
    _blockSize_    = blockSize;
    _numBlocks_    = numBlocks;
    _allocChunk_   = _chunks_.begin();
    _deallocChunk_ = _chunks_.begin();
  }

  // ============================================================================
  // Destructor.
  // ============================================================================
  INLINE FixedAllocator::~FixedAllocator() {
    for (_Chunks_::iterator chunkIter = _chunks_.begin(); chunkIter != _chunks_.end(); ++chunkIter)
      chunkIter->_release_();
    //    GUM_DESTRUCTOR(FixedAllocator);
  }

  // ############################################################################
  // @name Allocator / Deallocator
  // ############################################################################

  // ============================================================================
  // Allocates a block
  // ============================================================================
  INLINE void* FixedAllocator::allocate() {
    if (_chunks_.empty() || _allocChunk_->_blocksAvailable_ == 0) {
      // no available memory in this chunk
      // Try to find one with memory available
      for (_Chunks_::iterator chunksIter = _chunks_.begin();; ++chunksIter) {
        if (chunksIter == _chunks_.end()) {
          // All chunks are filled up. Adding a new one
          _chunks_.reserve(_chunks_.size() + 1);
          _Chunk_ newChunk;
          newChunk._init_(_blockSize_, _numBlocks_);
          _chunks_.push_back(newChunk);
          _allocChunk_ = _chunks_.end();
          --_allocChunk_;
          _deallocChunk_ = _allocChunk_;
          break;
        }
        if (chunksIter->_blocksAvailable_ > 0) {
          // Found a chunk
          _allocChunk_ = chunksIter;
          break;
        }
      }
    }
    return _allocChunk_->_allocate_(_blockSize_);
  }

  // ============================================================================
  // Deallocates a block
  // ============================================================================
  INLINE void FixedAllocator::deallocate(void* pDeallocatedBlock) {
    if (_deallocChunk_->_pData_ > pDeallocatedBlock
        || pDeallocatedBlock > (_deallocChunk_->_pData_ + (_numBlocks_ * _blockSize_))) {
      // If not things get ugly
      // We have to find where the Chunk containing this pointer is
      std::ptrdiff_t offset = 0;

      // We perform a bidirectionnal search from  _deallocChunk_
      while (true) {
        ++offset;
        // First we look for the one going to the end of the vector
        if ((_deallocChunk_ + offset) < _chunks_.end()) {
          if ((_deallocChunk_ + offset)->_pData_ <= pDeallocatedBlock
              && pDeallocatedBlock
                    < ((_deallocChunk_ + offset)->_pData_ + (_numBlocks_ * _blockSize_))) {
            // If pointed chunk contains this pointer, deallocation find the
            // place
            _deallocChunk_ = (_deallocChunk_ + offset);
            break;
          }
        }

        // Then we look for the one going to the beginning of the vector
        if ((_deallocChunk_ - offset) >= _chunks_.begin()) {
          if ((_deallocChunk_ - offset)->_pData_ <= pDeallocatedBlock
              && pDeallocatedBlock
                    < ((_deallocChunk_ - offset)->_pData_ + (_numBlocks_ * _blockSize_))) {
            // If pointed chunk contains this pointer, deallocation find the
            // place
            _deallocChunk_ = (_deallocChunk_ - offset);
            break;
          }
        }
      }
    }
    _deallocChunk_->_deallocat_(pDeallocatedBlock, _blockSize_);
  }

}   // namespace gum
