
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


/** @file
 * @brief Source implementation of Base classes for directed acyclic graphs
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */
#include <agrum/graphs/DAG.h>

#ifdef GUM_NO_INLINE
#  include <agrum/graphs/DAG_inl.h>
#endif   // GUM_NOINLINE

namespace gum {

  // diamond structure require to explicitly initialize NodeGraphPart
  DAG::DAG(Size nodes_size,
           bool nodes_resize_policy,
           Size arcs_size,
           bool arcs_resize_policy) :
      NodeGraphPart(nodes_size, nodes_resize_policy),
      DiGraph(nodes_size, nodes_resize_policy, arcs_size, arcs_resize_policy) {
    GUM_CONSTRUCTOR(DAG);
  }

  // diamond structure require to explicitly initialize NodeGraphPart
  DAG::DAG(const DAG& g) : NodeGraphPart(g), DiGraph(g) { GUM_CONS_CPY(DAG); }

  DAG::~DAG() { GUM_DESTRUCTOR(DAG); }

  /// we prefer to re-implement this (instead of using
  /// ArcGraphPart::directedPath)
  /// for optimisation
  /// @warning : this version is optimized for the search of a directedPath
  /// in a DAG !
  bool DAG::__hasDirectedPath(const NodeId from, const NodeId to) {
    if (!exists(from)) return false;

    if (from == to) return true;

    // not recursive version => use a FIFO for simulating the recursion
    List< NodeId > nodeFIFO;
    nodeFIFO.pushBack(from);

    NodeSet marked;
    marked.insert(from);

    NodeId new_one;

    while (!nodeFIFO.empty()) {
      new_one = nodeFIFO.front();
      // std::cout<<new_one<<std::endl;
      nodeFIFO.popFront();

      for (const auto chi : children(new_one)) {
        if (chi == to) return true;

        if (!marked.contains(chi)) {
          nodeFIFO.pushBack(chi);
          marked.insert(chi);
        }
      }
    }

    return false;
  }

  UndiGraph DAG::moralGraph() const {
    UndiGraph moralgraph;
    moralgraph.populateNodes(*this);
    // transform the arcs into edges
    for (const auto arc : arcs())
      moralgraph.addEdge(arc.first(), arc.second());

    // marry the parents
    for (const auto node : nodes()) {
      const auto& par = parents(node);

      for (auto it1 = par.begin(); it1 != par.end(); ++it1) {
        auto it2 = it1;

        for (++it2; it2 != par.end(); ++it2) {
          // will automatically check if this edge already exists
          moralgraph.addEdge(*it1, *it2);
        }
      }
    }
    return moralgraph;
  }

} /* namespace gum */
