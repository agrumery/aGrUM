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
 * @brief Headers of the leaf pair class.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#ifndef GUM_LEAF_PAIR_H
#define GUM_LEAF_PAIR_H
// =========================================================================
#include <agrum/tools/graphs/graphElements.h>
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
    LeafPair(AbstractLeaf* l1, AbstractLeaf* l2) : l1__(l1), l2__(l2) {
      GUM_CONSTRUCTOR(LeafPair);
    }

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
    void* operator new(size_t s) {
      return SmallObjectAllocator::instance().allocate(s);
    }
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
    AbstractLeaf* firstLeaf() { return l1__; }
    AbstractLeaf* secondLeaf() { return l2__; }

    // ###################################################################
    /// Returns true if pair has leaf in it
    // ###################################################################
    bool contains(NodeId testedId) {
      return l1__->contains(testedId) || l2__->contains(testedId);
    }

    // ###################################################################
    /// Returns a leaf matching data and having given id as id
    // ###################################################################
    AbstractLeaf* convert2Leaf(NodeId leafId) const {
      return new ComposedLeaf(leafId, l1__, l2__);
    }

    AbstractLeaf* otherLeaf(AbstractLeaf* l) const {
      return l == l1__ ? l2__ : l1__;
    }

    std::string toString();

    private:
    AbstractLeaf* l1__;
    AbstractLeaf* l2__;

    double likelyhood1__;
    double likelyhood2__;
  };


} /* namespace gum */


#endif   // GUM_LEAF_PAIR_H
