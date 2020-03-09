
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
 * @brief Source implementation of Base classes for oriented graphs
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */
#include <agrum/tools/graphs/diGraph.h>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/graphs/diGraph_inl.h>
#endif   // GUM_NOINLINE

namespace gum {

  DiGraph::DiGraph(Size nodes_size,
                   bool nodes_resize_policy,
                   Size arcs_size,
                   bool arcs_resize_policy) :
      NodeGraphPart(nodes_size, nodes_resize_policy),
      ArcGraphPart(arcs_size, arcs_resize_policy),
      __mutableTopologicalOrder(nullptr) {
    GUM_CONSTRUCTOR(DiGraph);
  }

  DiGraph::DiGraph(const DiGraph& g) :
      NodeGraphPart(g), ArcGraphPart(g), __mutableTopologicalOrder(nullptr) {
    GUM_CONS_CPY(DiGraph);
    if (g.__mutableTopologicalOrder != nullptr) {
      __mutableTopologicalOrder =
         new Sequence< NodeId >(*(g.__mutableTopologicalOrder));
    }
  }

  DiGraph::~DiGraph() {
    GUM_DESTRUCTOR(DiGraph);
    if (__mutableTopologicalOrder != nullptr) { delete __mutableTopologicalOrder; }
  }

  const std::string DiGraph::toString() const {
    std::string s = NodeGraphPart::toString();
    s += " , ";
    s += ArcGraphPart::toString();
    return s;
  }

  const std::string DiGraph::toDot() const {
    std::stringstream strBuff;
    std::string       tab = "     ";
    strBuff << "digraph {" << std::endl;

    for (const auto node: nodes())
      strBuff << tab << node << ";" << std::endl;

    strBuff << std::endl;

    for (const auto& arc: arcs())
      strBuff << tab << arc.tail() << " -> " << arc.head() << ";" << std::endl;

    strBuff << "}" << std::endl << std::endl;
    return strBuff.str();
  }

  /// for friendly displaying the content of directed graphs
  std::ostream& operator<<(std::ostream& stream, const DiGraph& g) {
    stream << g.toString();
    return stream;
  }

  const Sequence< NodeId >& DiGraph::topologicalOrder(bool clear) const {
    if (clear
        || (__mutableTopologicalOrder
            == nullptr)) {   // we have to call _topologicalOrder
      if (__mutableTopologicalOrder == nullptr) {
        __mutableTopologicalOrder = new Sequence< NodeId >();
      } else {
        // clear is True
        __mutableTopologicalOrder->clear();
      }

      __topologicalOrder();
    }

    return *__mutableTopologicalOrder;
  }

  void DiGraph::__topologicalOrder() const {
    auto dag = *this;
    auto roots = std::vector< NodeId >();

    for (const auto node: dag.nodes()) {
      if (dag.parents(node).empty()) { roots.push_back(node); }
    }

    while (roots.size()) {
      if (__mutableTopologicalOrder->exists(roots.back())) {
        GUM_ERROR(InvalidDirectedCycle,
                  "cycles prevent the creation of a topological ordering.");
      }
      __mutableTopologicalOrder->insert(roots.back());
      roots.pop_back();

      while (dag.children(__mutableTopologicalOrder->back()).size()) {
        auto back = __mutableTopologicalOrder->back();
        auto child = *(dag.children(back).begin());
        dag.eraseArc(Arc(back, child));

        if (dag.parents(child).empty()) { roots.push_back(child); }
      }
    }

    GUM_ASSERT(dag.sizeArcs() == (gum::Size)(0));
    GUM_ASSERT(__mutableTopologicalOrder->size() == dag.size());
  }

  bool DiGraph::hasDirectedPath(const NodeId from, const NodeId to) {
    if (!exists(from)) return false;

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

      for (const auto chi: children(new_one)) {
        if (chi == to) return true;

        if (!marked.contains(chi)) {
          nodeFIFO.pushBack(chi);
          marked.insert(chi);
        }
      }
    }

    return false;
  }

} /* namespace gum */
