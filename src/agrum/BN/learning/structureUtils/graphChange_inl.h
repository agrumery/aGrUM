/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief A class to account for changes in a graph
 *
 * This class shall be used by learning algorithms to notify scores, structural
 * constraints, etc, that the learnt graph has been modified.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    INLINE GraphChange::GraphChange(GraphChangeType type, NodeId node1, NodeId node2) noexcept :
        _type_{type}, _node1_{node1}, _node2_{node2} {
      GUM_CONSTRUCTOR(GraphChange);
    }

    /// copy constructor
    INLINE GraphChange::GraphChange(const GraphChange& from) noexcept :
        _type_{from._type_}, _node1_{from._node1_}, _node2_{from._node2_} {
      GUM_CONS_CPY(GraphChange);
    }

    /// move constructor
    INLINE GraphChange::GraphChange(GraphChange&& from) noexcept :
        _type_{from._type_}, _node1_{from._node1_}, _node2_{from._node2_} {
      GUM_CONS_MOV(GraphChange);
    }

    /// destructor
    INLINE GraphChange::~GraphChange() noexcept {
      GUM_DESTRUCTOR(GraphChange);
      ;
    }

    /// copy constructor
    INLINE GraphChange& GraphChange::operator=(const GraphChange& from) noexcept {
      _type_  = from._type_;
      _node1_ = from._node1_;
      _node2_ = from._node2_;
      return *this;
    }

    /// move operator
    INLINE GraphChange& GraphChange::operator=(GraphChange&& from) noexcept {
      _type_  = from._type_;
      _node1_ = from._node1_;
      _node2_ = from._node2_;
      return *this;
    }

    /// returns the type of the operation
    INLINE GraphChangeType GraphChange::type() const noexcept { return _type_; }

    /// returns the first node involved in the modification
    INLINE NodeId GraphChange::node1() const noexcept { return _node1_; }

    /// returns the second node involved in the modification
    INLINE NodeId GraphChange::node2() const noexcept { return _node2_; }

    /// returns whether two graph changes are identical or not
    INLINE bool GraphChange::operator==(const GraphChange& from) const noexcept {
      return ((_node1_ == from._node1_) && (_node2_ == from._node2_) && (_type_ == from._type_));
    }

    /// returns whether two graph changes are different or not
    INLINE bool GraphChange::operator!=(const GraphChange& from) const noexcept {
      return !operator==(from);
    }

    // ===========================================================================

    /// default constructor
    INLINE ArcAddition::ArcAddition(NodeId node1, NodeId node2) noexcept :
        GraphChange(GraphChangeType::ARC_ADDITION, node1, node2) {
      // do not use GUM_CONSTRUCTOR here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// copy constructor
    INLINE ArcAddition::ArcAddition(const ArcAddition& from) noexcept : GraphChange(from) {
      // do not use GUM_CONS_CPY here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// move constructor
    INLINE ArcAddition::ArcAddition(ArcAddition&& from) noexcept : GraphChange(std::move(from)) {
      // do not use GUM_CONS_MOV here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// destructor
    INLINE ArcAddition::~ArcAddition() noexcept {
      // do not use GUM_DESTRUCTOR here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// copy constructor
    INLINE ArcAddition& ArcAddition::operator=(const ArcAddition& from) noexcept {
      GraphChange::operator=(from);
      return *this;
    }

    /// move operator
    INLINE ArcAddition& ArcAddition::operator=(ArcAddition&& from) noexcept {
      GraphChange::operator=(std::move(from));
      return *this;
    }

    /// returns whether two graph changes are identical or not
    INLINE bool ArcAddition::operator==(const ArcAddition& from) const noexcept {
      return ((node1() == from.node1()) && (node2() == from.node2()));
    }

    /// returns whether two graph changes are different or not
    INLINE bool ArcAddition::operator!=(const ArcAddition& from) const noexcept {
      return !operator==(from);
    }

    // ===========================================================================

    /// default constructor
    INLINE ArcDeletion::ArcDeletion(NodeId node1, NodeId node2) noexcept :
        GraphChange(GraphChangeType::ARC_DELETION, node1, node2) {
      // do not use GUM_CONSTRUCTOR here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// copy constructor
    INLINE ArcDeletion::ArcDeletion(const ArcDeletion& from) noexcept : GraphChange(from) {
      // do not use GUM_CONS_CPY here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// move constructor
    INLINE ArcDeletion::ArcDeletion(ArcDeletion&& from) noexcept : GraphChange(std::move(from)) {
      // do not use GUM_CONS_MOV here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// destructor
    INLINE ArcDeletion::~ArcDeletion() noexcept {
      // do not use GUM_DESTRUCTOR here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// copy constructor
    INLINE ArcDeletion& ArcDeletion::operator=(const ArcDeletion& from) noexcept {
      GraphChange::operator=(from);
      return *this;
    }

    /// move operator
    INLINE ArcDeletion& ArcDeletion::operator=(ArcDeletion&& from) noexcept {
      GraphChange::operator=(std::move(from));
      return *this;
    }

    /// returns whether two graph changes are identical or not
    INLINE bool ArcDeletion::operator==(const ArcDeletion& from) const noexcept {
      return ((node1() == from.node1()) && (node2() == from.node2()));
    }

    /// returns whether two graph changes are different or not
    INLINE bool ArcDeletion::operator!=(const ArcDeletion& from) const noexcept {
      return !operator==(from);
    }

    // ===========================================================================

    /// default constructor
    INLINE ArcReversal::ArcReversal(NodeId node1, NodeId node2) noexcept :
        GraphChange(GraphChangeType::ARC_REVERSAL, node1, node2) {
      // do not use GUM_CONSTRUCTOR here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// copy constructor
    INLINE ArcReversal::ArcReversal(const ArcReversal& from) noexcept : GraphChange(from) {
      // do not use GUM_CONS_CPY here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// move constructor
    INLINE ArcReversal::ArcReversal(ArcReversal&& from) noexcept : GraphChange(std::move(from)) {
      // do not use GUM_CONS_MOV here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// destructor
    INLINE ArcReversal::~ArcReversal() noexcept {
      // do not use GUM_DESTRUCTOR here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// copy constructor
    INLINE ArcReversal& ArcReversal::operator=(const ArcReversal& from) noexcept {
      GraphChange::operator=(from);
      return *this;
    }

    /// move operator
    INLINE ArcReversal& ArcReversal::operator=(ArcReversal&& from) noexcept {
      GraphChange::operator=(std::move(from));
      return *this;
    }

    /// returns whether two arc reversals are identical or not
    INLINE bool ArcReversal::operator==(const ArcReversal& from) const noexcept {
      return ((node1() == from.node1()) && (node2() == from.node2()));
    }

    /// returns whether two arc reversals are different or not
    INLINE bool ArcReversal::operator!=(const ArcReversal& from) const noexcept {
      return !operator==(from);
    }

    // ===========================================================================

    /// default constructor
    INLINE EdgeAddition::EdgeAddition(NodeId node1, NodeId node2) noexcept :
        GraphChange(GraphChangeType::EDGE_ADDITION, node1, node2) {
      // do not use GUM_CONSTRUCTOR here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// copy constructor
    INLINE EdgeAddition::EdgeAddition(const EdgeAddition& from) noexcept : GraphChange(from) {
      // do not use GUM_CONS_CPY here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// move constructor
    INLINE EdgeAddition::EdgeAddition(EdgeAddition&& from) noexcept : GraphChange(std::move(from)) {
      // do not use GUM_CONS_MOV here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// destructor
    INLINE EdgeAddition::~EdgeAddition() noexcept {
      // do not use GUM_DESTRUCTOR here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// copy constructor
    INLINE EdgeAddition& EdgeAddition::operator=(const EdgeAddition& from) noexcept {
      GraphChange::operator=(from);
      return *this;
    }

    /// move operator
    INLINE EdgeAddition& EdgeAddition::operator=(EdgeAddition&& from) noexcept {
      GraphChange::operator=(std::move(from));
      return *this;
    }

    /// returns whether two graph changes are identical or not
    INLINE bool EdgeAddition::operator==(const EdgeAddition& from) const noexcept {
      return (((node1() == from.node1()) && (node2() == from.node2()))
              || ((node1() == from.node2()) && (node2() == from.node1())));
    }

    /// returns whether two graph changes are different or not
    INLINE bool EdgeAddition::operator!=(const EdgeAddition& from) const noexcept {
      return !operator==(from);
    }

    // ===========================================================================

    /// default constructor
    INLINE EdgeDeletion::EdgeDeletion(NodeId node1, NodeId node2) noexcept :
        GraphChange(GraphChangeType::EDGE_DELETION, node1, node2) {
      // do not use GUM_CONSTRUCTOR here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// copy constructor
    INLINE EdgeDeletion::EdgeDeletion(const EdgeDeletion& from) noexcept : GraphChange(from) {
      // do not use GUM_CONS_CPY here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// move constructor
    INLINE EdgeDeletion::EdgeDeletion(EdgeDeletion&& from) noexcept : GraphChange(std::move(from)) {
      // do not use GUM_CONS_MOV here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// destructor
    INLINE EdgeDeletion::~EdgeDeletion() noexcept {
      // do not use GUM_DESTRUCTOR here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// copy constructor
    INLINE EdgeDeletion& EdgeDeletion::operator=(const EdgeDeletion& from) noexcept {
      GraphChange::operator=(from);
      return *this;
    }

    /// move operator
    INLINE EdgeDeletion& EdgeDeletion::operator=(EdgeDeletion&& from) noexcept {
      GraphChange::operator=(std::move(from));
      return *this;
    }

    /// returns whether two graph changes are identical or not
    INLINE bool EdgeDeletion::operator==(const EdgeDeletion& from) const noexcept {
      return (((node1() == from.node1()) && (node2() == from.node2()))
              || ((node1() == from.node2()) && (node2() == from.node1())));
    }

    /// returns whether two graph changes are different or not
    INLINE bool EdgeDeletion::operator!=(const EdgeDeletion& from) const noexcept {
      return !operator==(from);
    }


  } /* namespace learning */


  // ===========================================================================

  // Returns the value of a key as a Size.
  INLINE Size HashFunc< learning::GraphChange >::castToSize(const learning::GraphChange& key) {
    return Size(key.node1()) * HashFuncConst::gold + Size(key.node2()) * HashFuncConst::pi;
  }

  /// computes the hashed value of a key
  INLINE Size
     HashFunc< learning::GraphChange >::operator()(const learning::GraphChange& key) const {
    return castToSize(key) >> this->right_shift_;
  }


  // Returns the value of a key as a Size.
  INLINE Size HashFunc< learning::ArcAddition >::castToSize(const learning::ArcAddition& key) {
    return Size(key.node1()) * HashFuncConst::gold + Size(key.node2()) * HashFuncConst::pi;
  }

  /// computes the hashed value of a key
  INLINE Size
     HashFunc< learning::ArcAddition >::operator()(const learning::ArcAddition& key) const {
    return castToSize(key) >> this->right_shift_;
  }


  // Returns the value of a key as a Size.
  INLINE Size HashFunc< learning::ArcDeletion >::castToSize(const learning::ArcDeletion& key) {
    return Size(key.node1()) * HashFuncConst::gold + Size(key.node2()) * HashFuncConst::pi;
  }

  /// computes the hashed value of a key
  INLINE Size
     HashFunc< learning::ArcDeletion >::operator()(const learning::ArcDeletion& key) const {
    return castToSize(key) >> this->right_shift_;
  }


  // Returns the value of a key as a Size.
  INLINE Size HashFunc< learning::ArcReversal >::castToSize(const learning::ArcReversal& key) {
    return Size(key.node1()) * HashFuncConst::gold + Size(key.node2()) * HashFuncConst::pi;
  }

  /// computes the hashed value of a key
  INLINE Size
     HashFunc< learning::ArcReversal >::operator()(const learning::ArcReversal& key) const {
    return castToSize(key) >> this->right_shift_;
  }


  // Returns the value of a key as a Size.
  INLINE Size HashFunc< learning::EdgeAddition >::castToSize(const learning::EdgeAddition& key) {
    return Size(key.node1()) * HashFuncConst::gold + Size(key.node2()) * HashFuncConst::pi;
  }

  /// computes the hashed value of a key
  INLINE Size
     HashFunc< learning::EdgeAddition >::operator()(const learning::EdgeAddition& key) const {
    return castToSize(key) >> this->right_shift_;
  }


  // Returns the value of a key as a Size.
  INLINE Size HashFunc< learning::EdgeDeletion >::castToSize(const learning::EdgeDeletion& key) {
    return Size(key.node1()) * HashFuncConst::gold + Size(key.node2()) * HashFuncConst::pi;
  }

  /// computes the hashed value of a key
  INLINE Size
     HashFunc< learning::EdgeDeletion >::operator()(const learning::EdgeDeletion& key) const {
    return castToSize(key) >> this->right_shift_;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
