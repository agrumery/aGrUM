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
 * @brief the structural constraint indicating that some arcs shall never be
 * removed or reversed
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/BN/learning/constraints/structuralConstraintMandatoryArcs.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#  include <agrum/BN/learning/constraints/structuralConstraintMandatoryArcs_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    StructuralConstraintMandatoryArcs::StructuralConstraintMandatoryArcs() {
      GUM_CONSTRUCTOR(StructuralConstraintMandatoryArcs);
    }

    /// constructor starting with a given graph
    StructuralConstraintMandatoryArcs::StructuralConstraintMandatoryArcs(const DiGraph& graph) {
      setGraph(graph);
      GUM_CONSTRUCTOR(StructuralConstraintMandatoryArcs);
    }

    /// copy constructor
    StructuralConstraintMandatoryArcs::StructuralConstraintMandatoryArcs(
        const StructuralConstraintMandatoryArcs& from) :
        _MandatoryArcs_mandatory_arcs_(from._MandatoryArcs_mandatory_arcs_) {
      GUM_CONS_CPY(StructuralConstraintMandatoryArcs);
    }

    /// move constructor
    StructuralConstraintMandatoryArcs::StructuralConstraintMandatoryArcs(
        StructuralConstraintMandatoryArcs&& from) :
        _MandatoryArcs_mandatory_arcs_(std::move(from._MandatoryArcs_mandatory_arcs_)) {
      GUM_CONS_MOV(StructuralConstraintMandatoryArcs);
    }

    /// destructor
    StructuralConstraintMandatoryArcs::~StructuralConstraintMandatoryArcs() {
      GUM_DESTRUCTOR(StructuralConstraintMandatoryArcs);
    }

    /// copy operator
    StructuralConstraintMandatoryArcs& StructuralConstraintMandatoryArcs::operator=(
        const StructuralConstraintMandatoryArcs& from) {
      if (this != &from) { _MandatoryArcs_mandatory_arcs_ = from._MandatoryArcs_mandatory_arcs_; }
      return *this;
    }

    /// move operator
    StructuralConstraintMandatoryArcs&
        StructuralConstraintMandatoryArcs::operator=(StructuralConstraintMandatoryArcs&& from) {
      if (this != &from) {
        _MandatoryArcs_mandatory_arcs_ = std::move(from._MandatoryArcs_mandatory_arcs_);
      }
      return *this;
    }

  } /* namespace learning */

} /* namespace gum */
