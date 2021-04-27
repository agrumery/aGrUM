/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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


/** @file
 * @brief Source implementation of the class building the Markov Blanket from a
 * DAGmodel and a node (id or name)
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *
 */
#include <agrum/BN/algorithms/MarkovBlanket.h>

#ifdef GUM_NO_INLINE
#  include <agrum/BN/algorithms/MarkovBlanket_inl.h>
#endif   // GUM_NOINLINE

namespace gum {
  MarkovBlanket::MarkovBlanket(const DAGmodel& m, NodeId id, int level) :
      model__(m), node__(id) {
    if (level < 1)
      GUM_ERROR(InvalidArgument, "Argument level(=" << level << ") must be >0.")

    NodeSet done;
    buildMarkovBlanket__(node__);
    done.insert(node__);

    while (level > 1) {
      level--;
      auto todo        = mb__.nodes().asNodeSet() - done;
      bool anythingnew = false;
      for (NodeId n: todo) {
        done.insert(n);
        if (buildMarkovBlanket__(n)) anythingnew = true;
      }
      if (!anythingnew) break;
    }

    // we add now some arcs that are between the nodes in mb__ but are not part of
    // the last ones.
    // For instance, an arc between a parent and a parent of children
    for (const auto node: mb__.nodes()) {
      for (const auto child: model__.children(node)) {
        if (mb__.existsNode(child) && !mb__.existsArc(Arc(node, child))) {
          mb__.addArc(node, child);
          specialArcs__.insert(Arc(node, child));
        }
      }
    }
  }

  MarkovBlanket::MarkovBlanket(const DAGmodel&    m,
                               const std::string& name,
                               int                level) :
      MarkovBlanket(m, m.idFromName(name), level) {}

  MarkovBlanket::~MarkovBlanket() {}

  bool MarkovBlanket::buildMarkovBlanket__(const NodeId node) {
    bool change = false;
    if (!model__.nodes().exists(node))
      GUM_ERROR(InvalidArgument, "Node " << node << " does not exist.");

    if (!mb__.nodes().exists(node)) {
      mb__.addNodeWithId(node);
      change = true;
    }

    for (const auto& parent: model__.parents(node)) {
      if (!mb__.nodes().exists(parent)) {
        mb__.addNodeWithId(parent);
        change = true;
      }
      mb__.addArc(parent, node);
    }

    for (const auto& child: model__.children(node)) {
      if (!mb__.nodes().exists(child)) {
        mb__.addNodeWithId(child);
        change = true;
      }
      mb__.addArc(node, child);
      for (const auto& opar: model__.parents(child)) {
        if (opar != node) {
          if (!mb__.nodes().exists(opar)) {
            mb__.addNodeWithId(opar);
            change = true;
          }
          mb__.addArc(opar, child);
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
        other.idFromName(model__.variable(nid).name());
      } catch (NotFound) { return false; }
    }

    for (const auto& arc: arcs()) {
      if (!other.arcs().exists(
             Arc(other.idFromName(model__.variable(arc.tail()).name()),
                 other.idFromName(model__.variable(arc.head()).name()))))
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

    for (const auto node: mb__.nodes()) {
      nodeStream << tab << node << "[label=\"" << model__.variable(node).name()
                 << "\"";
      if (node == node__) { nodeStream << ", color=red"; }
      nodeStream << "];" << std::endl;

      for (const auto chi: mb__.children(node)) {
        arcStream << tab << node << " -> " << chi;
        if (specialArcs__.exists(Arc(node, chi))) { arcStream << " [color=grey]"; }
        arcStream << ";" << std::endl;
      }
    }

    output << nodeStream.str() << std::endl
           << arcStream.str() << std::endl
           << "}" << std::endl;

    return output.str();
  }
}   // namespace gum
