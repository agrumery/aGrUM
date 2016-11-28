/*********************************************************************************
 * Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES *
 * {prenom.nom}_at_lip6.fr *
 *                                                                               *
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or *
 * (at your option) any later version. *
 *                                                                               *
 * This program is distributed in the hope that it will be useful, *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the *
 * GNU General Public License for more details. *
 *                                                                               *
 * You should have received a copy of the GNU General Public License *
 * along with this program; if not, write to the *
 * Free Software Foundation, Inc., *
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. *
 *********************************************************************************/
/**
* @file
* @brief Templates for fusion context class
*
* @author Jean-Christophe Magnan
*/
// =======================================================
#include <cmath>
// =======================================================
#include <agrum/FMDP/learning/datastructure/leaves/fusionContext.h>
// =======================================================

namespace gum {

  // ############################################################################
  // Constructors / Destructors
  // ############################################################################

  // ============================================================================
  // Default constructor.
  // ============================================================================
  template <bool isInitial>
  FusionContext<isInitial>::FusionContext( AbstractLeaf* leaf )
      : __leaf( leaf ) {
    GUM_CONSTRUCTOR( FusionContext )
  }

  // ============================================================================
  // Default constructor.
  // ============================================================================
  template <bool isInitial>
  FusionContext<isInitial>::~FusionContext() {
    GUM_DESTRUCTOR( FusionContext )

    for ( auto leafIter = __leaf2Pair.beginSafe();
          leafIter != __leaf2Pair.endSafe();
          ++leafIter )
      delete leafIter.val();

    delete __leaf;
  }

  // ############################################################################
  //
  // ############################################################################

  // ============================================================================
  //
  // ============================================================================
  template <bool isInitial>
  bool FusionContext<isInitial>::__associateLeaf( AbstractLeaf* l,
                                                  Int2Type<false> ) {

    LeafPair* ptop = __pairsHeap.empty() ? nullptr : __pairsHeap.top();
    ;
    LeafPair* p = new LeafPair( l, __leaf );
    __leaf2Pair.insert( l, p );
    __leaf2Pair[l]->updateLikelyhood();
    __pairsHeap.insert( p, p->likelyhood() );

    return ptop != __pairsHeap.top();
  }

  // ============================================================================
  //
  // ============================================================================
  template <bool isInitial>
  bool FusionContext<isInitial>::__updateAssociatedLeaf( AbstractLeaf* l,
                                                         Int2Type<false> ) {

    LeafPair* ptop = __pairsHeap.empty() ? nullptr : __pairsHeap.top();
    ;
    __leaf2Pair[l]->updateLikelyhood();
    __pairsHeap.setPriority( __leaf2Pair[l], __leaf2Pair[l]->likelyhood() );

    return ptop != __pairsHeap.top();
  }

  // ============================================================================
  //
  // ============================================================================
  template <bool isInitial>
  bool FusionContext<isInitial>::__updateAllAssociatedLeaves( Int2Type<false> ) {

    LeafPair* ptop = __pairsHeap.empty() ? nullptr : __pairsHeap.top();
    ;
    for ( HashTableConstIteratorSafe<AbstractLeaf*, LeafPair*> pairIter =
              __leaf2Pair.cbeginSafe();
          pairIter != __leaf2Pair.cendSafe();
          ++pairIter ) {
      pairIter.val()->updateLikelyhood();
      __pairsHeap.setPriority( pairIter.val(), pairIter.val()->likelyhood() );
    }
    LeafPair* ctop = __pairsHeap.empty() ? nullptr : __pairsHeap.top();

    return ptop != ctop;
  }

  // ============================================================================
  //
  // ============================================================================
  template <bool isInitial>
  bool FusionContext<isInitial>::__deassociateLeaf( AbstractLeaf* l,
                                                    Int2Type<false> ) {

    LeafPair* ptop = __pairsHeap.empty() ? nullptr : __pairsHeap.top();
    __pairsHeap.erase( __leaf2Pair[l] );
    __leaf2Pair.erase( l );

    LeafPair* ctop = __pairsHeap.empty() ? nullptr : __pairsHeap.top();

    return ptop != ctop;
  }


  // ############################################################################
  //
  // ############################################################################

  // ============================================================================
  //
  // ============================================================================
  template <bool isInitial>
  bool FusionContext<isInitial>::addPair( LeafPair* p ) {

    LeafPair* ptop = __pairsHeap.empty() ? nullptr : __pairsHeap.top();
    __pairsHeap.insert( p, p->likelyhood() );

    return ptop != __pairsHeap.top();
  }

  // ============================================================================
  //
  // ============================================================================
  template <bool isInitial>
  bool FusionContext<isInitial>::updatePair( LeafPair* p ) {

    LeafPair* ptop = __pairsHeap.empty() ? nullptr : __pairsHeap.top();
    __pairsHeap.setPriority( p, p->likelyhood() );

    return ptop != __pairsHeap.top();
  }

  // ============================================================================
  //
  // ============================================================================
  template <bool isInitial>
  bool FusionContext<isInitial>::removePair( LeafPair* p ) {

    LeafPair* ptop = __pairsHeap.empty() ? nullptr : __pairsHeap.top();
    __pairsHeap.erase( p );

    LeafPair* ctop = __pairsHeap.empty() ? nullptr : __pairsHeap.top();

    return ptop != ctop;
  }


  // ############################################################################
  //
  // ############################################################################

  // ============================================================================
  //
  // ============================================================================
  template <bool isInitial>
  Set<LeafPair*> FusionContext<isInitial>::__associatedPairs( Int2Type<false> ) {

    Set<LeafPair*> retBag;
    for ( auto pairIter = __leaf2Pair.beginSafe();
          pairIter != __leaf2Pair.endSafe();
          ++pairIter )
      retBag << pairIter.val();

    return retBag;
  }


  template <bool isInitial>
  std::string    FusionContext<isInitial>::toString() {

    std::stringstream ss;
    if ( __leaf )
      ss << "Associated Leaf : " << __leaf->toString() << std::endl
         << "Leaves Heap : " << std::endl;

    //      for( HashTableConstIteratorSafe<LeafPair*, std::vector<Size>>
    //      leafIter = __pairsHeap.allValues().cbeginSafe();
    //           leafIter != __pairsHeap.allValues().cendSafe(); ++leafIter ){
    //          ss << leafIter.key()->toString() << std::endl;
    //      }
    if ( !__pairsHeap.empty() )
      ss << "Top pair : " << __pairsHeap.top()->toString() << std::endl;

    return ss.str();
  }

}  // end gum namespace
