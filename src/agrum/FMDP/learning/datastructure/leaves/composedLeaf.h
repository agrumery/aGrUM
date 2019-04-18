
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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


  class ComposedLeaf : public AbstractLeaf {
    public:
    // ==========================================================================
    /// @name Constructor & destructor.
    // ==========================================================================
    /// @{

    // ###################################################################
    /// Default constructor
    // ###################################################################
    ComposedLeaf(NodeId leafId, AbstractLeaf* l1, AbstractLeaf* l2) :
        AbstractLeaf(leafId), __l1(l1), __l2(l2) {
      GUM_CONSTRUCTOR(ComposedLeaf);
    }

    // ###################################################################
    /// Default destructor
    // ###################################################################
    ~ComposedLeaf() { GUM_DESTRUCTOR(ComposedLeaf); }

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new(size_t s) {
      return SmallObjectAllocator::instance().allocate(s);
    }
    void operator delete(void* p) {
      SmallObjectAllocator::instance().deallocate(p, sizeof(ComposedLeaf));
    }

    /// @}

    // ###################################################################
    /// Gaves the leaf effectif for given modality
    // ###################################################################
    double effectif(Idx moda) const {
      return __l1->effectif(moda) + __l2->effectif(moda);
    }
    double total() const { return __l1->total() + __l2->total(); }

    // ###################################################################
    /// Returns true if abstractleaf has leaf in it
    // ###################################################################
    bool contains(NodeId testedId) const {
      return AbstractLeaf::contains(testedId) || __l1->contains(testedId)
             || __l2->contains(testedId);
    }

    Idx nbModa() const { return __l1->nbModa(); }

    std::string toString();

    private:
    AbstractLeaf* __l1;
    AbstractLeaf* __l2;
  };


} /* namespace gum */


#endif   // GUM_COMPOSED_LEAF_H
