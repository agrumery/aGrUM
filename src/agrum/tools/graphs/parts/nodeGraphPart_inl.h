
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
 * @brief Inline implementation of the base node set class for graphs
 *
 * @author Pierre-Henri WUILLEMIN (@LIP6) and Christophe GONZALES (@AMU)
 *
 */

// to ease parsing by IDE
#include <agrum/tools/graphs/parts/nodeGraphPart.h>

namespace gum {

  //=================NODEGRAPHPARTITERATOR============================

  /// ensure that the nodeId is either end() either a valid NodeId
  INLINE void NodeGraphPartIterator::_validate() noexcept {
    _valid = false;

    if (_pos > _nodes->bound()) { _pos = _nodes->bound(); }

    while (_pos < _nodes->bound()) {
      if (!_nodes->__inHoles(_pos)) {
        _valid = true;
        return;
      }

      ++_pos;
    }
  }

  /// default constructor
  INLINE
  NodeGraphPartIterator::NodeGraphPartIterator(
     const NodeGraphPart& nodes) noexcept :
      _nodes(&nodes) {
    GUM_CONSTRUCTOR(NodeGraphPartIterator);
  }

  /// copy constructor
  INLINE
  NodeGraphPartIterator::NodeGraphPartIterator(
     const NodeGraphPartIterator& it) noexcept :
      _nodes(it._nodes),
      _pos(it._pos), _valid(it._valid) {
    GUM_CONS_CPY(NodeGraphPartIterator);
  }

  /// move constructor
  INLINE
  NodeGraphPartIterator::NodeGraphPartIterator(
     NodeGraphPartIterator&& it) noexcept :
      _nodes(it._nodes),
      _pos(it._pos), _valid(it._valid) {
    GUM_CONS_MOV(NodeGraphPartIterator);
  }

  /// destructor
  INLINE NodeGraphPartIterator::~NodeGraphPartIterator() noexcept {
    GUM_DESTRUCTOR(NodeGraphPartIterator);
  }

  /// copy assignment operator
  INLINE NodeGraphPartIterator&
     NodeGraphPartIterator::operator=(const NodeGraphPartIterator& it) noexcept {
    _nodes = it._nodes;
    _pos = it._pos;
    _valid = it._valid;
    GUM_OP_CPY(NodeGraphPartIterator);

    return *this;
  }

  /// move assignment operator
  INLINE NodeGraphPartIterator&
     NodeGraphPartIterator::operator=(NodeGraphPartIterator&& it) noexcept {
    _nodes = it._nodes;
    _pos = it._pos;
    _valid = it._valid;
    GUM_OP_MOV(NodeGraphPartIterator);

    return *this;
  }

  /// checks whether two iterators point toward the same node
  INLINE
  bool NodeGraphPartIterator::operator==(const NodeGraphPartIterator& it) const
     noexcept {
    return ((_pos == it._pos) && (_valid == it._valid) && (_nodes == it._nodes));
  }

  /// checks whether two iterators point toward different nodes
  INLINE
  bool NodeGraphPartIterator::operator!=(const NodeGraphPartIterator& it) const
     noexcept {
    return !(operator==(it));
  }

  /// increment the iterator
  INLINE NodeGraphPartIterator& NodeGraphPartIterator::operator++() noexcept {
    ++_pos;
    _validate();
    return *this;
  }

  /// dereferencing operator
  INLINE NodeId NodeGraphPartIterator::operator*() const {
    if (!_valid) {
      GUM_ERROR(UndefinedIteratorValue, "This iterator is not valid !");
    }

    return _pos;
  }

  // unsafe private method
  INLINE void NodeGraphPartIterator::_setPos(NodeId id) noexcept {
    _pos = id;

    if (_pos >= _nodes->bound()) {
      _pos = _nodes->bound();
      _valid = false;
    } else {
      _valid = _nodes->exists(_pos);
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
    GUM_CONNECT(*const_cast< NodeGraphPart* >(_nodes),
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
    GUM_CONNECT(*const_cast< NodeGraphPart* >(_nodes),
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
    if (__holes && (!__holes->empty()))
      next = *(__holes->begin());
    else   // in other case
      next = __boundVal;

    return next;
  }

  // __holes is assumed to be not nullptr and id is assumed to be in __holes
  INLINE void NodeGraphPart::__eraseHole(NodeId id) {
    __holes->erase(id);

    if (__holes->empty()) {
      delete __holes;
      __holes = nullptr;
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
    if (__holes && (!__holes->empty())) {
      newNode = *(__holes->begin());
      __eraseHole(newNode);
    } else {
      newNode = __boundVal;
      ++__boundVal;
      __updateEndIteratorSafe();
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
    return (__holes) ? (__boundVal - __holes->size()) : __boundVal;
  }

  INLINE Size NodeGraphPart::size() const { return sizeNodes(); }

  INLINE bool NodeGraphPart::existsNode(const NodeId node) const {
    if (node >= __boundVal) return false;

    return (!__inHoles(node));
  }

  INLINE bool NodeGraphPart::exists(const NodeId node) const {
    return existsNode(node);
  }

  INLINE void NodeGraphPart::eraseNode(const NodeId node) {
    if (!existsNode(node)) return;

    __addHole(node);

    GUM_EMIT1(onNodeDeleted, node);
  }

  INLINE bool NodeGraphPart::emptyNodes() const { return (sizeNodes() == 0); }

  INLINE bool NodeGraphPart::empty() const { return emptyNodes(); }

  INLINE NodeId NodeGraphPart::bound() const { return __boundVal; }

  INLINE void NodeGraphPart::clearNodes() { __clearNodes(); }

  // warning: clear is an alias for clearNodes but it should never be the case
  // that the code of clear is just a call to clearNodes: as both methods are
  // virtual, this could induce bugs within the graphs hierarchy (i.e., virtual
  // functions calling recursively each other along the hierarchy) that are not
  // easy to debug. Hence, the code of clearNodes should be duplicated here.
  INLINE void NodeGraphPart::clear() { __clearNodes(); }

  INLINE NodeGraphPartIteratorSafe NodeGraphPart::beginSafe() const {
    NodeGraphPartIteratorSafe it(*this);
    it._validate();   // stop the iterator at the first not-in-holes
    return it;
  }

  INLINE void NodeGraphPart::__updateEndIteratorSafe() {
    __endIteratorSafe._setPos(__boundVal);
  }

  INLINE const NodeGraphPartIteratorSafe& NodeGraphPart::endSafe() const noexcept {
    return __endIteratorSafe;
  }

  INLINE NodeGraphPartIterator NodeGraphPart::begin() const noexcept {
    NodeGraphPartIterator it(*this);
    it._validate();   // stop the iterator at the first not-in-holes
    return it;
  }

  INLINE const NodeGraphPartIterator& NodeGraphPart::end() const noexcept {
    return __endIteratorSafe;
  }

  INLINE bool NodeGraphPart::operator==(const NodeGraphPart& p) const {
    if (__boundVal != p.__boundVal) return false;

    if (__holes)
      if (p.__holes)
        return (*__holes == *p.__holes);
      else
        return false;
    else if (p.__holes)
      return false;

    return true;
  }

  INLINE bool NodeGraphPart::operator!=(const NodeGraphPart& p) const {
    return !operator==(p);
  }

  INLINE NodeSet NodeGraphPart::asNodeSet() const {
    NodeSet son(sizeNodes());

    if (!empty()) {
      for (NodeId n = 0; n < __boundVal; ++n) {
        if (!__inHoles(n)) son.insert(n);
      }
    }

    return son;
  }

  INLINE const NodeGraphPart& NodeGraphPart::nodes() const {
    return *(static_cast< const NodeGraphPart* >(this));
  }

  INLINE bool NodeGraphPart::__inHoles(NodeId id) const {
    return __holes && __holes->contains(id);
  }

  /// @return the size of __holes
  INLINE Size NodeGraphPart::__sizeHoles() const {
    return __holes ? __holes->size() : (Size)0;
  }

} /* namespace gum */