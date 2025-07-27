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
 * @brief Headers of the abstract Leaf class.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#ifndef GUM_ABSTRACT_LEAF_H
#define GUM_ABSTRACT_LEAF_H
// =========================================================================
#include <agrum/base/core/multiPriorityQueue.h>
#include <agrum/base/core/sequence.h>
#include <agrum/base/core/smallobjectallocator/smallObjectAllocator.h>
// =========================================================================
#include <agrum/base/graphs/graphElements.h>

// =========================================================================

namespace gum {

  /**
   * @class AbstractLeaf abstractLeaf.h
   * <agrum/FMDP/learning/datastructure/leaves/abstractLeaf.h>
   * @brief Abstract Class implementing a Leaf
   * @ingroup fmdp_group
   *
   */


  class AbstractLeaf {
    public:
    // ==========================================================================
    /// @name Constructor & destructor.
    // ==========================================================================
    /// @{

    // ###################################################################
    /// Default constructor
    // ###################################################################
    AbstractLeaf(NodeId leafId) : _leafId_(leafId) { GUM_CONSTRUCTOR(AbstractLeaf); }

    // ###################################################################
    /// Default destructor
    // ###################################################################
    virtual ~AbstractLeaf() {
      GUM_DESTRUCTOR(AbstractLeaf);
      ;
    }

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new(size_t s) { return SmallObjectAllocator::instance().allocate(s); }

    void operator delete(void* p) {
      SmallObjectAllocator::instance().deallocate(p, sizeof(AbstractLeaf));
    }

    /// @}

    // ###################################################################
    /// Gaves the leaf effectif for given modality
    // ###################################################################
    virtual double effectif(Idx) const = 0;
    virtual double total() const       = 0;

    // ###################################################################
    /// Returns true if abstractleaf has leaf in it
    // ###################################################################
    virtual bool contains(NodeId testedId) const { return _leafId_ == testedId; }

    NodeId id() { return _leafId_; }

    virtual Idx nbModa() const = 0;

    virtual std::string toString() = 0;

    private:
    NodeId _leafId_;
  };


} /* namespace gum */


#endif   // GUM_ABSTRACT_LEAF_H
