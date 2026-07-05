/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
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
    void* operator new(size_t s);

    void operator delete(void* p);

    /// @}

    // ==========================================================================
    /// @name Associated Leaves Handling methods
    // ==========================================================================
    /// @{

    // ###################################################################
    ///
    // ###################################################################

    public:
    bool containsAssociatedLeaf(AbstractLeaf* l);

    private:
    bool _containsAssociatedLeaf_(AbstractLeaf* l, Int2Type< false >);

    bool _containsAssociatedLeaf_(AbstractLeaf*, Int2Type< true >);

    // ###################################################################
    ///
    // ###################################################################

    public:
    bool associateLeaf(AbstractLeaf* l);

    private:
    bool _associateLeaf_(AbstractLeaf*, Int2Type< false >);

    bool _associateLeaf_(AbstractLeaf*, Int2Type< true >);

    // ###################################################################
    ///
    // ###################################################################

    public:
    bool updateAssociatedLeaf(AbstractLeaf* l);

    private:
    bool _updateAssociatedLeaf_(AbstractLeaf*, Int2Type< false >);

    bool _updateAssociatedLeaf_(AbstractLeaf*, Int2Type< true >);

    public:
    bool updateAllAssociatedLeaves();

    private:
    bool _updateAllAssociatedLeaves_(Int2Type< false >);

    bool _updateAllAssociatedLeaves_(Int2Type< true >);

    // ###################################################################
    ///
    /// @warning : won't delete Associated Pair created (because subsequent
    /// fusioncontexts might be using it)
    // ###################################################################

    public:
    bool deassociateLeaf(AbstractLeaf* l);

    private:
    bool _deassociateLeaf_(AbstractLeaf*, Int2Type< false >);

    bool _deassociateLeaf_(AbstractLeaf*, Int2Type< true >);

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

    pair_iterator beginPairs();

    pair_iterator endPairs();

    /// @}

    // ==========================================================================
    /// @name Best Pair access methods
    // ==========================================================================
    /// @{

    // ###################################################################
    ///
    // ###################################################################
    LeafPair* top();

    // ###################################################################
    ///
    // ###################################################################
    double topLikelyhood();

    /// @}

    // ==========================================================================
    /// @name FusionContext Leaf and associated pairs handling methods
    // ==========================================================================
    /// @{

    // ###################################################################
    ///
    // ###################################################################
    AbstractLeaf* leaf();

    // ###################################################################
    ///
    // ###################################################################
    LeafPair* leafAssociatedPair(AbstractLeaf* l);

    // ###################################################################
    ///
    // ###################################################################

    public:
    Set< LeafPair* > associatedPairs();

    private:
    Set< LeafPair* > _associatedPairs_(Int2Type< false >);

    Set< LeafPair* > _associatedPairs_(Int2Type< true >);

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
