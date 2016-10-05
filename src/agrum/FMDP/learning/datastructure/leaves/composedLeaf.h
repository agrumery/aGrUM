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
 * @brief Headers of the abstract leaf class.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#ifndef GUM_COMPOSED_LEAF_H
#define GUM_COMPOSED_LEAF_H
// =========================================================================
#include <agrum/FMDP/learning/datastructure/leaves/abstractLeaf.h>
// =========================================================================

namespace gum {

  /**
   * @class ComposedLeaf composedLeaf.h
   * <agrum/FMDP/learning/datastructure/leaves/composedLeaf.h>
   * @brief A leaf that has leaves inside
   * @ingroup fmdp_group
   *
   */


  class ComposedLeaf : public AbstractLeaf {

    public:
    // ==========================================================================
    /// @name Constructor & destructor.
    // ==========================================================================
    /// @{

    // ###################################################################
    /// Default constructor
    // ###################################################################
    ComposedLeaf( NodeId leafId, AbstractLeaf* l1, AbstractLeaf* l2 )
        : AbstractLeaf( leafId )
        , __l1( l1 )
        , __l2( l2 ) {
      GUM_CONSTRUCTOR( ComposedLeaf )
    }

    // ###################################################################
    /// Default destructor
    // ###################################################################
    ~ComposedLeaf() { GUM_DESTRUCTOR( ComposedLeaf ) }

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new( size_t s ) {
      return SmallObjectAllocator::instance().allocate( s );
    }
    void operator delete( void* p ) {
      SmallObjectAllocator::instance().deallocate( p, sizeof( ComposedLeaf ) );
    }

    /// @}

    // ###################################################################
    /// Gaves the leaf effectif for given modality
    // ###################################################################
    double effectif( Idx moda ) const {
      return __l1->effectif( moda ) + __l2->effectif( moda );
    }
    double total() const { return __l1->total() + __l2->total(); }

    // ###################################################################
    /// Returns true if abstractleaf has leaf in it
    // ###################################################################
    bool contains( NodeId testedId ) const {
      return AbstractLeaf::contains( testedId ) || __l1->contains( testedId ) ||
             __l2->contains( testedId );
    }

    Idx nbModa() const { return __l1->nbModa(); }

    std::string toString();

    private:
    AbstractLeaf* __l1;
    AbstractLeaf* __l2;
  };


} /* namespace gum */


#endif  // GUM_COMPOSED_LEAF_H
