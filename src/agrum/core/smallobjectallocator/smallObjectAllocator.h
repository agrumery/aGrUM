
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
 * @author Jean-Christophe Magnan
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
#define GUM_DEFAULT_CHUNK_SIZE 8096
#define GUM_DEFAULT_MAX_OBJECT_SIZE 512


namespace gum {
  /**
   * @class SmallObjectAllocator smallObjectAllocator.h <agrum/core/smallObjectAllocator.h>
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
      // ############################################################################
      /// @name Constructors / Destructors
      // ############################################################################
      /// @{

        // ============================================================================
        /**
         * Constructor.
         * @param chunkSize is the size of a chunk in bytes.
         * @param maxObjectSize is the max size of object to be considered small
         * Greater object than maxObjectSize will be forwarded to op new.
         */
        // ============================================================================
        SmallObjectAllocator(const std::size_t& chunkSize, const std::size_t& maxObjectSize);

        // ============================================================================
        /// Destructor.
        // ============================================================================
        ~SmallObjectAllocator();

      /// @}

      // ############################################################################
      /// @name Allocator / Deallocator
      // ############################################################################
      /// @{

        // ============================================================================
        /// Allocates a block
        // ============================================================================
        void* allocate(const std::size_t &objectSize);

        // ============================================================================
        /// Deallocates an object
        /// @param pDeallocatedObject is the object to be deallocated
        /// @param objectSize is the size of that object (useful for faster deallocation)
        // ============================================================================
        void deallocate(void* pDeallocatedObject, const std::size_t& objectSize);

      /// @}

      // ============================================================================
      /// Displays the number of allocation and deallocation made so far
      // ============================================================================
      void displayStats(){
        std::cout << "Nb Small Allocation : " << nbAllocation  << " -  Nb Small Deallocation : " << nbDeallocation << std::endl;
      }

      Idx nbAlloc(){ return nbAllocation; }
      Idx nbDealloc(){ return nbDeallocation; }


    private:

      // ============================================================================
      /// Research in __pool for the FixedAllocator of objectSize
      /// If no FixedAllocator exists for given object, one is created
      /// @param start to accelerate the search
      /// @param objectSize the one for which we search the FixedAllocator
      // ============================================================================
//      std::vector<FixedAllocator>::iterator __binarySearch(
//              const std::vector<FixedAllocator>::iterator &beginning,
//              const std::size_t& objectSize);

      // ============================================================================
      /// The pool containing FixedAllocator
      // ============================================================================
      typedef HashTable<Size,FixedAllocator*> __Pool;
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
} // namespace gum

#ifndef GUM_NO_INLINE
#include <agrum/core/smallobjectallocator/smallObjectAllocator.inl>
#endif

#endif // GUM_SMALL_OBJECT_ALLOCATOR_H
