/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Headers of the MDDOperatorStrategy planer class.
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(@AMU)
 */

// =========================================================================
#ifndef GUM_ACTION_SET_H
#define GUM_ACTION_SET_H
// =========================================================================
#include <thread>
// =========================================================================
#include <agrum/tools/core/argMaxSet.h>
#include <agrum/tools/core/functors.h>
#include <agrum/tools/core/inline.h>
#include <agrum/tools/core/smallobjectallocator/smallObjectAllocator.h>
// =========================================================================
#include <agrum/tools/multidim/implementations/multiDimFunctionGraph.h>
#include <agrum/tools/multidim/utils/FunctionGraphUtilities/terminalNodePolicies/SetTerminalNodePolicy.h>
// =========================================================================
#include <agrum/FMDP/fmdp.h>
// =========================================================================

namespace gum {

  /**
   * @struct ArgumentMaximisesAction actionSet.h
   * <agrum/FMDP/planning/actionSet.h>
   * @brief Argument Maximization function object class
   * @ingroup core
   *
   * Returns the set that has the maximal value between its two arguments sets
   */
  template < typename GUM_SCALAR >
  struct ArgumentMaximisesAction {
    // ###########################################################################
    /// @name Operator()
    // ###########################################################################
    /// @{

    const GUM_SCALAR& operator()(const GUM_SCALAR& x, const GUM_SCALAR& y) const {
      if (x > y) { return x; }
      if (x < y) { return y; }

      temp__ = x;
      temp__ += y;
      return temp__;
    }

    private:
    mutable GUM_SCALAR temp__;
  };


  /**
   * @class ActionSet
   * @headerfile actionSet.h <agrum/FMDP/planning/actionSet.h>
   * @brief A class to store the optimal actions.
   * @ingroup fmdp_group
   *
   * Stores the ids of optimal actions. To be used as leaves on optimal policy
   * tree or function graph
   *
   */
  class ActionSet {
    public:
    // ###########################################################################
    /// @name CNL
    // ###########################################################################
    /// @{

    // ============================================================================
    /// Constructor
    // ============================================================================
    ActionSet() {
      GUM_CONSTRUCTOR(ActionSet);
      actionSeq__ = new Sequence< Idx >();
    }

    ActionSet(const ActionSet& src) {
      GUM_CONSTRUCTOR(ActionSet);
      actionSeq__ = new Sequence< Idx >();
      for (auto idi = src.beginSafe(); idi != src.endSafe(); ++idi)
        actionSeq__->insert(*idi);
    }

    ActionSet& operator=(const ActionSet& src) {
      actionSeq__ = new Sequence< Idx >();
      for (auto idi = src.beginSafe(); idi != src.endSafe(); ++idi)
        actionSeq__->insert(*idi);
      return *this;
    }

    // ============================================================================
    /// Destructor
    // ============================================================================
    ~ActionSet() {
      GUM_DESTRUCTOR(ActionSet);
      delete actionSeq__;
    }

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new(size_t s) {
      return SmallObjectAllocator::instance().allocate(s);
    }
    void operator delete(void* p) {
      SmallObjectAllocator::instance().deallocate(p, sizeof(ActionSet));
    }

    /// @}

    // ###########################################################################
    /// @name Iterators
    // ###########################################################################
    /// @{

    // ============================================================================
    /// Iterator beginning
    // ============================================================================
    SequenceIteratorSafe< Idx > beginSafe() const {
      return actionSeq__->beginSafe();
    }

    // ============================================================================
    /// Iterator end
    // ============================================================================
    SequenceIteratorSafe< Idx > endSafe() const { return actionSeq__->endSafe(); }

    /// @}

    // ###########################################################################
    /// @name Operators
    // ###########################################################################
    /// @{

    // ============================================================================
    /// Ajout d'un élément
    // ============================================================================
    ActionSet& operator+=(const Idx& elem) {
      actionSeq__->insert(elem);
      return *this;
    }

    // ============================================================================
    /// Use to insert the content of another set inside this one
    // ============================================================================
    ActionSet& operator+=(const ActionSet& src) {
      for (auto iter = src.beginSafe(); iter != src.endSafe(); ++iter)
        if (!actionSeq__->exists(*iter)) actionSeq__->insert(*iter);
      return *this;
    }

    // ============================================================================
    /// Use to insert the content of another set inside this one
    // ============================================================================
    ActionSet& operator-=(const ActionSet& src) {
      for (auto iter = src.beginSafe(); iter != src.endSafe(); ++iter)
        if (actionSeq__->exists(*iter)) actionSeq__->erase(*iter);
      return *this;
    }

    // ============================================================================
    /// Gives the ith element
    // ============================================================================
    const Idx& operator[](const Idx i) const { return actionSeq__->atPos(i); }

    // ============================================================================
    /// Compares two ActionSet to check if they are equals
    // ============================================================================
    bool operator==(const ActionSet& compared) const {
      for (auto iter = compared.beginSafe(); iter != compared.endSafe(); ++iter)
        if (!actionSeq__->exists(*iter)) return false;
      for (auto iter = this->beginSafe(); iter != this->endSafe(); ++iter)
        if (!compared.exists(*iter)) return false;
      return true;
    }
    bool operator!=(const ActionSet& compared) const {
      return !(*this == compared);
    }

    /// @}

    // ============================================================================
    /// Gives the size
    // ============================================================================
    Size size() const { return actionSeq__->size(); }

    bool exists(const Idx& elem) const { return actionSeq__->exists(elem); }

    private:
    /// The very bone of the ActionSet
    Sequence< Idx >* actionSeq__;

    friend std::ostream& operator<<(std::ostream& streamy, const ActionSet& objy) {
      streamy << objy.actionSeq__->toString();
      return streamy;
    }
  };
}   // namespace gum
#endif   // GUM_ACTION_SET_H
