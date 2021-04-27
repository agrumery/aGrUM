/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Templates for fusion context class
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(@AMU)
 */
// =======================================================
#include <agrum/tools/core/math/math_utils.h>
#include <agrum/FMDP/learning/datastructure/leaves/fusionContext.h>
// =======================================================

namespace gum {

  // ############################################################################
  // Constructors / Destructors
  // ############################################################################

  // ============================================================================
  // Default constructor.
  // ============================================================================
  template < bool isInitial >
  FusionContext< isInitial >::FusionContext(AbstractLeaf* leaf) : leaf__(leaf) {
    GUM_CONSTRUCTOR(FusionContext);
  }

  // ============================================================================
  // Default constructor.
  // ============================================================================
  template < bool isInitial >
  FusionContext< isInitial >::~FusionContext() {
    GUM_DESTRUCTOR(FusionContext);

    for (auto leafIter = leaf2Pair__.beginSafe();
         leafIter != leaf2Pair__.endSafe();
         ++leafIter)
      delete leafIter.val();

    delete leaf__;
  }

  // ############################################################################
  //
  // ############################################################################

  // ============================================================================
  //
  // ============================================================================
  template < bool isInitial >
  bool FusionContext< isInitial >::associateLeaf__(AbstractLeaf* l,
                                                   Int2Type< false >) {
    LeafPair* ptop = pairsHeap__.empty() ? nullptr : pairsHeap__.top();
    ;
    LeafPair* p = new LeafPair(l, leaf__);
    leaf2Pair__.insert(l, p);
    leaf2Pair__[l]->updateLikelyhood();
    pairsHeap__.insert(p, p->likelyhood());

    return ptop != pairsHeap__.top();
  }

  // ============================================================================
  //
  // ============================================================================
  template < bool isInitial >
  bool FusionContext< isInitial >::updateAssociatedLeaf__(AbstractLeaf* l,
                                                          Int2Type< false >) {
    LeafPair* ptop = pairsHeap__.empty() ? nullptr : pairsHeap__.top();
    ;
    leaf2Pair__[l]->updateLikelyhood();
    pairsHeap__.setPriority(leaf2Pair__[l], leaf2Pair__[l]->likelyhood());

    return ptop != pairsHeap__.top();
  }

  // ============================================================================
  //
  // ============================================================================
  template < bool isInitial >
  bool FusionContext< isInitial >::updateAllAssociatedLeaves__(Int2Type< false >) {
    LeafPair* ptop = pairsHeap__.empty() ? nullptr : pairsHeap__.top();
    ;
    for (HashTableConstIteratorSafe< AbstractLeaf*, LeafPair* > pairIter
         = leaf2Pair__.cbeginSafe();
         pairIter != leaf2Pair__.cendSafe();
         ++pairIter) {
      pairIter.val()->updateLikelyhood();
      pairsHeap__.setPriority(pairIter.val(), pairIter.val()->likelyhood());
    }
    LeafPair* ctop = pairsHeap__.empty() ? nullptr : pairsHeap__.top();

    return ptop != ctop;
  }

  // ============================================================================
  //
  // ============================================================================
  template < bool isInitial >
  bool FusionContext< isInitial >::deassociateLeaf__(AbstractLeaf* l,
                                                     Int2Type< false >) {
    LeafPair* ptop = pairsHeap__.empty() ? nullptr : pairsHeap__.top();
    pairsHeap__.erase(leaf2Pair__[l]);
    leaf2Pair__.erase(l);

    LeafPair* ctop = pairsHeap__.empty() ? nullptr : pairsHeap__.top();

    return ptop != ctop;
  }


  // ############################################################################
  //
  // ############################################################################

  // ============================================================================
  //
  // ============================================================================
  template < bool isInitial >
  bool FusionContext< isInitial >::addPair(LeafPair* p) {
    LeafPair* ptop = pairsHeap__.empty() ? nullptr : pairsHeap__.top();
    pairsHeap__.insert(p, p->likelyhood());

    return ptop != pairsHeap__.top();
  }

  // ============================================================================
  //
  // ============================================================================
  template < bool isInitial >
  bool FusionContext< isInitial >::updatePair(LeafPair* p) {
    LeafPair* ptop = pairsHeap__.empty() ? nullptr : pairsHeap__.top();
    pairsHeap__.setPriority(p, p->likelyhood());

    return ptop != pairsHeap__.top();
  }

  // ============================================================================
  //
  // ============================================================================
  template < bool isInitial >
  bool FusionContext< isInitial >::removePair(LeafPair* p) {
    LeafPair* ptop = pairsHeap__.empty() ? nullptr : pairsHeap__.top();
    pairsHeap__.erase(p);

    LeafPair* ctop = pairsHeap__.empty() ? nullptr : pairsHeap__.top();

    return ptop != ctop;
  }


  // ############################################################################
  //
  // ############################################################################

  // ============================================================================
  //
  // ============================================================================
  template < bool isInitial >
  Set< LeafPair* >
     FusionContext< isInitial >::associatedPairs__(Int2Type< false >) {
    Set< LeafPair* > retBag;
    for (auto pairIter = leaf2Pair__.beginSafe();
         pairIter != leaf2Pair__.endSafe();
         ++pairIter)
      retBag << pairIter.val();

    return retBag;
  }


  template < bool isInitial >
  std::string FusionContext< isInitial >::toString() {
    std::stringstream ss;
    if (leaf__)
      ss << "Associated Leaf : " << leaf__->toString() << std::endl
         << "Leaves Heap : " << std::endl;

    //      for( HashTableConstIteratorSafe<LeafPair*, std::vector<Size>>
    //      leafIter = pairsHeap__.allValues().cbeginSafe();
    //           leafIter != pairsHeap__.allValues().cendSafe(); ++leafIter ){
    //          ss << leafIter.key()->toString() << std::endl;
    //      }
    if (!pairsHeap__.empty())
      ss << "Top pair : " << pairsHeap__.top()->toString() << std::endl;

    return ss.str();
  }

}   // namespace gum
