/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @author Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *
 */
#include <agrum/tools/graphs/parts/nodeGraphPart.h>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/graphs/parts/nodeGraphPart_inl.h>
#endif   // GUM_NOINLINE

namespace gum {

  ///////////////////// NodeGraphPart
  NodeGraphPart::NodeGraphPart(Size holes_size, bool holes_resize_policy) :
      holes_size__(holes_size), holes_resize_policy__(holes_resize_policy),
      endIteratorSafe__(*this), boundVal__(0) {
    holes__ = nullptr;
    GUM_CONSTRUCTOR(NodeGraphPart);
    updateEndIteratorSafe__();
  }

  NodeGraphPart::NodeGraphPart(const NodeGraphPart& s) :
      holes_size__(s.holes_size__), holes_resize_policy__(s.holes_resize_policy__),
      endIteratorSafe__(*this), boundVal__(s.boundVal__) {
    holes__ = nullptr;

    if (s.holes__) holes__ = new NodeSet(*s.holes__);

    updateEndIteratorSafe__();

    GUM_CONS_CPY(NodeGraphPart);
  }

  NodeGraphPart::~NodeGraphPart() {
    if (holes__) delete holes__;

    GUM_DESTRUCTOR(NodeGraphPart);
  }

  void NodeGraphPart::populateNodes(const NodeGraphPart& s) {
    clear();   // "virtual" flush of the nodes set
    holes_size__          = s.holes_size__;
    holes_resize_policy__ = s.holes_resize_policy__;

    if (s.holes__) holes__ = new NodeSet(*s.holes__);

    boundVal__ = s.boundVal__;

    updateEndIteratorSafe__();
  }

  // id is assumed to belong to NodeGraphPart
  void NodeGraphPart::addHole__(NodeId node) {
    // we assume that the node exists
    if (node + 1 == boundVal__) {
      // we remove the max : no new hole and maybe a bunch of holes to remove
      --boundVal__;

      if (holes__) {
        while (holes__->contains(boundVal__ - 1)) {
          // a bunch of holes to remove. We do not use inHoles for optimisation
          // :
          // not to repeat the test if (holes__) each time
          holes__->erase(--boundVal__);
        }

        if (holes__->empty()) {
          delete holes__;
          holes__ = nullptr;
        }
      }

      updateEndIteratorSafe__();
    } else {
      if (!holes__) holes__ = new NodeSet(holes_size__, holes_resize_policy__);

      holes__->insert(node);
    }
  }

  std::string NodeGraphPart::toString() const {
    std::stringstream s;
    bool              first = true;
    s << "{";

    for (NodeId id = 0; id < boundVal__; ++id) {
      if (inHoles__(id)) continue;

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
    if (id >= boundVal__) {
      if (id > boundVal__) {   // we have to add holes
        if (!holes__) holes__ = new NodeSet(holes_size__, holes_resize_policy__);

        for (NodeId i = boundVal__; i < id; ++i)
          holes__->insert(i);
      }

      boundVal__ = id + 1;

      updateEndIteratorSafe__();
    } else {
      if (inHoles__(id)) {   // we fill a hole
        eraseHole__(id);
      } else {
        GUM_ERROR(DuplicateElement, "Id " << id << " is already used")
      }
    }

    GUM_EMIT1(onNodeAdded, id);
  }

  void NodeGraphPart::clearNodes__() {
    NodeId bound = boundVal__;
    boundVal__   = 0;

    if (onNodeDeleted.hasListener()) {
      for (NodeId n = 0; n < bound; ++n) {
        if (!inHoles__(n)) GUM_EMIT1(onNodeDeleted, n);
      }
    }

    updateEndIteratorSafe__();

    delete (holes__);
    holes__ = nullptr;
  }

  void NodeGraphPartIteratorSafe::whenNodeDeleted(const void* src, NodeId id) {
    if (id == pos_) {   // we just deleted the pos_ in NodeGraphPart
      valid_ = false;
    }

    if (pos_ >= nodes_->bound()) {   // moreover, it was the last position
      pos_   = nodes_->bound();
      valid_ = false;
    }
  }

} /* namespace gum */
