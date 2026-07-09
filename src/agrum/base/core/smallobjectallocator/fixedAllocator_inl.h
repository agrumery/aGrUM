/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


/**
 * @file
 * @brief Inlines of gum::FixedAllocator
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 *
 */
// ============================================================================
#include <agrum/base/core/smallobjectallocator/fixedAllocator.h>

// ============================================================================

namespace gum {

  // ============================================================================
  // Initializes a Chunk object
  // ============================================================================

  // ============================================================================
  // Allocates a block of memory
  // ============================================================================

  // ============================================================================
  // Deallocates a block of memory
  // ============================================================================

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
    for (_Chunks_::iterator chunkIter = _chunks_.begin(); chunkIter != _chunks_.end();
         ++chunkIter) {
      chunkIter->_release_();
    }
    //    GUM_DESTRUCTOR(FixedAllocator);
  }

  // ############################################################################
  // @name Allocator / Deallocator
  // ############################################################################

  // ============================================================================
  // Allocates a block
  // ============================================================================

  // ============================================================================
  // Deallocates a block
  // ============================================================================

  // ============================================================================
  // Returns the size of block allocated by this FixedAllocator
  // ============================================================================
  INLINE const size_t& FixedAllocator::objectSize() { return _blockSize_; }

}   // namespace gum
