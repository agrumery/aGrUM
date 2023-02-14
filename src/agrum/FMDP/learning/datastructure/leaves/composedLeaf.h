/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


  class ComposedLeaf: public AbstractLeaf {
    public:
    // ==========================================================================
    /// @name Constructor & destructor.
    // ==========================================================================
    /// @{

    // ###################################################################
    /// Default constructor
    // ###################################################################
    ComposedLeaf(NodeId leafId, AbstractLeaf* l1, AbstractLeaf* l2) :
        AbstractLeaf(leafId), _l1_(l1), _l2_(l2) {
      GUM_CONSTRUCTOR(ComposedLeaf);
    }

    // ###################################################################
    /// Default destructor
    // ###################################################################
    ~ComposedLeaf() {
      GUM_DESTRUCTOR(ComposedLeaf);
      ;
    }

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new(size_t s) { return SmallObjectAllocator::instance().allocate(s); }
    void  operator delete(void* p) {
      SmallObjectAllocator::instance().deallocate(p, sizeof(ComposedLeaf));
    }

    /// @}

    // ###################################################################
    /// Gaves the leaf effectif for given modality
    // ###################################################################
    double effectif(Idx moda) const { return _l1_->effectif(moda) + _l2_->effectif(moda); }
    double total() const { return _l1_->total() + _l2_->total(); }

    // ###################################################################
    /// Returns true if abstractleaf has leaf in it
    // ###################################################################
    bool contains(NodeId testedId) const {
      return AbstractLeaf::contains(testedId) || _l1_->contains(testedId)
          || _l2_->contains(testedId);
    }

    Idx nbModa() const { return _l1_->nbModa(); }

    std::string toString();

    private:
    AbstractLeaf* _l1_;
    AbstractLeaf* _l2_;
  };


} /* namespace gum */


#endif   // GUM_COMPOSED_LEAF_H
