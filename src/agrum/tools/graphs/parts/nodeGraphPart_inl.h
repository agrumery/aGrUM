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
 * @brief Inline implementation of the base node set class for graphs
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
      if (!nodes_->_inHoles_(pos_)) {
        valid_ = true;
        return;
      }

      ++pos_;
    }
  }

  /// default constructor
  INLINE
  NodeGraphPartIterator::NodeGraphPartIterator(const NodeGraphPart& nodes) noexcept :
      nodes_(&nodes) {
    GUM_CONSTRUCTOR(NodeGraphPartIterator);
  }

  /// copy constructor
  INLINE NodeGraphPartIterator::NodeGraphPartIterator(const NodeGraphPartIterator& it) noexcept :
      nodes_(it.nodes_), pos_(it.pos_), valid_(it.valid_) {
    GUM_CONS_CPY(NodeGraphPartIterator);
  }

  /// move constructor
  INLINE NodeGraphPartIterator::NodeGraphPartIterator(NodeGraphPartIterator&& it) noexcept :
      nodes_(it.nodes_), pos_(it.pos_), valid_(it.valid_) {
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
  bool NodeGraphPartIterator::operator==(const NodeGraphPartIterator& it) const noexcept {
    return ((pos_ == it.pos_) && (valid_ == it.valid_) && (nodes_ == it.nodes_));
  }

  /// checks whether two iterators point toward different nodes
  INLINE
  bool NodeGraphPartIterator::operator!=(const NodeGraphPartIterator& it) const noexcept {
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
    if (!valid_) { GUM_ERROR(UndefinedIteratorValue, "This iterator is not valid !") }

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
  NodeGraphPartIteratorSafe::NodeGraphPartIteratorSafe(const NodeGraphPart& nodes) :
      NodeGraphPartIterator(nodes) {
    GUM_CONNECT(*const_cast< NodeGraphPart* >(&nodes),
                onNodeDeleted,
                *this,
                NodeGraphPartIteratorSafe::whenNodeDeleted);
    GUM_CONSTRUCTOR(NodeGraphPartIteratorSafe);
  }

  /// copy constructor
  INLINE
  NodeGraphPartIteratorSafe::NodeGraphPartIteratorSafe(const NodeGraphPartIteratorSafe& it) :
      NodeGraphPartIterator(it) {
    GUM_CONNECT(*const_cast< NodeGraphPart* >(nodes_),
                onNodeDeleted,
                *this,
                NodeGraphPartIteratorSafe::whenNodeDeleted);
    GUM_CONS_CPY(NodeGraphPartIteratorSafe);
  }

  /// move constructor
  INLINE
  NodeGraphPartIteratorSafe::NodeGraphPartIteratorSafe(NodeGraphPartIteratorSafe&& it) :
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
    if (_holes_ && (!_holes_->empty())) next = *(_holes_->begin());
    else   // in other case
      next = _boundVal_;

    return next;
  }

  //  _holes_ is assumed to be not nullptr and id is assumed to be in  _holes_
  INLINE void NodeGraphPart::_eraseHole_(NodeId id) {
    _holes_->erase(id);

    if (_holes_->empty()) {
      delete _holes_;
      _holes_ = nullptr;
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
    if (_holes_ && (!_holes_->empty())) {
      newNode = *(_holes_->begin());
      _eraseHole_(newNode);
    } else {
      newNode = _boundVal_;
      ++_boundVal_;
      _updateEndIteratorSafe_();
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
    return (_holes_) ? (_boundVal_ - _holes_->size()) : _boundVal_;
  }

  INLINE Size NodeGraphPart::size() const { return sizeNodes(); }

  INLINE bool NodeGraphPart::existsNode(const NodeId node) const {
    if (node >= _boundVal_) return false;

    return (!_inHoles_(node));
  }

  INLINE bool NodeGraphPart::exists(const NodeId node) const { return existsNode(node); }

  INLINE void NodeGraphPart::eraseNode(const NodeId node) {
    if (!existsNode(node)) return;

    _addHole_(node);

    GUM_EMIT1(onNodeDeleted, node);
  }

  INLINE bool NodeGraphPart::emptyNodes() const { return (sizeNodes() == 0); }

  INLINE bool NodeGraphPart::empty() const { return emptyNodes(); }

  INLINE NodeId NodeGraphPart::bound() const { return _boundVal_; }

  INLINE void NodeGraphPart::clearNodes() { _clearNodes_(); }

  // warning: clear is an alias for clearNodes but it should never be the case
  // that the code of clear is just a call to clearNodes: as both methods are
  // virtual, this could induce bugs within the graphs hierarchy (i.e., virtual
  // functions calling recursively each other along the hierarchy) that are not
  // easy to debug. Hence, the code of clearNodes should be duplicated here.
  INLINE void NodeGraphPart::clear() { _clearNodes_(); }

  INLINE NodeGraphPartIteratorSafe NodeGraphPart::beginSafe() const {
    NodeGraphPartIteratorSafe it(*this);
    it.validate_();   // stop the iterator at the first not-in-holes
    return it;
  }

  INLINE void NodeGraphPart::_updateEndIteratorSafe_() { _endIteratorSafe_.setPos_(_boundVal_); }

  INLINE const NodeGraphPartIteratorSafe& NodeGraphPart::endSafe() const noexcept {
    return _endIteratorSafe_;
  }

  INLINE NodeGraphPartIterator NodeGraphPart::begin() const noexcept {
    NodeGraphPartIterator it(*this);
    it.validate_();   // stop the iterator at the first not-in-holes
    return it;
  }

  INLINE const NodeGraphPartIterator& NodeGraphPart::end() const noexcept {
    return _endIteratorSafe_;
  }

  INLINE bool NodeGraphPart::operator==(const NodeGraphPart& p) const {
    if (_boundVal_ != p._boundVal_) return false;

    if (_holes_)
      if (p._holes_) return (*_holes_ == *p._holes_);
      else return false;
    else if (p._holes_) return false;

    return true;
  }

  INLINE bool NodeGraphPart::operator!=(const NodeGraphPart& p) const { return !operator==(p); }

  INLINE NodeSet NodeGraphPart::asNodeSet() const {
    NodeSet son(sizeNodes());

    if (!empty()) {
      for (NodeId n = 0; n < _boundVal_; ++n) {
        if (!_inHoles_(n)) son.insert(n);
      }
    }

    return son;
  }

  INLINE const NodeGraphPart& NodeGraphPart::nodes() const {
    return *(static_cast< const NodeGraphPart* >(this));
  }

  INLINE bool NodeGraphPart::_inHoles_(NodeId id) const { return _holes_ && _holes_->contains(id); }

  /// @return the size of  _holes_
  INLINE Size NodeGraphPart::_sizeHoles_() const { return _holes_ ? _holes_->size() : (Size)0; }

} /* namespace gum */
