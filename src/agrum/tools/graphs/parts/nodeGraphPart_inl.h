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
 * @brief Inline implementation of the base node set class for graphs
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *
 */

// to ease parsing by IDE
#include <agrum/tools/graphs/parts/nodeGraphPart.h>

namespace gum {

  //=================NODEGRAPHPARTITERATOR============================

  /// ensure that the nodeId is either end() either a valid NodeId
  INLINE void NodeGraphPartIterator::validate_() noexcept {
    valid_ = false;

    if (pos_ > nodes_->bound()) { pos_ = nodes_->bound(); }

    while (pos_ < nodes_->bound()) {
      if (!nodes_->inHoles__(pos_)) {
        valid_ = true;
        return;
      }

      ++pos_;
    }
  }

  /// default constructor
  INLINE
  NodeGraphPartIterator::NodeGraphPartIterator(const NodeGraphPart& nodes) noexcept
      :
      nodes_(&nodes) {
    GUM_CONSTRUCTOR(NodeGraphPartIterator);
  }

  /// copy constructor
  INLINE
  NodeGraphPartIterator::NodeGraphPartIterator(
     const NodeGraphPartIterator& it) noexcept :
      nodes_(it.nodes_),
      pos_(it.pos_), valid_(it.valid_) {
    GUM_CONS_CPY(NodeGraphPartIterator);
  }

  /// move constructor
  INLINE
  NodeGraphPartIterator::NodeGraphPartIterator(NodeGraphPartIterator&& it) noexcept
      :
      nodes_(it.nodes_),
      pos_(it.pos_), valid_(it.valid_) {
    GUM_CONS_MOV(NodeGraphPartIterator);
  }

  /// destructor
  INLINE NodeGraphPartIterator::~NodeGraphPartIterator() noexcept {
    GUM_DESTRUCTOR(NodeGraphPartIterator);
  }

  /// copy assignment operator
  INLINE NodeGraphPartIterator&
     NodeGraphPartIterator::operator=(const NodeGraphPartIterator& it) noexcept {
    nodes_ = it.nodes_;
    pos_   = it.pos_;
    valid_ = it.valid_;
    GUM_OP_CPY(NodeGraphPartIterator);

    return *this;
  }

  /// move assignment operator
  INLINE NodeGraphPartIterator&
     NodeGraphPartIterator::operator=(NodeGraphPartIterator&& it) noexcept {
    nodes_ = it.nodes_;
    pos_   = it.pos_;
    valid_ = it.valid_;
    GUM_OP_MOV(NodeGraphPartIterator);

    return *this;
  }

  /// checks whether two iterators point toward the same node
  INLINE
  bool NodeGraphPartIterator::operator==(
     const NodeGraphPartIterator& it) const noexcept {
    return ((pos_ == it.pos_) && (valid_ == it.valid_) && (nodes_ == it.nodes_));
  }

  /// checks whether two iterators point toward different nodes
  INLINE
  bool NodeGraphPartIterator::operator!=(
     const NodeGraphPartIterator& it) const noexcept {
    return !(operator==(it));
  }

  /// increment the iterator
  INLINE NodeGraphPartIterator& NodeGraphPartIterator::operator++() noexcept {
    ++pos_;
    validate_();
    return *this;
  }

  /// dereferencing operator
  INLINE NodeId NodeGraphPartIterator::operator*() const {
    if (!valid_) {
      GUM_ERROR(UndefinedIteratorValue, "This iterator is not valid !")
    }

    return pos_;
  }

  // unsafe private method
  INLINE void NodeGraphPartIterator::setPos_(NodeId id) noexcept {
    pos_ = id;

    if (pos_ >= nodes_->bound()) {
      pos_   = nodes_->bound();
      valid_ = false;
    } else {
      valid_ = nodes_->exists(pos_);
    }
  }

  //=================NODEGRAPHPARTITERATORSAFE============================

  /// default constructor
  INLINE
  NodeGraphPartIteratorSafe::NodeGraphPartIteratorSafe(
     const NodeGraphPart& nodes) :
      NodeGraphPartIterator(nodes) {
    GUM_CONNECT(*const_cast< NodeGraphPart* >(&nodes),
                onNodeDeleted,
                *this,
                NodeGraphPartIteratorSafe::whenNodeDeleted);
    GUM_CONSTRUCTOR(NodeGraphPartIteratorSafe);
  }

  /// copy constructor
  INLINE
  NodeGraphPartIteratorSafe::NodeGraphPartIteratorSafe(
     const NodeGraphPartIteratorSafe& it) :
      NodeGraphPartIterator(it) {
    GUM_CONNECT(*const_cast< NodeGraphPart* >(nodes_),
                onNodeDeleted,
                *this,
                NodeGraphPartIteratorSafe::whenNodeDeleted);
    GUM_CONS_CPY(NodeGraphPartIteratorSafe);
  }

  /// move constructor
  INLINE
  NodeGraphPartIteratorSafe::NodeGraphPartIteratorSafe(
     NodeGraphPartIteratorSafe&& it) :
      NodeGraphPartIterator(std::move(it)) {
    GUM_CONNECT(*const_cast< NodeGraphPart* >(nodes_),
                onNodeDeleted,
                *this,
                NodeGraphPartIteratorSafe::whenNodeDeleted);
    GUM_CONS_MOV(NodeGraphPartIteratorSafe);
  }

  /// destructor
  INLINE NodeGraphPartIteratorSafe::~NodeGraphPartIteratorSafe() {
    GUM_DESTRUCTOR(NodeGraphPartIteratorSafe);
  }

  /// copy assignment operator
  INLINE NodeGraphPartIteratorSafe&
     NodeGraphPartIteratorSafe::operator=(const NodeGraphPartIteratorSafe& it) {
    //  avoid self assignment
    if (&it != this) {
      NodeGraphPartIterator::operator=(it);
      Listener::             operator=(it);
      GUM_OP_CPY(NodeGraphPartIteratorSafe);
    }

    return *this;
  }

  /// move assignment operator
  INLINE NodeGraphPartIteratorSafe&
     NodeGraphPartIteratorSafe::operator=(NodeGraphPartIteratorSafe&& it) {
    //  avoid self assignment
    if (&it != this) {
      NodeGraphPartIterator::operator=(std::move(it));
      Listener::             operator=(std::move(it));
      GUM_OP_MOV(NodeGraphPartIteratorSafe);
    }

    return *this;
  }

  //=================NODEGRAPHPART============================

  INLINE NodeGraphPart& NodeGraphPart::operator=(const NodeGraphPart& p) {
    // avoid self assignment
    if (this != &p) { populateNodes(p); }

    return *this;
  }

  INLINE NodeId NodeGraphPart::nextNodeId() const {
    NodeId next = 0;

    // return the first hole if holes exist
    if (holes__ && (!holes__->empty()))
      next = *(holes__->begin());
    else   // in other case
      next = boundVal__;

    return next;
  }

  // holes__ is assumed to be not nullptr and id is assumed to be in holes__
  INLINE void NodeGraphPart::eraseHole__(NodeId id) {
    holes__->erase(id);

    if (holes__->empty()) {
      delete holes__;
      holes__ = nullptr;
    }
  }

  // warning: do not try to use function addNodeWithId ( const NodeId id ) within
  // function addNodeWithId(): as both functions are virtual, this may create
  // bugs within the graphs hierarchy (i.e., virtual functions calling
  // recursively
  // each other along the hierarchy) that are not easy to debug.
  INLINE NodeId NodeGraphPart::addNode() {
    NodeId newNode;

    // fill the first hole if holes exist
    if (holes__ && (!holes__->empty())) {
      newNode = *(holes__->begin());
      eraseHole__(newNode);
    } else {
      newNode = boundVal__;
      ++boundVal__;
      updateEndIteratorSafe__();
    }

    GUM_EMIT1(onNodeAdded, newNode);

    return newNode;
  }

  INLINE std::vector< NodeId > NodeGraphPart::addNodes(Size N) {
    std::vector< NodeId > v;
    v.reserve(N);
    for (Idx i = 0; i < N; i++)
      v.push_back(this->addNode());
    return v;
  }


  INLINE Size NodeGraphPart::sizeNodes() const {
    return (holes__) ? (boundVal__ - holes__->size()) : boundVal__;
  }

  INLINE Size NodeGraphPart::size() const { return sizeNodes(); }

  INLINE bool NodeGraphPart::existsNode(const NodeId node) const {
    if (node >= boundVal__) return false;

    return (!inHoles__(node));
  }

  INLINE bool NodeGraphPart::exists(const NodeId node) const {
    return existsNode(node);
  }

  INLINE void NodeGraphPart::eraseNode(const NodeId node) {
    if (!existsNode(node)) return;

    addHole__(node);

    GUM_EMIT1(onNodeDeleted, node);
  }

  INLINE bool NodeGraphPart::emptyNodes() const { return (sizeNodes() == 0); }

  INLINE bool NodeGraphPart::empty() const { return emptyNodes(); }

  INLINE NodeId NodeGraphPart::bound() const { return boundVal__; }

  INLINE void NodeGraphPart::clearNodes() { clearNodes__(); }

  // warning: clear is an alias for clearNodes but it should never be the case
  // that the code of clear is just a call to clearNodes: as both methods are
  // virtual, this could induce bugs within the graphs hierarchy (i.e., virtual
  // functions calling recursively each other along the hierarchy) that are not
  // easy to debug. Hence, the code of clearNodes should be duplicated here.
  INLINE void NodeGraphPart::clear() { clearNodes__(); }

  INLINE NodeGraphPartIteratorSafe NodeGraphPart::beginSafe() const {
    NodeGraphPartIteratorSafe it(*this);
    it.validate_();   // stop the iterator at the first not-in-holes
    return it;
  }

  INLINE void NodeGraphPart::updateEndIteratorSafe__() {
    endIteratorSafe__.setPos_(boundVal__);
  }

  INLINE const NodeGraphPartIteratorSafe& NodeGraphPart::endSafe() const noexcept {
    return endIteratorSafe__;
  }

  INLINE NodeGraphPartIterator NodeGraphPart::begin() const noexcept {
    NodeGraphPartIterator it(*this);
    it.validate_();   // stop the iterator at the first not-in-holes
    return it;
  }

  INLINE const NodeGraphPartIterator& NodeGraphPart::end() const noexcept {
    return endIteratorSafe__;
  }

  INLINE bool NodeGraphPart::operator==(const NodeGraphPart& p) const {
    if (boundVal__ != p.boundVal__) return false;

    if (holes__)
      if (p.holes__)
        return (*holes__ == *p.holes__);
      else
        return false;
    else if (p.holes__)
      return false;

    return true;
  }

  INLINE bool NodeGraphPart::operator!=(const NodeGraphPart& p) const {
    return !operator==(p);
  }

  INLINE NodeSet NodeGraphPart::asNodeSet() const {
    NodeSet son(sizeNodes());

    if (!empty()) {
      for (NodeId n = 0; n < boundVal__; ++n) {
        if (!inHoles__(n)) son.insert(n);
      }
    }

    return son;
  }

  INLINE const NodeGraphPart& NodeGraphPart::nodes() const {
    return *(static_cast< const NodeGraphPart* >(this));
  }

  INLINE bool NodeGraphPart::inHoles__(NodeId id) const {
    return holes__ && holes__->contains(id);
  }

  /// @return the size of holes__
  INLINE Size NodeGraphPart::sizeHoles__() const {
    return holes__ ? holes__->size() : (Size)0;
  }

} /* namespace gum */
