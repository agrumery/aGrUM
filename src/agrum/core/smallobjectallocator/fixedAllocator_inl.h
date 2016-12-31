
/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 * @file
 * @brief Inlines of gum::FixedAllocator
 *
 * @author Jean-Christophe Magnan and Pierre-Henri WUILLEMIN
 *
 */
// ============================================================================
#include <agrum/core/smallobjectallocator/fixedAllocator.h>
// ============================================================================

namespace gum {

  // ============================================================================
  // Initializes a Chunk object
  // ============================================================================
  INLINE void FixedAllocator::__Chunk::__init( const std::size_t&   blockSize,
                                               const unsigned char& numBlocks ) {

    // Chunk memory space allocation. A chunk allocates a memory of blockSize *
    // numBlocks size.
    // The chunk will then give us numBlocks distinct blocks of blockSize from
    // that space.
    __pData = new unsigned char[blockSize * numBlocks];

    // The first available block of memory is logically at the beginning.
    __firstAvailableBlock = 0;

    // The number of block still available is all the blocks at the beginning.
    __blocksAvailable = numBlocks;

    // For each unallocated block, the first byte contains a number.
    // That number is the index of the next available block
    // Since we're at the beginning, next free block is the next one simply.
    // Following code initiate those number for each block
    unsigned char* p = __pData;
    for ( unsigned char indexBlock = 0; indexBlock != numBlocks; p += blockSize )
      *p = ++indexBlock;
  }

  // ============================================================================
  // Allocates a block of memory
  // ============================================================================
  INLINE void*
  FixedAllocator::__Chunk::__allocate( const std::size_t& blockSize ) {
    if ( !__blocksAvailable )
      // If no block is available return nullptr
      return NULL;

    // __pData points to the beginning of allocated space.
    // __firstAvailableBlock gives us how many block to pass before getting
    // the good one. We have to multiply by blockSize to get the good memory
    // emplacement
    unsigned char* pResult = __pData + ( __firstAvailableBlock * blockSize );

    // Remember that the first byte of each block gives us the index of next
    // available slot.
    // The new first availble block will be at the index indicating in this
    // block.
    __firstAvailableBlock = *pResult;

    // We lose one block
    --__blocksAvailable;

    return pResult;
  }

  // ============================================================================
  // Deallocates a block of memory
  // ============================================================================
  INLINE void
  FixedAllocator::__Chunk::__deallocat( void*              pDeallocatedBlock,
                                        const std::size_t& blockSize ) {

    // first, ensure that deallocated is in this chunk
    assert( pDeallocatedBlock >= __pData );

    // Conversion pf pointer for handling
    unsigned char* toRelease = static_cast<unsigned char*>( pDeallocatedBlock );

    // Alignement check
    assert( ( toRelease - __pData ) % blockSize == 0 );

    // First byte of toRelease has now to give the index of current first
    // available block
    *toRelease = __firstAvailableBlock;

    // So that first available block points to it
    __firstAvailableBlock =
        static_cast<unsigned char>( ( toRelease - __pData ) / blockSize );

    // Truncation check
    assert( __firstAvailableBlock == ( toRelease - __pData ) / blockSize );

    // We gain one block, yeah
    ++__blocksAvailable;
  }

  // ============================================================================
  // Releases the allocated memory
  // ============================================================================
  INLINE void FixedAllocator::__Chunk::__release() { delete[] __pData; }


  // ############################################################################
  // @name Constructors / Destructors
  // ############################################################################

  // ============================================================================
  // Constructor.
  // ============================================================================
  INLINE FixedAllocator::FixedAllocator( const std::size_t&   blockSize,
                                         const unsigned char& numBlocks ) {
    //    GUM_CONSTRUCTOR(FixedAllocator)
    __blockSize = blockSize;
    __numBlocks = numBlocks;
    __allocChunk = __chunks.begin();
    __deallocChunk = __chunks.begin();
  }

  // ============================================================================
  // Destructor.
  // ============================================================================
  INLINE FixedAllocator::~FixedAllocator() {
    for ( __Chunks::iterator chunkIter = __chunks.begin();
          chunkIter != __chunks.end();
          ++chunkIter )
      chunkIter->__release();
    //    GUM_DESTRUCTOR(FixedAllocator)
  }

  // ############################################################################
  // @name Allocator / Deallocator
  // ############################################################################

  // ============================================================================
  // Allocates a block
  // ============================================================================
  INLINE void* FixedAllocator::allocate() {

    if ( __chunks.empty() || __allocChunk->__blocksAvailable == 0 ) {
      // no available memory in this chunk
      // Try to find one with memory available
      for ( __Chunks::iterator chunksIter = __chunks.begin();; ++chunksIter ) {
        if ( chunksIter == __chunks.end() ) {
          // All chunks are filled up. Adding a new one
          __chunks.reserve( __chunks.size() + 1 );
          __Chunk newChunk;
          newChunk.__init( __blockSize, __numBlocks );
          __chunks.push_back( newChunk );
          __allocChunk = __chunks.end();
          --__allocChunk;
          __deallocChunk = __allocChunk;
          break;
        }
        if ( chunksIter->__blocksAvailable > 0 ) {
          // Found a chunk
          __allocChunk = chunksIter;
          break;
        }
      }
    }
    return __allocChunk->__allocate( __blockSize );
  }

  // ============================================================================
  // Deallocates a block
  // ============================================================================
  INLINE void FixedAllocator::deallocate( void* pDeallocatedBlock ) {

    if ( __deallocChunk->__pData > pDeallocatedBlock ||
         pDeallocatedBlock >
             ( __deallocChunk->__pData + ( __numBlocks * __blockSize ) ) ) {
      // If not things get ugly
      // We have to find where the Chunk containing this pointer is
      std::ptrdiff_t offset = 0;

      // We perform a bidirectionnal search from __deallocChunk
      while ( true ) {
        ++offset;
        // First we look for the one going to the end of the vector
        if ( ( __deallocChunk + offset ) < __chunks.end() ) {
          if ( ( __deallocChunk + offset )->__pData <= pDeallocatedBlock &&
               pDeallocatedBlock < ( ( __deallocChunk + offset )->__pData +
                                     ( __numBlocks * __blockSize ) ) ) {
            // If pointed chunk contains this pointer, deallocation find the
            // place
            __deallocChunk = ( __deallocChunk + offset );
            break;
          }
        }

        // Then we look for the one going to the beginning of the vector
        if ( ( __deallocChunk - offset ) >= __chunks.begin() ) {
          if ( ( __deallocChunk - offset )->__pData <= pDeallocatedBlock &&
               pDeallocatedBlock < ( ( __deallocChunk - offset )->__pData +
                                     ( __numBlocks * __blockSize ) ) ) {
            // If pointed chunk contains this pointer, deallocation find the
            // place
            __deallocChunk = ( __deallocChunk - offset );
            break;
          }
        }
      }
    }
    __deallocChunk->__deallocat( pDeallocatedBlock, __blockSize );
  }

}  // namespace gum
