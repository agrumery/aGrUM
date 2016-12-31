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
 * @brief Headers of the Fusion Context class.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#ifndef GUM_FUSION_CONTEXT_H
#define GUM_FUSION_CONTEXT_H
// =========================================================================
#include <agrum/core/multiPriorityQueue.h>
// =========================================================================
#include <agrum/graphs/graphElements.h>
// =========================================================================
#include <agrum/FMDP/learning/core/templateStrategy.h>
#include <agrum/FMDP/learning/datastructure/leaves/abstractLeaf.h>
#include <agrum/FMDP/learning/datastructure/leaves/leafPair.h>
// =========================================================================

namespace gum {

  /**
   * @class FusionContext fusionContext.h
   * <agrum/FMDP/learning/datastructure/leaves/fusionContext.h>
   * @brief Contains leaves situation after a merging have been made
   * @ingroup fmdp_group
   *
   */

  typedef HashTableConstIteratorSafe<LeafPair*, std::vector<Size>> pair_iterator;


  template <bool isInitial = false>
  class FusionContext {

    public:
    // ==========================================================================
    /// @name Constructor & destructor.
    // ==========================================================================
    /// @{

    // ###################################################################
    /// Default constructor
    // ###################################################################
    FusionContext( AbstractLeaf* );

    // ###################################################################
    /// Default destructor
    // ###################################################################
    ~FusionContext();

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new( size_t s ) {
      return SmallObjectAllocator::instance().allocate( s );
    }
    void operator delete( void* p ) {
      SmallObjectAllocator::instance().deallocate( p, sizeof( FusionContext ) );
    }

    /// @}

    // ==========================================================================
    /// @name Associated Leaves Handling methods
    // ==========================================================================
    /// @{

    // ###################################################################
    ///
    // ###################################################################
    public:
    bool containsAssociatedLeaf( AbstractLeaf* l ) {
      return __containsAssociatedLeaf( l, Int2Type<isInitial>() );
    }

    private:
    bool __containsAssociatedLeaf( AbstractLeaf* l, Int2Type<false> ) {
      return __leaf2Pair.exists( l );
    }
    bool __containsAssociatedLeaf( AbstractLeaf*, Int2Type<true> ) {
      return false;
    }

    // ###################################################################
    ///
    // ###################################################################
    public:
    bool associateLeaf( AbstractLeaf* l ) {
      return __associateLeaf( l, Int2Type<isInitial>() );
    }

    private:
    bool __associateLeaf( AbstractLeaf*, Int2Type<false> );
    bool __associateLeaf( AbstractLeaf*, Int2Type<true> ) { return false; }


    // ###################################################################
    ///
    // ###################################################################
    public:
    bool updateAssociatedLeaf( AbstractLeaf* l ) {
      return __updateAssociatedLeaf( l, Int2Type<isInitial>() );
    }

    private:
    bool __updateAssociatedLeaf( AbstractLeaf*, Int2Type<false> );
    bool __updateAssociatedLeaf( AbstractLeaf*, Int2Type<true> ) { return false; }

    public:
    bool updateAllAssociatedLeaves() {
      return __updateAllAssociatedLeaves( Int2Type<isInitial>() );
    }

    private:
    bool __updateAllAssociatedLeaves( Int2Type<false> );
    bool __updateAllAssociatedLeaves( Int2Type<true> ) { return false; }


    // ###################################################################
    ///
    /// @warning : won't delete Associated Pair created (because subsequent
    /// fusioncontexts might be using it)
    // ###################################################################
    public:
    bool deassociateLeaf( AbstractLeaf* l ) {
      return __deassociateLeaf( l, Int2Type<isInitial>() );
    }

    private:
    bool __deassociateLeaf( AbstractLeaf*, Int2Type<false> );
    bool __deassociateLeaf( AbstractLeaf*, Int2Type<true> ) { return false; }

    /// @}

    public:
    // ==========================================================================
    /// @name Pair handling methods
    // ==========================================================================
    /// @{

    // ###################################################################
    ///
    // ###################################################################
    bool addPair( LeafPair* p );

    // ###################################################################
    ///
    // ###################################################################
    bool updatePair( LeafPair* p );

    // ###################################################################
    ///
    // ###################################################################
    bool removePair( LeafPair* p );


    pair_iterator beginPairs() { return __pairsHeap.allValues().beginSafe(); }
    pair_iterator endPairs() { return __pairsHeap.allValues().endSafe(); }

    /// @}

    // ==========================================================================
    /// @name Best Pair access methods
    // ==========================================================================
    /// @{

    // ###################################################################
    ///
    // ###################################################################
    LeafPair* top() { return !__pairsHeap.empty() ? __pairsHeap.top() : nullptr; }

    // ###################################################################
    ///
    // ###################################################################
    double topLikelyhood() {
      return !__pairsHeap.empty() ? __pairsHeap.topPriority() : 1.0;
    }

    /// @}

    // ==========================================================================
    /// @name FusionContext Leaf and associated pairs handling methods
    // ==========================================================================
    /// @{

    // ###################################################################
    ///
    // ###################################################################
    AbstractLeaf* leaf() { return __leaf; }

    // ###################################################################
    ///
    // ###################################################################
    LeafPair* leafAssociatedPair( AbstractLeaf* l ) {
      return __leaf2Pair.getWithDefault( l, nullptr );
    }

    // ###################################################################
    ///
    // ###################################################################
    public:
    Set<LeafPair*> associatedPairs() {
      return __associatedPairs( Int2Type<isInitial>() );
    }

    private:
    Set<LeafPair*> __associatedPairs( Int2Type<false> );
    Set<LeafPair*> __associatedPairs( Int2Type<true> ) { return Set<LeafPair*>(); }
    /// @}

    public:
    std::string toString();

    private:
    MultiPriorityQueue<LeafPair*, double, std::less<double>> __pairsHeap;

    HashTable<AbstractLeaf*, LeafPair*> __leaf2Pair;

    AbstractLeaf* __leaf;
  };


} /* namespace gum */

#include <agrum/FMDP/learning/datastructure/leaves/fusionContext_tpl.h>

#endif  // GUM_FUSION_CONTEXT_H
