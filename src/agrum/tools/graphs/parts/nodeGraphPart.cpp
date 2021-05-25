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
       _holes_size_(holes_size),  _holes_resize_policy_(holes_resize_policy),
       _endIteratorSafe_(*this),  _boundVal_(0) {
     _holes_ = nullptr;
    GUM_CONSTRUCTOR(NodeGraphPart);
     _updateEndIteratorSafe_();
  }

  NodeGraphPart::NodeGraphPart(const NodeGraphPart& s) :
       _holes_size_(s. _holes_size_),  _holes_resize_policy_(s. _holes_resize_policy_),
       _endIteratorSafe_(*this),  _boundVal_(s. _boundVal_) {
     _holes_ = nullptr;

    if (s. _holes_)  _holes_ = new NodeSet(*s. _holes_);

     _updateEndIteratorSafe_();

    GUM_CONS_CPY(NodeGraphPart);
  }

  NodeGraphPart::~NodeGraphPart() {
    if ( _holes_) delete  _holes_;

    GUM_DESTRUCTOR(NodeGraphPart);
  }

  void NodeGraphPart::populateNodes(const NodeGraphPart& s) {
    clear();   // "virtual" flush of the nodes set
     _holes_size_          = s. _holes_size_;
     _holes_resize_policy_ = s. _holes_resize_policy_;

    if (s. _holes_)  _holes_ = new NodeSet(*s. _holes_);

     _boundVal_ = s. _boundVal_;

     _updateEndIteratorSafe_();
  }

  // id is assumed to belong to NodeGraphPart
  void NodeGraphPart:: _addHole_(NodeId node) {
    // we assume that the node exists
    if (node + 1 ==  _boundVal_) {
      // we remove the max : no new hole and maybe a bunch of holes to remove
      -- _boundVal_;

      if ( _holes_) {
        while ( _holes_->contains( _boundVal_ - 1)) {
          // a bunch of holes to remove. We do not use inHoles for optimisation
          // :
          // not to repeat the test if ( _holes_) each time
           _holes_->erase(-- _boundVal_);
        }

        if ( _holes_->empty()) {
          delete  _holes_;
           _holes_ = nullptr;
        }
      }

       _updateEndIteratorSafe_();
    } else {
      if (! _holes_)  _holes_ = new NodeSet( _holes_size_,  _holes_resize_policy_);

       _holes_->insert(node);
    }
  }

  std::string NodeGraphPart::toString() const {
    std::stringstream s;
    bool              first = true;
    s << "{";

    for (NodeId id = 0; id <  _boundVal_; ++id) {
      if ( _inHoles_(id)) continue;

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
    if (id >=  _boundVal_) {
      if (id >  _boundVal_) {   // we have to add holes
        if (! _holes_)  _holes_ = new NodeSet( _holes_size_,  _holes_resize_policy_);

        for (NodeId i =  _boundVal_; i < id; ++i)
           _holes_->insert(i);
      }

       _boundVal_ = id + 1;

       _updateEndIteratorSafe_();
    } else {
      if ( _inHoles_(id)) {   // we fill a hole
         _eraseHole_(id);
      } else {
        GUM_ERROR(DuplicateElement, "Id " << id << " is already used")
      }
    }

    GUM_EMIT1(onNodeAdded, id);
  }

  void NodeGraphPart:: _clearNodes_() {
    NodeId bound =  _boundVal_;
     _boundVal_   = 0;

    if (onNodeDeleted.hasListener()) {
      for (NodeId n = 0; n < bound; ++n) {
        if (! _inHoles_(n)) GUM_EMIT1(onNodeDeleted, n);
      }
    }

     _updateEndIteratorSafe_();

    delete ( _holes_);
     _holes_ = nullptr;
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
