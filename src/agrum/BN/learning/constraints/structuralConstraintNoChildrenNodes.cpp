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

#include <agrum/BN/learning/constraints/structuralConstraintNoChildrenNodes.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#  include <agrum/BN/learning/constraints/structuralConstraintNoChildrenNodes_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum::learning {

  /// default constructor
  StructuralConstraintNoChildrenNodes::StructuralConstraintNoChildrenNodes() {
    GUM_CONSTRUCTOR(StructuralConstraintNoChildrenNodes);
  }

  /// constructor starting with a given graph
  StructuralConstraintNoChildrenNodes::StructuralConstraintNoChildrenNodes(const DiGraph& graph) {
    setGraph(graph);
    GUM_CONSTRUCTOR(StructuralConstraintNoChildrenNodes);
  }

  /// copy constructor
  StructuralConstraintNoChildrenNodes::StructuralConstraintNoChildrenNodes(
      const StructuralConstraintNoChildrenNodes& from) : _noChildrenNodes_(from._noChildrenNodes_) {
    GUM_CONS_CPY(StructuralConstraintNoChildrenNodes);
  }

  /// move constructor
  StructuralConstraintNoChildrenNodes::StructuralConstraintNoChildrenNodes(
      StructuralConstraintNoChildrenNodes&& from) :
      _noChildrenNodes_(std::move(from._noChildrenNodes_)) {
    GUM_CONS_MOV(StructuralConstraintNoChildrenNodes);
  }

  /// destructor
  StructuralConstraintNoChildrenNodes::~StructuralConstraintNoChildrenNodes() {
    GUM_DESTRUCTOR(StructuralConstraintNoChildrenNodes);
  }

  /// copy operator
  StructuralConstraintNoChildrenNodes& StructuralConstraintNoChildrenNodes::operator=(
      const StructuralConstraintNoChildrenNodes& from) {
    if (this != &from) { _noChildrenNodes_ = from._noChildrenNodes_; }
    return *this;
  }

  /// move operator
  StructuralConstraintNoChildrenNodes&
      StructuralConstraintNoChildrenNodes::operator=(StructuralConstraintNoChildrenNodes&& from) {
    if (this != &from) { _noChildrenNodes_ = std::move(from._noChildrenNodes_); }
    return *this;
  }

}   // namespace gum::learning
