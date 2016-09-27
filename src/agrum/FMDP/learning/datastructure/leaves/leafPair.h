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
 * @brief Headers of the leaf pair class.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#ifndef GUM_LEAF_PAIR_H
#define GUM_LEAF_PAIR_H
// =========================================================================
#include <agrum/graphs/graphElements.h>
// =========================================================================
#include <agrum/FMDP/learning/datastructure/leaves/abstractLeaf.h>
#include <agrum/FMDP/learning/datastructure/leaves/composedLeaf.h>
// =========================================================================

namespace gum {

  /**
   * @class LeafPair leafPair.h
   * <agrum/FMDP/learning/datastructure/leaves/leafPair.h>
   * @brief Handles a pair of leaves
   * @ingroup fmdp_group
   *
   */


  class LeafPair {

    public:
    // ==========================================================================
    /// @name Constructor & destructor.
    // ==========================================================================
    /// @{

    // ###################################################################
    /// Default constructor
    // ###################################################################
    LeafPair( AbstractLeaf* l1, AbstractLeaf* l2 )
        : __l1( l1 )
        , __l2( l2 ) {
      GUM_CONSTRUCTOR( LeafPair )
    }

    // ###################################################################
    /// Default destructor
    // ###################################################################
    ~LeafPair() { GUM_DESTRUCTOR( LeafPair ) }

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new( size_t s ) {
      return SmallObjectAllocator::instance().allocate( s );
    }
    void operator delete( void* p ) {
      SmallObjectAllocator::instance().deallocate( p, sizeof( LeafPair ) );
    }

    /// @}

    // ==========================================================================
    /// @name Likelyhood Methods
    // ==========================================================================
    /// @{

    // ###################################################################
    /// Updates GStatistic
    // ###################################################################
    void updateLikelyhood();

    // ###################################################################
    /// Updates GStatistic
    // ###################################################################
    double likelyhood();

    /// @}

    // ###################################################################
    ///
    // ###################################################################
    AbstractLeaf* firstLeaf() { return __l1; }
    AbstractLeaf* secondLeaf() { return __l2; }

    // ###################################################################
    /// Returns true if pair has leaf in it
    // ###################################################################
    bool contains( NodeId testedId ) {
      return __l1->contains( testedId ) || __l2->contains( testedId );
    }

    // ###################################################################
    /// Returns a leaf matching data and having given id as id
    // ###################################################################
    AbstractLeaf* convert2Leaf( NodeId leafId ) const {
      return new ComposedLeaf( leafId, __l1, __l2 );
    }

    AbstractLeaf* otherLeaf( AbstractLeaf* l ) const {
      return l == __l1 ? __l2 : __l1;
    }

    std::string toString();

    private:
    AbstractLeaf* __l1;
    AbstractLeaf* __l2;

    double __likelyhood1;
    double __likelyhood2;
  };


} /* namespace gum */


#endif  // GUM_LEAF_PAIR_H
