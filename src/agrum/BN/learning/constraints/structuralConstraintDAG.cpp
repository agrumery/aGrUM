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
 * @brief the base class for structural constraints imposed by DAGs
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/BN/learning/constraints/structuralConstraintDAG.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#  include <agrum/BN/learning/constraints/structuralConstraintDAG_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    StructuralConstraintDAG::StructuralConstraintDAG() { GUM_CONSTRUCTOR(StructuralConstraintDAG); }

    /// constructor starting with an empty graph with a given number of nodes
    StructuralConstraintDAG::StructuralConstraintDAG(Size nb_nodes) :
        StructuralConstraintDiGraph(nb_nodes) {
      StructuralConstraintDiGraph::setGraph(nb_nodes);

      DAG g;
      for (NodeId i = 0; i < nb_nodes; ++i) {
        g.addNodeWithId(i);
      }
      _DAG_cycle_detector_.setDAG(g);

      GUM_CONSTRUCTOR(StructuralConstraintDAG);
    }

    /// constructor starting with a given graph
    StructuralConstraintDAG::StructuralConstraintDAG(const DAG& graph) {
      StructuralConstraintDiGraph::setGraph(graph);
      _DAG_cycle_detector_.setDAG(graph);

      GUM_CONSTRUCTOR(StructuralConstraintDAG);
    }

    /// copy constructor
    StructuralConstraintDAG::StructuralConstraintDAG(const StructuralConstraintDAG& from) :
        StructuralConstraintDiGraph(from), _DAG_cycle_detector_(from._DAG_cycle_detector_) {
      GUM_CONS_CPY(StructuralConstraintDAG);
    }

    /// move constructor
    StructuralConstraintDAG::StructuralConstraintDAG(StructuralConstraintDAG&& from) :
        StructuralConstraintDiGraph(std::move(from)),
        _DAG_cycle_detector_(std::move(from._DAG_cycle_detector_)) {
      GUM_CONS_MOV(StructuralConstraintDAG);
    }

    /// destructor
    StructuralConstraintDAG::~StructuralConstraintDAG() { GUM_DESTRUCTOR(StructuralConstraintDAG); }

    /// copy operator
    StructuralConstraintDAG&
        StructuralConstraintDAG::operator=(const StructuralConstraintDAG& from) {
      if (this != &from) {
        StructuralConstraintDiGraph::operator=(from);
        _DAG_cycle_detector_ = from._DAG_cycle_detector_;
      }
      return *this;
    }

    /// move operator
    StructuralConstraintDAG& StructuralConstraintDAG::operator=(StructuralConstraintDAG&& from) {
      if (this != &from) {
        StructuralConstraintDiGraph::operator=(std::move(from));
        _DAG_cycle_detector_ = std::move(from._DAG_cycle_detector_);
      }
      return *this;
    }

  } /* namespace learning */

} /* namespace gum */
