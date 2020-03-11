
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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
 * @brief Source implementation of nodes sets
 *
 * @author Pierre-Henri WUILLEMIN (@LIP6) and Christophe GONZALES (@AMU)
 *
 */
#include <agrum/tools/graphs/parts/nodeGraphPart.h>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/graphs/parts/nodeGraphPart_inl.h>
#endif   // GUM_NOINLINE

namespace gum {

  ///////////////////// NodeGraphPart
  NodeGraphPart::NodeGraphPart(Size holes_size, bool holes_resize_policy) :
      __holes_size(holes_size), __holes_resize_policy(holes_resize_policy),
      __endIteratorSafe(*this), __boundVal(0) {
    __holes = nullptr;
    GUM_CONSTRUCTOR(NodeGraphPart);
    __updateEndIteratorSafe();
  }

  NodeGraphPart::NodeGraphPart(const NodeGraphPart& s) :
      __holes_size(s.__holes_size), __holes_resize_policy(s.__holes_resize_policy),
      __endIteratorSafe(*this), __boundVal(s.__boundVal) {
    __holes = nullptr;

    if (s.__holes) __holes = new NodeSet(*s.__holes);

    __updateEndIteratorSafe();

    GUM_CONS_CPY(NodeGraphPart);
  }

  NodeGraphPart::~NodeGraphPart() {
    if (__holes) delete __holes;

    GUM_DESTRUCTOR(NodeGraphPart);
  }

  void NodeGraphPart::populateNodes(const NodeGraphPart& s) {
    clear();   // "virtual" flush of the nodes set
    __holes_size = s.__holes_size;
    __holes_resize_policy = s.__holes_resize_policy;

    if (s.__holes) __holes = new NodeSet(*s.__holes);

    __boundVal = s.__boundVal;

    __updateEndIteratorSafe();
  }

  // id is assumed to belong to NodeGraphPart
  void NodeGraphPart::__addHole(NodeId node) {
    // we assume that the node exists
    if (node + 1 == __boundVal) {
      // we remove the max : no new hole and maybe a bunch of holes to remove
      --__boundVal;

      if (__holes) {
        while (__holes->contains(__boundVal - 1)) {
          // a bunch of holes to remove. We do not use inHoles for optimisation
          // :
          // not to repeat the test if (__holes) each time
          __holes->erase(--__boundVal);
        }

        if (__holes->empty()) {
          delete __holes;
          __holes = nullptr;
        }
      }

      __updateEndIteratorSafe();
    } else {
      if (!__holes) __holes = new NodeSet(__holes_size, __holes_resize_policy);

      __holes->insert(node);
    }
  }

  std::string NodeGraphPart::toString() const {
    std::stringstream s;
    bool              first = true;
    s << "{";

    for (NodeId id = 0; id < __boundVal; ++id) {
      if (__inHoles(id)) continue;

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

  std::ostream& operator<<(std::ostream& stream, const NodeGraphPart& set) {
    stream << set.toString();
    return stream;
  }

  void NodeGraphPart::addNodeWithId(const NodeId id) {
    if (id >= __boundVal) {
      if (id > __boundVal) {   // we have to add holes
        if (!__holes) __holes = new NodeSet(__holes_size, __holes_resize_policy);

        for (NodeId i = __boundVal; i < id; ++i)
          __holes->insert(i);
      }

      __boundVal = id + 1;

      __updateEndIteratorSafe();
    } else {
      if (__inHoles(id)) {   // we fill a hole
        __eraseHole(id);
      } else {
        GUM_ERROR(DuplicateElement, "Id " << id << " is already used");
      }
    }

    GUM_EMIT1(onNodeAdded, id);
  }

  void NodeGraphPart::__clearNodes() {
    NodeId bound = __boundVal;
    __boundVal = 0;

    if (onNodeDeleted.hasListener()) {
      for (NodeId n = 0; n < bound; ++n) {
        if (!__inHoles(n)) GUM_EMIT1(onNodeDeleted, n);
      }
    }

    __updateEndIteratorSafe();

    delete (__holes);
    __holes = nullptr;
  }

  void NodeGraphPartIteratorSafe::whenNodeDeleted(const void* src, NodeId id) {
    if (id == _pos) {   // we just deleted the _pos in NodeGraphPart
      _valid = false;
    }

    if (_pos >= _nodes->bound()) {   // moreover, it was the last position
      _pos = _nodes->bound();
      _valid = false;
    }
  }

} /* namespace gum */
