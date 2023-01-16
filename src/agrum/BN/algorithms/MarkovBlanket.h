/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Class building the markovBlanket from a DAGmodel and a node name
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */
#ifndef GUM_MARKOVBLANKET_H
#define GUM_MARKOVBLANKET_H

#include <agrum/tools/graphicalModels/DAGmodel.h>

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
#endif   // GUM_NOINLINE

#endif   // GUM_MARKOVBLANKET_H
