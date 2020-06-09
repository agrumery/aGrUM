
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Inlines of gum::SmallObjectAllocator
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN(@LIP6)
 *
 */
// ============================================================================
#include <agrum/tools/core/smallobjectallocator/fixedAllocator.h>
#include <agrum/tools/core/smallobjectallocator/smallObjectAllocator.h>
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
  INLINE SmallObjectAllocator::SmallObjectAllocator() :
      chunkSize__(GUM_DEFAULT_CHUNK_SIZE),
      maxObjectSize__(GUM_DEFAULT_MAX_OBJECT_SIZE) {
    pool__.setKeyUniquenessPolicy(false);
    GUM_CONSTRUCTOR(SmallObjectAllocator);
    nbAllocation = 0;
    nbDeallocation = 0;

    // SmallObjectAllocator::Instance will create a static SmallObjectAllocator and
    // a HashTable that will not be deleted ...
    // so we inform our leak detector not to count those 2 objects
    GUM_DESTRUCTOR(SmallObjectAllocator);
    GUM_DESTRUCTOR(HashTable);
  }

  // ============================================================================
  // Destructor.
  // ============================================================================
  INLINE SmallObjectAllocator::~SmallObjectAllocator() {
    GUM_DESTRUCTOR(SmallObjectAllocator);
    for (Pool__::iterator pit = pool__.begin(); pit != pool__.end(); ++pit)
      delete pit.val();
  }

  INLINE SmallObjectAllocator& SmallObjectAllocator::instance() {
    static SmallObjectAllocator soa;

    return soa;
  }

  // ############################################################################
  // @name Allocator / Deallocator
  // ############################################################################

  // ============================================================================
  // Allocates an object
  // ============================================================================
  INLINE void* SmallObjectAllocator::allocate(const size_t& objectSize) {
    // Small Object Allocator called for an object of size equals to 0
    GUM_ASSERT(objectSize > 0);

    // If objectSize is greater than maxObjectSize, normal new is called
    if (objectSize > maxObjectSize__) return new unsigned char[objectSize];

    void* ret;
#pragma omp critical(soa)
    {
      //
      if (!pool__.exists(Size(objectSize))) {
        // Calcul du nombre de block par chunk pour des objets de cette taille
        std::size_t nb = chunkSize__ / Size(objectSize);
        if (nb > UCHAR_MAX) nb = UCHAR_MAX;
        unsigned char numBlocks = static_cast< unsigned char >(nb);

        FixedAllocator* newFa = new FixedAllocator(Size(objectSize), numBlocks);
        pool__.set(Size(objectSize), newFa);
      }
      nbAllocation++;

      ret = pool__[Size(objectSize)]->allocate();
    }
    return ret;
  }

  // ============================================================================
  // Deallocates an object
  // @param pDeallocatedObject is the object to be deallocated
  // @param objectSize is the size of that object (useful for faster
  // deallocation)
  // ============================================================================
  INLINE void SmallObjectAllocator::deallocate(void*         pDeallocatedObject,
                                               const size_t& objectSize) {
    // Small Object Allocator called for an object of size equals to 0
    GUM_ASSERT(objectSize > 0);

    // If objectSize is greater than maxObjectSize, normal new is called
    if (objectSize > maxObjectSize__) {
      delete[](unsigned char*) pDeallocatedObject;
      return;
    }

#pragma omp critical(soa)
    {
      //      std::cout << "Deallocating " << pDeallocatedObject << std::endl;
      pool__[Size(objectSize)]->deallocate(pDeallocatedObject);
      nbDeallocation++;
    }
  }

}   // namespace gum
