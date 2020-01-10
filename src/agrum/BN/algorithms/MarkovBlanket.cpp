
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
 * @brief Source implementation of the class building the Markov Blanket from a
 * DAGmodel and a node (id or name)
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */
#include <agrum/BN/algorithms/MarkovBlanket.h>

#ifdef GUM_NO_INLINE
#  include <agrum/BN/algorithms/MarkovBlanket_inl.h>
#endif   // GUM_NOINLINE

namespace gum {
  MarkovBlanket::MarkovBlanket(const DAGmodel& m, NodeId id, int level) :
      __model(m), __node(id) {
    if (level < 1)
      GUM_ERROR(InvalidArgument, "Argument level(=" << level << ") must be >0.")

    NodeSet done;
    __buildMarkovBlanket(__node);
    done.insert(__node);

    while (level > 1) {
      level--;
      auto todo = __mb.nodes().asNodeSet() - done;
      bool anythingnew = false;
      for (NodeId n: todo) {
        done.insert(n);
        if (__buildMarkovBlanket(n)) anythingnew = true;
      }
      if (!anythingnew) break;
    }

    // we add now some arcs that are between the nodes in __mb but are not part of
    // the last ones.
    // For instance, an arc between a parent and a parent of children
    for (const auto node: __mb.nodes()) {
      for (const auto child: __model.children(node)) {
        if (__mb.existsNode(child) && !__mb.existsArc(Arc(node, child))) {
          __mb.addArc(node, child);
          __specialArcs.insert(Arc(node, child));
        }
      }
    }
  }

  MarkovBlanket::MarkovBlanket(const DAGmodel&    m,
                               const std::string& name,
                               int                level) :
      MarkovBlanket(m, m.idFromName(name), level) {}

  MarkovBlanket::~MarkovBlanket() {}

  bool MarkovBlanket::__buildMarkovBlanket(const NodeId node) {
    bool change = false;
    if (!__model.nodes().exists(node))
      GUM_ERROR(InvalidArgument, "Node " << node << " does not exist.");

    if (!__mb.nodes().exists(node)) {
      __mb.addNodeWithId(node);
      change = true;
    }

    for (const auto& parent: __model.parents(node)) {
      if (!__mb.nodes().exists(parent)) {
        __mb.addNodeWithId(parent);
        change = true;
      }
      __mb.addArc(parent, node);
    }

    for (const auto& child: __model.children(node)) {
      if (!__mb.nodes().exists(child)) {
        __mb.addNodeWithId(child);
        change = true;
      }
      __mb.addArc(node, child);
      for (const auto& opar: __model.parents(child)) {
        if (opar != node) {
          if (!__mb.nodes().exists(opar)) {
            __mb.addNodeWithId(opar);
            change = true;
          }
          __mb.addArc(opar, child);
        }
      }
    }

    return change;
  }

  bool MarkovBlanket::hasSameStructure(const DAGmodel& other) {
    if (size() != other.size()) return false;

    if (sizeArcs() != other.sizeArcs()) return false;

    for (const auto& nid: nodes()) {
      try {
        other.idFromName(__model.variable(nid).name());
      } catch (NotFound) { return false; }
    }

    for (const auto& arc: arcs()) {
      if (!other.arcs().exists(
             Arc(other.idFromName(__model.variable(arc.tail()).name()),
                 other.idFromName(__model.variable(arc.head()).name()))))
        return false;
    }

    return true;
  }

  std::string MarkovBlanket::toDot() const {
    std::stringstream output;
    std::stringstream nodeStream;
    std::stringstream arcStream;
    List< NodeId >    treatedNodes;
    output << "digraph \""
           << "no_name\" {" << std::endl;
    nodeStream << "node [shape = ellipse];" << std::endl;
    std::string tab = "  ";

    for (const auto node: __mb.nodes()) {
      nodeStream << tab << node << "[label=\"" << __model.variable(node).name()
                 << "\"";
      if (node == __node) { nodeStream << ", color=red"; }
      nodeStream << "];" << std::endl;

      for (const auto chi: __mb.children(node)) {
        arcStream << tab << node << " -> " << chi;
        if (__specialArcs.exists(Arc(node, chi))) { arcStream << " [color=grey]"; }
        arcStream << ";" << std::endl;
      }
    }

    output << nodeStream.str() << std::endl
           << arcStream.str() << std::endl
           << "}" << std::endl;

    return output.str();
  }
}   // namespace gum
