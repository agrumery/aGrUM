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
 * @brief Class building the markovBlanket from a DAGmodel and a node name
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */
#ifndef GUM_MARKOVBLANKET_H
#define GUM_MARKOVBLANKET_H

#include <agrum/base/graphicalModels/DAGmodel.h>

namespace gum {

  /**
* @class MarkovBlanket
* @headerfile MarkovBlanket.h <agrum/BN/algorithms/MarkovBlanket.h>
   * @brief Class building the markov Blanket from a BN and a nodeName.
   * @ingroup bn_group

   * The main goal of this class is to build and to encapsulate the DiGraph which
   represents the Markov Blanket.
   *
   */
  class MarkovBlanket {
    public:
    MarkovBlanket(const DAGmodel& m, NodeId n, int level = 1);
    MarkovBlanket(const DAGmodel& m, const std::string& name, int level = 1);

    ~MarkovBlanket();

    /// @return a copy of the graph
    DAG dag() const;

    // @return a dot representation of this MarkovBlanket
    // node of interest is in red
    // special arcs (not used during the construction of the Markov Blanket) are in
    // grey
    std::string toDot() const;

    /// wrapping @ref DAG::parents(id)
    const NodeSet& parents(NodeId id) const;

    /// wrapping @ref DAG::children(id)
    const NodeSet& children(NodeId id) const;

    /// wrapping @ref DAG::parents(ids)
    NodeSet parents(const NodeSet& ids) const;

    /// wrapping @ref DAG::children(ids)
    NodeSet children(const NodeSet& ids) const;

    /// wrapping @ref DAG::sizeArcs()
    Size sizeArcs() const;

    /// wrapping @ref DAG::arcs()
    const ArcSet& arcs() const;

    /// wrapping @ref DAG::sizeNodes()
    Size sizeNodes() const;

    /// wrapping @ref DAG::size()
    Size size() const;

    /// wrapping @ref DAG::nodes()
    const NodeGraphPart& nodes() const;

    /// @return true if all the named node are the same and all the named arcs are
    /// the same
    bool hasSameStructure(const DAGmodel& other) const;

    private:
    bool _buildMarkovBlanket_(const NodeId id);

    const DAGmodel& _model_;
    DAG             _mb_;
    const NodeId    _node_;
    ArcSet          _specialArcs_;
  };
}   // namespace gum

#ifndef GUM_NO_INLINE
#  include <agrum/BN/algorithms/MarkovBlanket_inl.h>
#endif   // GU%_NO_INLINE

#endif   // GUM_MARKOVBLANKET_H
