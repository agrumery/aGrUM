/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
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
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief the structural constraint for forbidding the creation of some arcs
 * during structure learning
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/BN/learning/constraints/structuralConstraintNoParentNodes.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#  include <agrum/BN/learning/constraints/structuralConstraintNoParentNodes_inl.h>
#endif /* GUM_NO_INLINE */


namespace gum::learning {

  /// default constructor
  StructuralConstraintNoParentNodes::StructuralConstraintNoParentNodes() {
    GUM_CONSTRUCTOR(StructuralConstraintNoParentNodes);
  }

  /// constructor starting with a given graph
  StructuralConstraintNoParentNodes::StructuralConstraintNoParentNodes(const DiGraph& graph) {
    setGraph(graph);
    GUM_CONSTRUCTOR(StructuralConstraintNoParentNodes);
  }

  /// copy constructor
  StructuralConstraintNoParentNodes::StructuralConstraintNoParentNodes(
      const StructuralConstraintNoParentNodes& from) : _noParentNodes_(from._noParentNodes_) {
    GUM_CONS_CPY(StructuralConstraintNoParentNodes);
  }

  /// move constructor
  StructuralConstraintNoParentNodes::StructuralConstraintNoParentNodes(
      StructuralConstraintNoParentNodes&& from) : _noParentNodes_(std::move(from._noParentNodes_)) {
    GUM_CONS_MOV(StructuralConstraintNoParentNodes);
  }

  /// destructor
  StructuralConstraintNoParentNodes::~StructuralConstraintNoParentNodes() {
    GUM_DESTRUCTOR(StructuralConstraintNoParentNodes);
  }

  /// copy operator
  StructuralConstraintNoParentNodes&
      StructuralConstraintNoParentNodes::operator=(const StructuralConstraintNoParentNodes& from) {
    if (this != &from) { _noParentNodes_ = from._noParentNodes_; }
    return *this;
  }

  /// move operator
  StructuralConstraintNoParentNodes&
      StructuralConstraintNoParentNodes::operator=(StructuralConstraintNoParentNodes&& from) {
    if (this != &from) { _noParentNodes_ = std::move(from._noParentNodes_); }
    return *this;
  }

}   // namespace gum::learning
