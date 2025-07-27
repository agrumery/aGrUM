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


/** @file
 * @brief the base class for structural constraints used by learning algorithms
 * that learn an undirected graph structure
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/BN/learning/constraints/structuralConstraintUndiGraph.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#  include <agrum/BN/learning/constraints/structuralConstraintUndiGraph_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    StructuralConstraintUndiGraph::StructuralConstraintUndiGraph() {
      GUM_CONSTRUCTOR(StructuralConstraintUndiGraph);
    }

    /// constructor starting with an empty graph with a given number of nodes
    StructuralConstraintUndiGraph::StructuralConstraintUndiGraph(Size nb_nodes) {
      setGraph(nb_nodes);
      GUM_CONSTRUCTOR(StructuralConstraintUndiGraph);
    }

    /// constructor starting with a given graph
    StructuralConstraintUndiGraph::StructuralConstraintUndiGraph(const UndiGraph& graph) {
      setGraph(graph);
      GUM_CONSTRUCTOR(StructuralConstraintUndiGraph);
    }

    /// copy constructor
    StructuralConstraintUndiGraph::StructuralConstraintUndiGraph(
        const StructuralConstraintUndiGraph& from) : _UndiGraph_graph_(from._UndiGraph_graph_) {
      GUM_CONS_CPY(StructuralConstraintUndiGraph);
    }

    /// move constructor
    StructuralConstraintUndiGraph::StructuralConstraintUndiGraph(
        StructuralConstraintUndiGraph&& from) :
        _UndiGraph_graph_(std::move(from._UndiGraph_graph_)) {
      GUM_CONS_MOV(StructuralConstraintUndiGraph);
    }

    /// destructor
    StructuralConstraintUndiGraph::~StructuralConstraintUndiGraph() {
      GUM_DESTRUCTOR(StructuralConstraintUndiGraph);
    }

    /// copy operator
    StructuralConstraintUndiGraph&
        StructuralConstraintUndiGraph::operator=(const StructuralConstraintUndiGraph& from) {
      if (this != &from) { _UndiGraph_graph_ = from._UndiGraph_graph_; }
      return *this;
    }

    /// move operator
    StructuralConstraintUndiGraph&
        StructuralConstraintUndiGraph::operator=(StructuralConstraintUndiGraph&& from) {
      if (this != &from) { _UndiGraph_graph_ = std::move(from._UndiGraph_graph_); }
      return *this;
    }

  } /* namespace learning */

} /* namespace gum */
