
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
 * @brief Inlines of gum::SmallObjectAllocator
 *
 * @author Jean-Christophe Magnan
 *
 */
// ============================================================================
#include <agrum/core/smallobjectallocator/fixedAllocator.h>
#include <agrum/core/smallobjectallocator/smallObjectAllocator.h>
// ============================================================================


namespace gum {
  // ############################################################################
  // @name Constructors / Destructors
  // ############################################################################

    // ============================================================================
    /*
     * Constructor.
     * @param chunkSize is the size of a chunk in bytes.
     * @param maxObjectSize is the max size of object to be considered small
     * Greater object than maxObjectSize will be forwarded to op new.
     */
    // ============================================================================
    INLINE SmallObjectAllocator::SmallObjectAllocator(const std::size_t& chunkSize, const std::size_t& maxObjectSize) :
        __chunkSize(chunkSize), __maxObjectSize(maxObjectSize){
      __pool.setKeyUniquenessPolicy(false);
      GUM_CONSTRUCTOR(SmallObjectAllocator)
      nbAllocation = 0;
      nbDeallocation = 0;
    }

    // ============================================================================
    // Destructor.
    // ============================================================================
    INLINE SmallObjectAllocator::~SmallObjectAllocator(){
      GUM_DESTRUCTOR(SmallObjectAllocator)
//      for(__Pool::iterator pit = __pool.begin(); pit != __pool.end(); ++pit)
//        delete &*pit;
              std::cout << "Nb Small Allocation : " << nbAllocation  << " -  Nb Small Deallocation : " << nbDeallocation << std::endl;
    }

  // ############################################################################
  // @name Allocator / Deallocator
  // ############################################################################

    // ============================================================================
    // Allocates an object
    // ============================================================================
    INLINE void* SmallObjectAllocator::allocate(const std::size_t& objectSize){
      // If objectSize is greater than maxObjectSize, normal new is called
      if( objectSize > __maxObjectSize )
          return new unsigned char[objectSize];

      //
      if(!__pool.exists(objectSize)){
        // Calcul du nombre de block par chunk pour des objets de cette taille
        std::size_t numBlocks = __chunkSize / objectSize;
        if( numBlocks > UCHAR_MAX )
          numBlocks = UCHAR_MAX;
        FixedAllocator* newFa = new FixedAllocator(objectSize, numBlocks);
        __pool.set(objectSize, *newFa);
      }
      nbAllocation++;

      return __pool[objectSize].allocate();
    }

    // ============================================================================
    // Deallocates an object
    // @param pDeallocatedObject is the object to be deallocated
    // @param objectSize is the size of that object (useful for faster deallocation)
    // ============================================================================
    INLINE void SmallObjectAllocator::deallocate(void* pDeallocatedObject, const std::size_t &objectSize){
      __pool[objectSize].deallocate(pDeallocatedObject);
      nbDeallocation++;
    }

} // namespace gum
