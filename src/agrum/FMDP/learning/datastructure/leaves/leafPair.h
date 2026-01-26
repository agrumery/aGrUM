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
 * @brief Headers of the leaf pair class.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#ifndef GUM_LEAF_PAIR_H
#define GUM_LEAF_PAIR_H
// =========================================================================
// =========================================================================
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
    LeafPair(AbstractLeaf* l1, AbstractLeaf* l2) : _l1_(l1), _l2_(l2) { GUM_CONSTRUCTOR(LeafPair); }

    // ###################################################################
    /// Default destructor
    // ###################################################################
    ~LeafPair() {
      GUM_DESTRUCTOR(LeafPair);
      ;
    }

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new(size_t s) { return SmallObjectAllocator::instance().allocate(s); }

    void operator delete(void* p) {
      SmallObjectAllocator::instance().deallocate(p, sizeof(LeafPair));
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
    AbstractLeaf* firstLeaf() { return _l1_; }

    AbstractLeaf* secondLeaf() { return _l2_; }

    // ###################################################################
    /// Returns true if pair has leaf in it
    // ###################################################################
    bool contains(NodeId testedId) { return _l1_->contains(testedId) || _l2_->contains(testedId); }

    // ###################################################################
    /// Returns a leaf matching data and having given id as id
    // ###################################################################
    AbstractLeaf* convert2Leaf(NodeId leafId) const { return new ComposedLeaf(leafId, _l1_, _l2_); }

    AbstractLeaf* otherLeaf(AbstractLeaf* l) const { return l == _l1_ ? _l2_ : _l1_; }

    std::string toString();

    private:
    AbstractLeaf* _l1_;
    AbstractLeaf* _l2_;

    double _likelyhood1_;
    double _likelyhood2_;
  };


} /* namespace gum */


#endif   // GUM_LEAF_PAIR_H
