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
 * @brief Headers of the MDDOperatorStrategy planer class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */

// =========================================================================
#ifndef GUM_ACTION_SET_H
#define GUM_ACTION_SET_H
// =========================================================================
#include <thread>
// =========================================================================
#include <agrum/base/core/functors.h>
#include <agrum/base/core/inline.h>
// =========================================================================
#include <agrum/base/multidim/utils/FunctionGraphUtilities/terminalNodePolicies/SetTerminalNodePolicy.h>
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
  template < typename GUM_ELEMENT >
  struct ArgumentMaximisesAction {
    // ###########################################################################
    /// @name Operator()
    // ###########################################################################
    /// @{

    const GUM_ELEMENT& operator()(const GUM_ELEMENT& x, const GUM_ELEMENT& y) const;

    private:
    mutable GUM_ELEMENT _temp_;
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
    ActionSet();

    ActionSet(const ActionSet& src);

    ActionSet& operator=(const ActionSet& src);

    // ============================================================================
    /// Destructor
    // ============================================================================
    ~ActionSet();

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new(size_t s);

    void operator delete(void* p);

    /// @}

    // ###########################################################################
    /// @name Iterators
    // ###########################################################################
    /// @{

    // ============================================================================
    /// Iterator beginning
    // ============================================================================
    SequenceIteratorSafe< Idx > beginSafe() const;

    // ============================================================================
    /// Iterator end
    // ============================================================================
    SequenceIteratorSafe< Idx > endSafe() const;

    /// @}

    // ###########################################################################
    /// @name Operators
    // ###########################################################################
    /// @{

    // ============================================================================
    /// Ajout d'un élément
    // ============================================================================
    ActionSet& operator+=(const Idx& elem);

    // ============================================================================
    /// Use to insert the content of another set inside this one
    // ============================================================================
    ActionSet& operator+=(const ActionSet& src);

    // ============================================================================
    /// Use to insert the content of another set inside this one
    // ============================================================================
    ActionSet& operator-=(const ActionSet& src);

    // ============================================================================
    /// Gives the ith element
    // ============================================================================
    const Idx& operator[](const Idx i) const { return _actionSeq_->atPos(i); }

    // ============================================================================
    /// Compares two ActionSet to check if they are equals
    // ============================================================================
    bool operator==(const ActionSet& compared) const;

    /// @}

    // ============================================================================
    /// Gives the size
    // ============================================================================
    Size size() const;

    bool exists(const Idx& elem) const;

    private:
    /// The very bone of the ActionSet
    Sequence< Idx >* _actionSeq_;

    friend std::ostream& operator<<(std::ostream& streamy, const ActionSet& objy);
  };
}   // namespace gum

#ifndef GUM_NO_INLINE
#  include <agrum/FMDP/planning/actionSet_inl.h>
#endif
#include <agrum/FMDP/planning/actionSet_tpl.h>
#endif   // GUM_ACTION_SET_H
