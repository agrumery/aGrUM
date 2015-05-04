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
/** @file
 * @brief Source implementation of nodes sets
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */
#include <agrum/graphs/nodeGraphPart.h>

#ifdef GUM_NO_INLINE
#include <agrum/graphs/nodeGraphPart.inl>
#endif // GUM_NOINLINE

namespace gum {

  ///////////////////// NodeGraphPart
  NodeGraphPart::NodeGraphPart(Size holes_size, bool holes_resize_policy)
      : __holes_size(holes_size), __holes_resize_policy(holes_resize_policy),
        __endIteratorSafe(*this), __bound(0) {
    __holes = 0;
    GUM_CONSTRUCTOR(NodeGraphPart);
    __updateEndIteratorSafe();
  }

  NodeGraphPart::NodeGraphPart(const NodeGraphPart &s)
      : __holes_size(s.__holes_size), __holes_resize_policy(s.__holes_resize_policy),
        __endIteratorSafe(*this), __bound(s.__bound) {
    __holes = 0;

    if (s.__holes)
      __holes = new NodeSet(*s.__holes);

    __updateEndIteratorSafe();

    GUM_CONS_CPY(NodeGraphPart);
  }

  NodeGraphPart::~NodeGraphPart() {
    if (__holes)
      delete __holes;

    GUM_DESTRUCTOR(NodeGraphPart);
  }

  void NodeGraphPart::populateNodes(const NodeGraphPart &s) {
    clear(); // "virtual" flush of the nodes set
    __holes_size = s.__holes_size;
    __holes_resize_policy = s.__holes_resize_policy;

    if (s.__holes)
      __holes = new NodeSet(*s.__holes);

    __bound = s.__bound;

    __updateEndIteratorSafe();
  }

  // id is assumed to belong to NodeGraphPart
  void NodeGraphPart::__addHole(NodeId node) {

    // we assume that the node exists
    if (node + 1 == __bound) {
      // we remove the max : no new hole and maybe a bunch of holes to remove
      --__bound;

      if (__holes) {
        while (__holes->contains(__bound - 1)) {
          // a bunch of holes to remove. We do not use inHoles for optimisation :
          // not to repeat the test if (__holes) each time
          __holes->erase(--__bound);
        }

        if (__holes->empty()) {
          __holes->resize(__holes_size);
        }
      }

      __updateEndIteratorSafe();
    } else {
      if (!__holes)
        __holes = new NodeSet(__holes_size, __holes_resize_policy);

      __holes->insert(node);
    }
  }

  std::string NodeGraphPart::toString() const {
    std::stringstream s;
    bool first = true;
    s << "{";

    for (NodeId id = 0; id < __bound; ++id) {
      if (__inHoles(id))
        continue;

      if (first) {
        first = false;
      } else {
        s << ",";
      }

      s << id;
    }

    s << "}";

    return s.str();
  }

  std::ostream &operator<<(std::ostream &stream, const NodeGraphPart &set) {
    stream << set.toString();
    return stream;
  }

  void NodeGraphPart::insertNode(const NodeId id) { addNode(id); }
  void NodeGraphPart::addNode(const NodeId id) {
    if (id >= __bound) {
      if (id > __bound) { // we have to add holes
        if (!__holes)
          __holes = new NodeSet();

        for (NodeId i = __bound; i < id; ++i)
          __holes->insert(i);
      }

      __bound = id + 1;

      __updateEndIteratorSafe();
    } else {
      if (__inHoles(id)) { // we fill a hole
        __eraseHole(id);
      } else {
        GUM_ERROR(DuplicateElement, "Id " << id << " is already used");
      }
    }

    GUM_EMIT1(onNodeAdded, id);
  }

  void NodeGraphPart::__clearNodes(void) {
    NodeId bound = __bound;
    __bound = 0;

    if (onNodeDeleted.hasListener()) {
      for (NodeId n = 0; n < bound; ++n) {
        if (!__inHoles(n))
          GUM_EMIT1(onNodeDeleted, n);
      }
    }

    __updateEndIteratorSafe();

    delete (__holes);

    __holes = 0;
  }

  void NodeGraphPartIteratorSafe::whenNodeDeleted(const void *src, NodeId id) {
    if (id == _pos) { // we just deleted the _pos in NodeGraphPart
      _valid = false;
    }

    if (_pos >= _nodes->bound()) { // moreover, it was the last position
      _pos = _nodes->bound();
      _valid = false;
    }
  }

} /* namespace gum */
