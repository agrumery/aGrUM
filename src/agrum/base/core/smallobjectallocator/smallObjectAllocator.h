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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/





/**
 * @file
 * @brief Headers of gum::SmallObjectAllocator
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 *
 */
#ifndef GUM_SMALL_OBJECT_ALLOCATOR_H
#define GUM_SMALL_OBJECT_ALLOCATOR_H

// ============================================================================
#include <agrum/agrum.h>
// ============================================================================
#include <agrum/base/core/hashTable.h>
#include <agrum/base/core/smallobjectallocator/fixedAllocator.h>

// ============================================================================


namespace gum {
  /**
   * @class SmallObjectAllocator smallObjectAllocator.h
   * <agrum/base/core/smallObjectAllocator.h>
   *
   * @brief Allocates objects of any size
   *
   * SmallObjectAllocator does so by aggregating several FixedAllocator objects.
   * When SmallObjectAllocator receives an allocation request, it either forwards
   * it to the best matching FixedAllocator object or passes it to the default
   * operator new
   *
   * @ingroup core
   */
  class SmallObjectAllocator {
    public:
    /**
     * @param The default size of chunck of memory.
     * These chuncks are pre-allocated memory space which are
     * then split in small memory space of the size of a small object
     */
    static const size_t GUM_DEFAULT_CHUNK_SIZE;

    /**
     * @param The default maximal size under which an object is considered
     * small.
     * If an object size is over this limit, the normal new allocator is called.
     */
    static const size_t GUM_DEFAULT_MAX_OBJECT_SIZE;


    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    private:
    // ============================================================================
    /**
     * Constructor.
     * Greater object than maxObjectSize will be forwarded to op new.
     */
    // ============================================================================
    SmallObjectAllocator();

    // ============================================================================
    /// Copy Constructor (does nothing since we use a Singleton)
    // ============================================================================
    SmallObjectAllocator(const SmallObjectAllocator&) {};

    // ============================================================================
    /// Operator = (does nothing since we use a Singleton)
    // ============================================================================
    SmallObjectAllocator& operator=(const SmallObjectAllocator&) { return instance(); }

    public:
    // ============================================================================
    /// Destructor.
    // ============================================================================
    virtual ~SmallObjectAllocator();

    /// @}

    static SmallObjectAllocator& instance();

    // ############################################################################
    /// @name Allocator / Deallocator
    // ############################################################################
    /// @{
    // ============================================================================
    /// Allocates a block
    // ============================================================================
    void* allocate(const size_t& objectSize);

    // ============================================================================
    /// Deallocates an object
    /// @param pDeallocatedObject is the object to be deallocated
    /// @param objectSize is the size of that object (useful for faster
    /// deallocation)
    // ============================================================================
    void deallocate(void* pDeallocatedObject, const size_t& objectSize);

    /// @}

    // ============================================================================
    /// Displays the number of allocation and deallocation made so far
    // ============================================================================
    void displayStats() {
      GUM_TRACE("Nb Small Allocation : " << nbAllocation
                                         << " -  Nb Small Deallocation : " << nbDeallocation);
    }

    Idx nbAlloc() { return nbAllocation; }

    Idx nbDealloc() { return nbDeallocation; }

    private:
    // ============================================================================
    /// The pool containing FixedAllocator
    // ============================================================================
    using _Pool_ = HashTable< Size, FixedAllocator* >;

    _Pool_ _pool_;

    // ============================================================================
    /// The memory that a chunk allocates
    // ============================================================================
    std::size_t _chunkSize_;

    // ============================================================================
    /// The maximal size of an object befor new is called
    // ============================================================================
    std::size_t _maxObjectSize_;

    Idx nbAllocation;
    Idx nbDeallocation;
  };
}   // namespace gum

// Macro used to shorten code in classes using SmallObjectAllocator
#define SOA_ALLOCATE(x)      SmallObjectAllocator::instance().allocate(x)
#define SOA_DEALLOCATE(x, y) SmallObjectAllocator::instance().deallocate(x, y)

#ifndef GUM_NO_INLINE
#  include <agrum/base/core/smallobjectallocator/smallObjectAllocator_inl.h>
#endif

#endif   // GUM_SMALL_OBJECT_ALLOCATOR_H
