
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
 * @author Jean-Christophe Magnan and Pierre-Henri WUILLEMIN
 *
 */
// ============================================================================
#include <assert.h>
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
  INLINE SmallObjectAllocator::SmallObjectAllocator()
      : __chunkSize( GUM_DEFAULT_CHUNK_SIZE )
      , __maxObjectSize( GUM_DEFAULT_MAX_OBJECT_SIZE ) {
    __pool.setKeyUniquenessPolicy( false );
    GUM_CONSTRUCTOR( SmallObjectAllocator );
    nbAllocation = 0;
    nbDeallocation = 0;

    // SmallObjectAllocator::Instance will create a static SmallObjectAllocator and
    // a HashTable that will not be deleted ...
    // so we inform our leak detector not to count those 2 objects
    GUM_DESTRUCTOR( SmallObjectAllocator );
    GUM_DESTRUCTOR( HashTable );
  }

  // ============================================================================
  // Destructor.
  // ============================================================================
  INLINE SmallObjectAllocator::~SmallObjectAllocator() {
    GUM_DESTRUCTOR( SmallObjectAllocator )
    for ( __Pool::iterator pit = __pool.begin(); pit != __pool.end(); ++pit )
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
  INLINE void* SmallObjectAllocator::allocate( const size_t& objectSize ) {

    assert( objectSize > 0 &&
            "Small Object Allocator called for an object of size equals to 0" );

    // If objectSize is greater than maxObjectSize, normal new is called
    if ( objectSize > __maxObjectSize ) return new unsigned char[objectSize];

    void*   ret;
#pragma omp critical( soa )
    {
      //
      if ( !__pool.exists( Size( objectSize ) ) ) {
        // Calcul du nombre de block par chunk pour des objets de cette taille
        std::size_t nb = __chunkSize / Size( objectSize );
        if ( nb > UCHAR_MAX ) nb = UCHAR_MAX;
        unsigned char numBlocks = static_cast<unsigned char>( nb );

        FixedAllocator* newFa =
            new FixedAllocator( Size( objectSize ), numBlocks );
        __pool.set( Size( objectSize ), newFa );
      }
      nbAllocation++;

      ret = __pool[Size( objectSize )]->allocate();
    }
    return ret;
  }

  // ============================================================================
  // Deallocates an object
  // @param pDeallocatedObject is the object to be deallocated
  // @param objectSize is the size of that object (useful for faster
  // deallocation)
  // ============================================================================
  INLINE void SmallObjectAllocator::deallocate( void*         pDeallocatedObject,
                                                const size_t& objectSize ) {

    assert( objectSize > 0 &&
            "Small Object Allocator called for an object of size equals to 0" );

    // If objectSize is greater than maxObjectSize, normal new is called
    if ( objectSize > __maxObjectSize ) {
      delete[]( unsigned char* ) pDeallocatedObject;
      return;
    }

#pragma omp critical( soa )
    {

      //      std::cout << "Deallocating " << pDeallocatedObject << std::endl;
      __pool[Size( objectSize )]->deallocate( pDeallocatedObject );
      nbDeallocation++;
    }
  }

}  // namespace gum
