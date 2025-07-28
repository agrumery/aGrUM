/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Templates for fusion context class
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */
// =======================================================
#include <agrum/FMDP/learning/datastructure/leaves/fusionContext.h>

#include <agrum/base/core/math/math_utils.h>

// =======================================================

namespace gum {

  // ############################################################################
  // Constructors / Destructors
  // ############################################################################

  // ============================================================================
  // Default constructor.
  // ============================================================================
  template < bool isInitial >
  FusionContext< isInitial >::FusionContext(AbstractLeaf* leaf) : _leaf_(leaf) {
    GUM_CONSTRUCTOR(FusionContext);
  }

  // ============================================================================
  // Default constructor.
  // ============================================================================
  template < bool isInitial >
  FusionContext< isInitial >::~FusionContext() {
    GUM_DESTRUCTOR(FusionContext);

    for (auto leafIter = _leaf2Pair_.beginSafe(); leafIter != _leaf2Pair_.endSafe(); ++leafIter)
      delete leafIter.val();

    delete _leaf_;
  }

  // ############################################################################
  //
  // ############################################################################

  // ============================================================================
  //
  // ============================================================================
  template < bool isInitial >
  bool FusionContext< isInitial >::_associateLeaf_(AbstractLeaf* l, Int2Type< false >) {
    LeafPair* ptop = _pairsHeap_.empty() ? nullptr : _pairsHeap_.top();
    ;
    LeafPair* p = new LeafPair(l, _leaf_);
    _leaf2Pair_.insert(l, p);
    _leaf2Pair_[l]->updateLikelyhood();
    _pairsHeap_.insert(p, p->likelyhood());

    return ptop != _pairsHeap_.top();
  }

  // ============================================================================
  //
  // ============================================================================
  template < bool isInitial >
  bool FusionContext< isInitial >::_updateAssociatedLeaf_(AbstractLeaf* l, Int2Type< false >) {
    LeafPair* ptop = _pairsHeap_.empty() ? nullptr : _pairsHeap_.top();
    ;
    _leaf2Pair_[l]->updateLikelyhood();
    _pairsHeap_.setPriority(_leaf2Pair_[l], _leaf2Pair_[l]->likelyhood());

    return ptop != _pairsHeap_.top();
  }

  // ============================================================================
  //
  // ============================================================================
  template < bool isInitial >
  bool FusionContext< isInitial >::_updateAllAssociatedLeaves_(Int2Type< false >) {
    LeafPair* ptop = _pairsHeap_.empty() ? nullptr : _pairsHeap_.top();
    ;
    for (HashTableConstIteratorSafe< AbstractLeaf*, LeafPair* > pairIter = _leaf2Pair_.cbeginSafe();
         pairIter != _leaf2Pair_.cendSafe();
         ++pairIter) {
      pairIter.val()->updateLikelyhood();
      _pairsHeap_.setPriority(pairIter.val(), pairIter.val()->likelyhood());
    }
    LeafPair* ctop = _pairsHeap_.empty() ? nullptr : _pairsHeap_.top();

    return ptop != ctop;
  }

  // ============================================================================
  //
  // ============================================================================
  template < bool isInitial >
  bool FusionContext< isInitial >::_deassociateLeaf_(AbstractLeaf* l, Int2Type< false >) {
    LeafPair* ptop = _pairsHeap_.empty() ? nullptr : _pairsHeap_.top();
    _pairsHeap_.erase(_leaf2Pair_[l]);
    _leaf2Pair_.erase(l);

    LeafPair* ctop = _pairsHeap_.empty() ? nullptr : _pairsHeap_.top();

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
    LeafPair* ptop = _pairsHeap_.empty() ? nullptr : _pairsHeap_.top();
    _pairsHeap_.insert(p, p->likelyhood());

    return ptop != _pairsHeap_.top();
  }

  // ============================================================================
  //
  // ============================================================================
  template < bool isInitial >
  bool FusionContext< isInitial >::updatePair(LeafPair* p) {
    LeafPair* ptop = _pairsHeap_.empty() ? nullptr : _pairsHeap_.top();
    _pairsHeap_.setPriority(p, p->likelyhood());

    return ptop != _pairsHeap_.top();
  }

  // ============================================================================
  //
  // ============================================================================
  template < bool isInitial >
  bool FusionContext< isInitial >::removePair(LeafPair* p) {
    LeafPair* ptop = _pairsHeap_.empty() ? nullptr : _pairsHeap_.top();
    _pairsHeap_.erase(p);

    LeafPair* ctop = _pairsHeap_.empty() ? nullptr : _pairsHeap_.top();

    return ptop != ctop;
  }

  // ############################################################################
  //
  // ############################################################################

  // ============================================================================
  //
  // ============================================================================
  template < bool isInitial >
  Set< LeafPair* > FusionContext< isInitial >::_associatedPairs_(Int2Type< false >) {
    Set< LeafPair* > retBag;
    for (auto pairIter = _leaf2Pair_.beginSafe(); pairIter != _leaf2Pair_.endSafe(); ++pairIter)
      retBag << pairIter.val();

    return retBag;
  }

  template < bool isInitial >
  std::string FusionContext< isInitial >::toString() {
    std::stringstream ss;
    if (_leaf_)
      ss << "Associated Leaf : " << _leaf_->toString() << std::endl
         << "Leaves Heap : " << std::endl;

    //      for( HashTableConstIteratorSafe<LeafPair*, std::vector<Size>>
    //      leafIter =  _pairsHeap_.allValues().cbeginSafe();
    //           leafIter !=  _pairsHeap_.allValues().cendSafe(); ++leafIter ){
    //          ss << leafIter.key()->toString() << std::endl;
    //      }
    if (!_pairsHeap_.empty()) ss << "Top pair : " << _pairsHeap_.top()->toString() << std::endl;

    return ss.str();
  }

}   // namespace gum
