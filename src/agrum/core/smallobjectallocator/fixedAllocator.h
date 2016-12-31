
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
 * @brief Headers of gum::FixedAllocator
 *
 * @author Jean-Christophe Magnan and Pierre-Henri WUILLEMIN
 *
 */
#ifndef GUM_FIXED_ALLOCATOR_H
#define GUM_FIXED_ALLOCATOR_H

// ============================================================================
#include <mutex>
#include <thread>
// ============================================================================
#include <agrum/config.h>
// ============================================================================

namespace gum {
  /**
   * @class FixedAllocator fixedAllocator.h <agrum/core/fixedAllocator.h>
   *
   * @brief Allocates objects of one given size
   *
   * Fixed allocator knows how to allocate and deallocate blocks of fixed size
   * but is not limited to a chunck size. Its capacity is limited only by the
   * available memory.
   * To achieve this, FixedAllocator aggregates a vector of Chunk objects.
   * Whenever an allocation request occurs, FixedAllocators looks for a Chunk
   * that can accomodate the request.
   * If all Chunks are filled up, FixedAllocator appends a new Chunk.
   *
   * @ingroup core
   */

  class FixedAllocator {
    /**
     * @struct Chunk fixedAllocator.h <agrum/core/fixedAllocator.h>
     *
     * @brief Allocates objects of one given size. Has a fixed limit of
     * allocation
     *
     * Each object of type Chunk contains and manages a chunk of memory
     * containing a
     * amount of blocks. At construction time, you configure the block size and
     * the
     * number of blocks.
     * A Chunk contains logic that allows you to allocate and deallocate memory
     * blocks
     * from that chunk of memory. When there are no more blocks available in the
     * chunk,
     * the allocation function returns zero.
     *
     * @ingroup core
     */
    struct __Chunk {

      // ============================================================================
      /// Initializes a Chunk object
      // ============================================================================
      void __init( const std::size_t& blockSize, const unsigned char& numBlocks );

      // ============================================================================
      /// Allocates a block of memory
      // ============================================================================
      void* __allocate( const std::size_t& blockSize );

      // ============================================================================
      /// Deallocates a block of memory
      // ============================================================================
      void __deallocat( void* p, const std::size_t& blockSize );

      // ============================================================================
      /// Releases the allocated memory
      // ============================================================================
      void __release();

      // ============================================================================
      /// Pointer to the managed memory itself
      // ============================================================================
      unsigned char* __pData;

      // ============================================================================
      /// Holds the index of the first block available in this chunck
      // ============================================================================
      unsigned char __firstAvailableBlock;

      // ============================================================================
      /// Number of blocks available in this chunck
      // ============================================================================
      unsigned char __blocksAvailable;
    };

    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    // ============================================================================
    /**
     * Constructor.
     * @param blockSize is the size of an allocated block.
     * @param numBlocks is the number of block allocated per chunk
     * numBlock * blockSize is the size that a chunk allocates directly
     * when it is created
     */
    // ============================================================================
    FixedAllocator( const std::size_t&   blockSize,
                    const unsigned char& numBlocks = UCHAR_MAX );

    // ============================================================================
    /// Destructor.
    // ============================================================================
    ~FixedAllocator();

    /// @}

    // ############################################################################
    /// @name Allocator / Deallocator
    // ############################################################################
    /// @{

    // ============================================================================
    /// Allocates a block
    // ============================================================================
    void* allocate();

    // ============================================================================
    /// Deallocates a block
    // ============================================================================
    void deallocate( void* pDeallocatedBlock );

    /// @}

    // ============================================================================
    /// Returns the size of block allocated by this FixedAllocator
    // ============================================================================
    const size_t& objectSize() { return __blockSize; }

    private:
    // ============================================================================
    /// Size of a memory block allocated
    // ============================================================================
    std::size_t __blockSize;

    // ============================================================================
    /// The maximum number of blocks a chunk can allocate
    // ============================================================================
    unsigned char __numBlocks;

    // ============================================================================
    /// Vector of __Chunk objects
    // ============================================================================
    typedef std::vector<__Chunk> __Chunks;
    __Chunks                     __chunks;

    // ============================================================================
    /// Last Chunk used for an allocation
    // ============================================================================
    __Chunks::iterator __allocChunk;

    // ============================================================================
    /// Last Chunk used for a deallocation
    // ============================================================================
    __Chunks::iterator __deallocChunk;
  };

}  // namespace gum

#ifndef GUM_NO_INLINE
#include <agrum/core/smallobjectallocator/fixedAllocator_inl.h>
#endif

#endif  // FIXEDALLOCATOR_H
