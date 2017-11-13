/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/**
 * @file
 * @brief Class building the essential Graph from a DAGmodel
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */
#ifndef GUM_ESSENTIALGRAPH_H
#define GUM_ESSENTIALGRAPH_H

#include <agrum/graphicalModels/DAGmodel.h>
#include <agrum/graphs/graphElements.h>
#include <agrum/graphs/mixedGraph.h>

namespace gum {

  /**
   * @class EssentialGraph essentialGraph.h <agrum/BN/algorithms/essentialGraph.h>
   * @brief Class building the essential graph from a BN.
   * @ingroup bn_group
   *
   * Essential graph is a mixed graph (Chain Graph) that represents the class of
   * markov equivalent Bayesian Networks (with the same independency model).
   *
   * The main goal of this class is to nest the algorithm to build the essential
   * graph
   * from a BN and to encapsulate the representation (as a MixedGraph) of the
   * essential
   * graph.
   *
   * gum::operator<<(std::ostream&, const BayesNet<GUM_SCALAR>&).
   */
  class EssentialGraph {
    public:
    EssentialGraph(const DAGmodel& m);
    ~EssentialGraph();

    /// @return a copy of the mixed graph
    MixedGraph mixedGraph();

    /// @return a dot representation of this essentialGraph
    std::string toDot(void) const;

    /// wrapping @ref MixedGraph::parents(id)
    const NodeSet& parents(const NodeId id) const;

    /// wrapping @ref MixedGraph::parents(id)
    const NodeSet& children(const NodeId id) const;

    /// wrapping @ref MixedGraph::parents(id)
    const NodeSet& neighbours(const NodeId id) const;

    /// wrapping @ref MixedGraph::sizeArcs()
    Size sizeArcs() const;

    /// wrapping @ref MixedGraph::arcs()
    const ArcSet& arcs() const;
    /// wrapping @ref MixedGraph::sizeEdges()
    Size sizeEdges() const;

    /// wrapping @ref MixedGraph::edges()
    const EdgeSet& edges() const;

    /// wrapping @ref MixedGraph::sizeNodes()
    Size sizeNodes() const;
    /// wrapping @ref MixedGraph::size()
    Size size() const;

    /// wrapping @ref MixedGraph::nodes()
    const NodeGraphPart& nodes() const;

    private:
    void __buildEssentialGraph();
    bool __strongly_protected(NodeId a, NodeId b);

    const DAGmodel& __dag;
    MixedGraph      __mg;
  };
}  // namespace gum

#ifndef GUM_NO_INLINE
#include <agrum/BN/algorithms/essentialGraph_inl.h>
#endif  // GUM_NOINLINE

#endif  // GUM_ESSENTIALGRAPH_H
