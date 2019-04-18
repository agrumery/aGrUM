
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
#include <agrum/graphs/undiGraph.h>

namespace gum {

  /**
   * @class EssentialGraph
   * @headerfile essentialGraph.h <agrum/BN/algorithms/essentialGraph.h>
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
    EssentialGraph() = default;
    // this constructor will build the essential graph
    explicit EssentialGraph(const DAGmodel& m);
    // this constructor will use mh as essential graph for m
    EssentialGraph(const DAGmodel& m, const MixedGraph& mg);
    EssentialGraph(const EssentialGraph& g);
    EssentialGraph& operator=(const EssentialGraph& g);

    ~EssentialGraph();

    /// @return a copy of the mixed graph
    MixedGraph mixedGraph();

    /// @return a dot representation of this essentialGraph
    std::string toDot() const;

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

    UndiGraph skeleton() const;

    /// wrapping @ref MixedGraph::nodes()
    const NodeGraphPart& nodes() const;

    private:
    void __buildEssentialGraph();
    bool __strongly_protected(NodeId a, NodeId b);

    const DAGmodel* __dagmodel;
    MixedGraph      __mg;
  };
}   // namespace gum

#ifndef GUM_NO_INLINE
#  include <agrum/BN/algorithms/essentialGraph_inl.h>
#endif   // GUM_NOINLINE

#endif   // GUM_ESSENTIALGRAPH_H
