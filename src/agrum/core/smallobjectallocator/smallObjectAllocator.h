
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
 * @brief Headers of gum::SmallObjectAllocator
 *
 * @author Jean-Christophe Magnan and Pierre-Henri WUILLEMIN
 *
 */
#ifndef GUM_SMALL_OBJECT_ALLOCATOR_H
#define GUM_SMALL_OBJECT_ALLOCATOR_H

// ============================================================================
#include <agrum/config.h>
// ============================================================================
#include <agrum/core/hashTable.h>
#include <agrum/core/smallobjectallocator/fixedAllocator.h>
// ============================================================================


namespace gum {
  /**
   * @class SmallObjectAllocator smallObjectAllocator.h
   * <agrum/core/smallObjectAllocator.h>
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
    SmallObjectAllocator( const SmallObjectAllocator& )  {};

    // ============================================================================
    /// Operator = (does nothing since we use a Singleton)
    // ============================================================================
    SmallObjectAllocator& operator=( const SmallObjectAllocator& ) {
      return instance();
    }

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
    void* allocate( const size_t& objectSize );

    // ============================================================================
    /// Deallocates an object
    /// @param pDeallocatedObject is the object to be deallocated
    /// @param objectSize is the size of that object (useful for faster
    /// deallocation)
    // ============================================================================
    void deallocate( void* pDeallocatedObject, const size_t& objectSize );

    /// @}

    // ============================================================================
    /// Displays the number of allocation and deallocation made so far
    // ============================================================================
    void displayStats() {
      GUM_TRACE( "Nb Small Allocation : " << nbAllocation
                                          << " -  Nb Small Deallocation : "
                                          << nbDeallocation );
    }

    Idx nbAlloc() { return nbAllocation; }
    Idx nbDealloc() { return nbDeallocation; }

    private:
    // ============================================================================
    /// The pool containing FixedAllocator
    // ============================================================================
    typedef HashTable<Size, FixedAllocator*> __Pool;
    __Pool __pool;

    // ============================================================================
    /// The memory that a chunk allocates
    // ============================================================================
    std::size_t __chunkSize;

    // ============================================================================
    /// The maximal size of an object befor new is called
    // ============================================================================
    std::size_t __maxObjectSize;

    Idx nbAllocation;
    Idx nbDeallocation;
  };
}  // namespace gum

// Macro used to shorten code in classes using SmallObjectAllocator
#define SOA_ALLOCATE( x ) SmallObjectAllocator::instance().allocate( x )
#define SOA_DEALLOCATE( x, y ) SmallObjectAllocator::instance().deallocate( x, y )

#ifndef GUM_NO_INLINE
#include <agrum/core/smallobjectallocator/smallObjectAllocator_inl.h>
#endif

#endif  // GUM_SMALL_OBJECT_ALLOCATOR_H
