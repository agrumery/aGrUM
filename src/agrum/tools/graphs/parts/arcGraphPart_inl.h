/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */

// to ease parsing by IDE
#include <agrum/tools/graphs/parts/arcGraphPart.h>

namespace gum {

  INLINE bool ArcGraphPart::emptyArcs() const { return _arcs_.empty(); }

  INLINE Size ArcGraphPart::sizeArcs() const { return _arcs_.size(); }

  INLINE const ArcSet& ArcGraphPart::arcs() const { return _arcs_; }

  INLINE bool ArcGraphPart::existsArc(const Arc& arc) const { return _arcs_.contains(arc); }

  INLINE bool ArcGraphPart::existsArc(NodeId tail, NodeId head) const {
    return _parents_.exists(head) && _parents_[head]->exists(tail);
  }

  INLINE void ArcGraphPart::_checkParents_(NodeId id) {
    if (!_parents_.exists(id)) { _parents_.insert(id, new NodeSet); }
  }

  INLINE void ArcGraphPart::_checkChildren_(NodeId id) {
    if (!_children_.exists(id)) { _children_.insert(id, new NodeSet); }
  }

  INLINE const NodeSet& ArcGraphPart::parents(NodeId id) const {
    if (_parents_.exists(id)) return *(_parents_[id]);
    else return emptyNodeSet;
  }

  INLINE NodeSet ArcGraphPart::family(NodeId id) const {
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
    if (_children_.exists(id)) return *_children_[id];
    else return emptyNodeSet;
  }

  INLINE void ArcGraphPart::addArc(NodeId tail, NodeId head) {
    Arc arc(tail, head);

    _arcs_.insert(arc);
    _checkParents_(head);
    _checkChildren_(tail);
    _parents_[head]->insert(tail);
    _children_[tail]->insert(head);

    GUM_EMIT2(onArcAdded, tail, head);
  }

  INLINE void ArcGraphPart::eraseArc(const Arc& arc) {
    // ASSUMING tail and head exists in  _parents_ anf  _children_
    // (if not, it is an error)
    if (existsArc(arc)) {
      NodeId tail = arc.tail();
      NodeId head = arc.head();
      _parents_[head]->erase(tail);
      _children_[tail]->erase(head);
      _arcs_.erase(arc);
      GUM_EMIT2(onArcDeleted, tail, head);
    }
  }

  INLINE void ArcGraphPart::eraseSetOfArcs_(const ArcSet& set) {
    for (const auto& arc: set)
      eraseArc(arc);
  }

  INLINE void ArcGraphPart::eraseParents(NodeId id) {
    if (_parents_.exists(id)) {
      const NodeSet& parents = *(_parents_[id]);

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
    if (_children_.exists(id)) {
      const NodeSet& children = *(_children_[id]);

      for (auto iter = children.beginSafe();   // safe iterator needed here
           iter != children.endSafe();
           ++iter) {
        // warning: use this erase so that you actually use the virtualized
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
    if (_parents_.exists(id)) {
      const NodeSet& parents = *(_parents_[id]);

      for (auto iter = parents.beginSafe();   // safe iterator needed here
           iter != parents.endSafe();
           ++iter) {
        ArcGraphPart::eraseArc(Arc(*iter, id));
      }
    }
  }

  INLINE void ArcGraphPart::unvirtualizedEraseChildren(NodeId id) {
    if (_children_.exists(id)) {
      const NodeSet& children = *(_children_[id]);

      for (auto iter = children.beginSafe();   // safe iterator needed here
           iter != children.endSafe();
           ++iter) {
        ArcGraphPart::eraseArc(Arc(id, *iter));
      }
    }
  }

  INLINE bool ArcGraphPart::operator==(const ArcGraphPart& p) const { return _arcs_ == p._arcs_; }

} /* namespace gum */
