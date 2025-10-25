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


  class ComposedLeaf final: public AbstractLeaf {
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

    void operator delete(void* p) {
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
