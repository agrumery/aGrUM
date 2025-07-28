/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Headers of gum::FixedAllocator
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 *
 */
#ifndef GUM_FIXED_ALLOCATOR_H
#define GUM_FIXED_ALLOCATOR_H

// ============================================================================
#include <mutex>
#include <thread>
// ============================================================================
#include <agrum/agrum.h>

// ============================================================================

namespace gum {
  /**
   * @class FixedAllocator
   * @headerfile fixedAllocator.h <agrum/base/core/fixedAllocator.h>
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
    // clang-format off
    /**
     * @struct  _Chunk_ fixedAllocator.h <agrum/base/core/smallobjectallocator/fixedAllocator.h>
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
    // clang-format on

    struct _Chunk_ {
      // ============================================================================
      /// Initializes a Chunk object
      // ============================================================================
      void _init_(const std::size_t& blockSize, const unsigned char& numBlocks);

      // ============================================================================
      /// Allocates a block of memory
      // ============================================================================
      void* _allocate_(const std::size_t& blockSize);

      // ============================================================================
      /// Deallocates a block of memory
      // ============================================================================
      void _deallocat_(void* p, const std::size_t& blockSize);

      // ============================================================================
      /// Releases the allocated memory
      // ============================================================================
      void _release_();

      // ============================================================================
      /// Pointer to the managed memory itself
      // ============================================================================
      unsigned char* _pData_;

      // ============================================================================
      /// Holds the index of the first block available in this chunck
      // ============================================================================
      unsigned char _firstAvailableBlock_;

      // ============================================================================
      /// Number of blocks available in this chunck
      // ============================================================================
      unsigned char _blocksAvailable_;
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
    FixedAllocator(const std::size_t& blockSize, const unsigned char& numBlocks = UCHAR_MAX);

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
    void deallocate(void* pDeallocatedBlock);

    /// @}

    // ============================================================================
    /// Returns the size of block allocated by this FixedAllocator
    // ============================================================================
    const size_t& objectSize() { return _blockSize_; }

    private:
    // ============================================================================
    /// Size of a memory block allocated
    // ============================================================================
    std::size_t _blockSize_;

    // ============================================================================
    /// The maximum number of blocks a chunk can allocate
    // ============================================================================
    unsigned char _numBlocks_;

    // ============================================================================
    /// Vector of  _Chunk_ objects
    // ============================================================================
    using _Chunks_ = std::vector< _Chunk_ >;
    _Chunks_ _chunks_;

    // ============================================================================
    /// Last Chunk used for an allocation
    // ============================================================================
    _Chunks_::iterator _allocChunk_;

    // ============================================================================
    /// Last Chunk used for a deallocation
    // ============================================================================
    _Chunks_::iterator _deallocChunk_;
  };

}   // namespace gum

#ifndef GUM_NO_INLINE
#  include <agrum/base/core/smallobjectallocator/fixedAllocator_inl.h>
#endif

#endif   // FIXEDALLOCATOR_H
