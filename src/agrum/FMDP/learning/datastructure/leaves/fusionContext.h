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
 * @brief Headers of the Fusion Context class.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#ifndef GUM_FUSION_CONTEXT_H
#define GUM_FUSION_CONTEXT_H
// =========================================================================
// =========================================================================
// =========================================================================
#include <agrum/FMDP/learning/core/templateStrategy.h>
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

  using pair_iterator = HashTableConstIteratorSafe< LeafPair*, std::vector< Size > >;

  template < bool isInitial = false >
  class FusionContext {
    public:
    // ==========================================================================
    /// @name Constructor & destructor.
    // ==========================================================================
    /// @{

    // ###################################################################
    /// Default constructor
    // ###################################################################
    FusionContext(AbstractLeaf*);

    // ###################################################################
    /// Default destructor
    // ###################################################################
    ~FusionContext();

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new(size_t s) { return SmallObjectAllocator::instance().allocate(s); }

    void operator delete(void* p) {
      SmallObjectAllocator::instance().deallocate(p, sizeof(FusionContext));
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
    bool containsAssociatedLeaf(AbstractLeaf* l) {
      return _containsAssociatedLeaf_(l, Int2Type< isInitial >());
    }

    private:
    bool _containsAssociatedLeaf_(AbstractLeaf* l, Int2Type< false >) {
      return _leaf2Pair_.exists(l);
    }

    bool _containsAssociatedLeaf_(AbstractLeaf*, Int2Type< true >) { return false; }

    // ###################################################################
    ///
    // ###################################################################

    public:
    bool associateLeaf(AbstractLeaf* l) { return _associateLeaf_(l, Int2Type< isInitial >()); }

    private:
    bool _associateLeaf_(AbstractLeaf*, Int2Type< false >);

    bool _associateLeaf_(AbstractLeaf*, Int2Type< true >) { return false; }

    // ###################################################################
    ///
    // ###################################################################

    public:
    bool updateAssociatedLeaf(AbstractLeaf* l) {
      return _updateAssociatedLeaf_(l, Int2Type< isInitial >());
    }

    private:
    bool _updateAssociatedLeaf_(AbstractLeaf*, Int2Type< false >);

    bool _updateAssociatedLeaf_(AbstractLeaf*, Int2Type< true >) { return false; }

    public:
    bool updateAllAssociatedLeaves() {
      return _updateAllAssociatedLeaves_(Int2Type< isInitial >());
    }

    private:
    bool _updateAllAssociatedLeaves_(Int2Type< false >);

    bool _updateAllAssociatedLeaves_(Int2Type< true >) { return false; }

    // ###################################################################
    ///
    /// @warning : won't delete Associated Pair created (because subsequent
    /// fusioncontexts might be using it)
    // ###################################################################

    public:
    bool deassociateLeaf(AbstractLeaf* l) { return _deassociateLeaf_(l, Int2Type< isInitial >()); }

    private:
    bool _deassociateLeaf_(AbstractLeaf*, Int2Type< false >);

    bool _deassociateLeaf_(AbstractLeaf*, Int2Type< true >) { return false; }

    /// @}

    public:
    // ==========================================================================
    /// @name Pair handling methods
    // ==========================================================================
    /// @{

    // ###################################################################
    ///
    // ###################################################################
    bool addPair(LeafPair* p);

    // ###################################################################
    ///
    // ###################################################################
    bool updatePair(LeafPair* p);

    // ###################################################################
    ///
    // ###################################################################
    bool removePair(LeafPair* p);

    pair_iterator beginPairs() { return _pairsHeap_.allValues().beginSafe(); }

    pair_iterator endPairs() { return _pairsHeap_.allValues().endSafe(); }

    /// @}

    // ==========================================================================
    /// @name Best Pair access methods
    // ==========================================================================
    /// @{

    // ###################################################################
    ///
    // ###################################################################
    LeafPair* top() { return !_pairsHeap_.empty() ? _pairsHeap_.top() : nullptr; }

    // ###################################################################
    ///
    // ###################################################################
    double topLikelyhood() { return !_pairsHeap_.empty() ? _pairsHeap_.topPriority() : 1.0; }

    /// @}

    // ==========================================================================
    /// @name FusionContext Leaf and associated pairs handling methods
    // ==========================================================================
    /// @{

    // ###################################################################
    ///
    // ###################################################################
    AbstractLeaf* leaf() { return _leaf_; }

    // ###################################################################
    ///
    // ###################################################################
    LeafPair* leafAssociatedPair(AbstractLeaf* l) { return _leaf2Pair_.getWithDefault(l, nullptr); }

    // ###################################################################
    ///
    // ###################################################################

    public:
    Set< LeafPair* > associatedPairs() { return _associatedPairs_(Int2Type< isInitial >()); }

    private:
    Set< LeafPair* > _associatedPairs_(Int2Type< false >);

    Set< LeafPair* > _associatedPairs_(Int2Type< true >) { return Set< LeafPair* >(); }

    /// @}

    public:
    std::string toString();

    private:
    MultiPriorityQueue< LeafPair*, double, std::less< double > > _pairsHeap_;

    HashTable< AbstractLeaf*, LeafPair* > _leaf2Pair_;

    AbstractLeaf* _leaf_;
  };


} /* namespace gum */

#include <agrum/FMDP/learning/datastructure/leaves/fusionContext_tpl.h>

#endif   // GUM_FUSION_CONTEXT_H
