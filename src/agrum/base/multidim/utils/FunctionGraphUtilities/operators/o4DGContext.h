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
 * @brief Class used to manipulate o4DGContext in Function Graph Operations
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */

#ifndef O4DGCONTEXT_H
#define O4DGCONTEXT_H

#include <agrum/agrum.h>

#include <agrum/base/multidim/implementations/multiDimFunctionGraph.h>

namespace gum {

  // clang-format off
  /**
   * @class O4DGContext
   * @headerfile o4DGContext.h <agrum/base/multidim/FunctionGraphUtilities/o4DGContext.h>
   * @ingroup multidim_group
   *
   * @brief Class used to manipulate context during Function Graph Operations
   */
  // clang-format on

  class O4DGContext {
    public:
    // ============================================================================
    /// @name Constructors, Destructors.
    // ============================================================================
    /// @{

    /**
     * @brief Default constructor.
     */
    O4DGContext(Idx*, Idx);

    /**
     * @brief Default destructor.
     */
    ~O4DGContext();

    /**
     * @brief Operator new overload to use the SmallObjectAllocator
     */
    void* operator new(size_t s);

    /**
     * @brief Operator delete overload to use the SmallObjectAllocator
     */
    void operator delete(void* p);
    /// @}

    // ============================================================================
    /// @name Diagrams current nodes setters & getters.
    // ============================================================================

    /// @{
    /// Set DG1 diagram current explored Node
    void setDG1Node(const NodeId&);

    /// Get DG1 diagram current explored Node
    const NodeId& DG1Node() const { return _DG1ExploredNode_; }

    /// Set DG2 diagram current explored Node
    void setDG2Node(const NodeId&);

    /// Get DG2 diagram current explored Node
    const NodeId& DG2Node() const { return _DG2ExploredNode_; }

    /// @}

    // ============================================================================
    /// @name Variables modalities handlers
    // ============================================================================
    /// @{

    /// Changes given variable modality
    void chgVarModality(Idx, Idx);

    /// Changes given variable modality
    Idx varModality(Idx);

    /// @}
    // ============================================================================
    /// @name O4DG Handling methods
    // ============================================================================
    /// @{

    /// Returns o4DGContext key
    const double& key(short int* instNeeded);

    /// @}

    private:
    /// DG1 Diagram current explored node
    NodeId _DG1ExploredNode_;

    /// DG2 Diagram current explored node
    NodeId _DG2ExploredNode_;

    /// Vector containing for each retrograde variable its current modality
    /// 0 meaning no instantiation done => there's an offset
    Idx* _varInstantiation_;
    Idx  _nbVar_;

    /// The key use to store the context as a key in the hashtable
    double _key_;

    /// Table containing the log2 of prime numbers
    static const double _logPrime_[];
    static const Idx    _nbLogPrime_, _offsetv_, _offset1_, _offset2_;
  };
}   // namespace gum

#ifndef GUM_NO_INLINE
#  include <agrum/base/multidim/utils/FunctionGraphUtilities/operators/o4DGContext_inl.h>
#endif   /* GUM_NO_INLINE */

#endif   // O4DGCONTEXT_H
