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
 * @brief Inline implementation of classes for directed edge sets
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *
 */

// to ease parsing by IDE
#include <agrum/tools/graphs/parts/arcGraphPart.h>

namespace gum {

  INLINE bool ArcGraphPart::emptyArcs() const { return arcs__.empty(); }

  INLINE Size ArcGraphPart::sizeArcs() const { return arcs__.size(); }

  INLINE const ArcSet& ArcGraphPart::arcs() const { return arcs__; }

  INLINE bool ArcGraphPart::existsArc(const Arc& arc) const {
    return arcs__.contains(arc);
  }

  INLINE bool ArcGraphPart::existsArc(NodeId tail, NodeId head) const {
    return parents__.exists(head) && parents__[head]->exists(tail);
  }

  INLINE void ArcGraphPart::checkParents__(NodeId id) const {
    if (!parents__.exists(id)) { parents__.insert(id, new NodeSet); }
  }

  INLINE void ArcGraphPart::checkChildren__(NodeId id) const {
    if (!children__.exists(id)) { children__.insert(id, new NodeSet); }
  }

  INLINE const NodeSet& ArcGraphPart::parents(NodeId id) const {
    checkParents__(id);
    return *(parents__[id]);
  }

  INLINE NodeSet ArcGraphPart::family(NodeId id) const {
    checkParents__(id);
    NodeSet res{id};
    return res + parents(id);
  }

  /// returns the set of children of a set of nodes
  INLINE NodeSet ArcGraphPart::children(const NodeSet& ids) const {
    NodeSet res;
    for (const auto node: ids)
      res += children(node);
    return res;
  }

  /// returns the set of parents of a set of nodes
  INLINE NodeSet ArcGraphPart::parents(const NodeSet& ids) const {
    NodeSet res;
    for (const auto node: ids)
      res += parents(node);
    return res;
  }

  /// returns the set of family nodes of a set of nodes
  INLINE NodeSet ArcGraphPart::family(const NodeSet& ids) const {
    NodeSet res;
    for (const auto node: ids)
      res += family(node);
    return res;
  }

  INLINE const NodeSet& ArcGraphPart::children(NodeId id) const {
    checkChildren__(id);
    return *(children__[id]);
  }

  INLINE void ArcGraphPart::addArc(NodeId tail, NodeId head) {
    Arc arc(tail, head);

    arcs__.insert(arc);
    checkParents__(head);
    checkChildren__(tail);
    parents__[head]->insert(tail);
    children__[tail]->insert(head);

    GUM_EMIT2(onArcAdded, tail, head);
  }

  INLINE void ArcGraphPart::eraseArc(const Arc& arc) {
    // ASSUMING tail and head exists in parents__ anf children__
    // (if not, it is an error)
    if (existsArc(arc)) {
      NodeId tail = arc.tail(), head = arc.head();
      parents__[head]->erase(tail);
      children__[tail]->erase(head);
      arcs__.erase(arc);
      GUM_EMIT2(onArcDeleted, tail, head);
    }
  }

  INLINE void ArcGraphPart::eraseSetOfArcs_(const ArcSet& set) {
    for (const auto& arc: set)
      eraseArc(arc);
  }

  INLINE void ArcGraphPart::eraseParents(NodeId id) {
    if (parents__.exists(id)) {
      NodeSet& parents = *(parents__[id]);

      for (auto iter = parents.beginSafe();   // safe iterator needed here
           iter != parents.endSafe();
           ++iter) {
        // warning: use this erase so that you actually use the virtualized
        // arc removal function
        eraseArc(Arc(*iter, id));
      }
    }
  }

  INLINE void ArcGraphPart::eraseChildren(NodeId id) {
    if (children__.exists(id)) {
      NodeSet& children = *(children__[id]);

      for (auto iter = children.beginSafe();   // safe iterator needed here
           iter != children.endSafe();
           ++iter) {
        // warning: use this erase so that you actually use the vritualized
        // arc removal function
        eraseArc(Arc(id, *iter));
      }
    }
  }

  INLINE void ArcGraphPart::unvirtualizedEraseSetOfArcs_(const ArcSet& set) {
    for (const auto& arc: set)
      ArcGraphPart::eraseArc(arc);
  }

  INLINE void ArcGraphPart::unvirtualizedEraseParents(NodeId id) {
    if (parents__.exists(id)) {
      NodeSet& parents = *(parents__[id]);

      for (auto iter = parents.beginSafe();   // safe iterator needed here
           iter != parents.endSafe();
           ++iter) {
        ArcGraphPart::eraseArc(Arc(*iter, id));
      }
    }
  }

  INLINE void ArcGraphPart::unvirtualizedEraseChildren(NodeId id) {
    if (children__.exists(id)) {
      NodeSet& children = *(children__[id]);

      for (auto iter = children.beginSafe();   // safe iterator needed here
           iter != children.endSafe();
           ++iter) {
        ArcGraphPart::eraseArc(Arc(id, *iter));
      }
    }
  }

  INLINE bool ArcGraphPart::operator==(const ArcGraphPart& p) const {
    return arcs__ == p.arcs__;
  }

  INLINE bool ArcGraphPart::operator!=(const ArcGraphPart& p) const {
    return arcs__ != p.arcs__;
  }

} /* namespace gum */
