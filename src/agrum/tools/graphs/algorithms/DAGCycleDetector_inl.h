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
 * @brief A class for detecting directed cycles in DAGs when trying to apply
 * many changes to the graph
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

namespace gum {

  /* ===========================================================================
   */
  // CHANGES
  /* ===========================================================================
   */

  // default constructor
  INLINE DAGCycleDetector::Change::Change(ChangeType type, NodeId tail, NodeId head) noexcept :
      _type_{type}, _tail_{tail}, _head_{head} {
    GUM_CONSTRUCTOR(DAGCycleDetector::Change);
  }

  // copy constructor
  INLINE DAGCycleDetector::Change::Change(const DAGCycleDetector::Change& from) noexcept :
      _type_{from._type_}, _tail_{from._tail_}, _head_{from._head_} {
    GUM_CONS_CPY(DAGCycleDetector::Change);
  }

  // move constructor
  INLINE DAGCycleDetector::Change::Change(DAGCycleDetector::Change&& from) noexcept :
      _type_{from._type_}, _tail_{from._tail_}, _head_{from._head_} {
    GUM_CONS_MOV(DAGCycleDetector::Change);
  }

  // destructor
  INLINE DAGCycleDetector::Change::~Change() noexcept { GUM_DESTRUCTOR(DAGCycleDetector::Change); }

  // copy operator
  INLINE DAGCycleDetector::Change&
     DAGCycleDetector::Change::operator=(const DAGCycleDetector::Change& from) noexcept {
    _type_ = from._type_;
    _tail_ = from._tail_;
    _head_ = from._head_;
    return *this;
  }

  // move operator
  INLINE DAGCycleDetector::Change&
     DAGCycleDetector::Change::operator=(DAGCycleDetector::Change&& from) noexcept {
    _type_ = from._type_;
    _tail_ = from._tail_;
    _head_ = from._head_;
    return *this;
  }

  /// returns the type of the operation
  INLINE DAGCycleDetector::ChangeType DAGCycleDetector::Change::type() const noexcept {
    return _type_;
  }

  /// indicates the tail of the arc involved in the modification
  INLINE NodeId DAGCycleDetector::Change::tail() const noexcept { return _tail_; }

  /// indicates the head of the arc involved in the modification
  INLINE NodeId DAGCycleDetector::Change::head() const noexcept { return _head_; }

  /* ===========================================================================
   */
  // ArcAdd
  /* ===========================================================================
   */

  /// default constructor
  INLINE DAGCycleDetector::ArcAdd::ArcAdd(NodeId tail, NodeId head) noexcept :
      DAGCycleDetector::Change(DAGCycleDetector::ChangeType::ARC_ADDITION, tail, head) {
    GUM_CONSTRUCTOR(DAGCycleDetector::ArcAdd);
  }

  /// copy constructor
  INLINE DAGCycleDetector::ArcAdd::ArcAdd(const DAGCycleDetector::ArcAdd& from) noexcept :
      DAGCycleDetector::Change(from.type(), from.tail(), from.head()) {
    GUM_CONS_CPY(DAGCycleDetector::ArcAdd);
  }

  /// move constructor
  INLINE DAGCycleDetector::ArcAdd::ArcAdd(DAGCycleDetector::ArcAdd&& from) noexcept :
      DAGCycleDetector::Change(std::move(from.type()),
                               std::move(from.tail()),
                               std::move(from.head())) {
    GUM_CONS_MOV(DAGCycleDetector::ArcAdd);
  }

  /// destructor
  INLINE DAGCycleDetector::ArcAdd::~ArcAdd() noexcept { GUM_DESTRUCTOR(DAGCycleDetector::ArcAdd); }

  /// copy operator
  INLINE DAGCycleDetector::ArcAdd&
     DAGCycleDetector::ArcAdd::operator=(const DAGCycleDetector::ArcAdd& from) noexcept {
    DAGCycleDetector::Change::operator=(from);
    return *this;
  }

  /// move operator
  INLINE DAGCycleDetector::ArcAdd&
     DAGCycleDetector::ArcAdd::operator=(DAGCycleDetector::ArcAdd&& from) noexcept {
    DAGCycleDetector::Change::operator=(std::move(from));
    return *this;
  }

  /* ===========================================================================
   */
  // ArcDel
  /* ===========================================================================
   */

  /// default constructor
  INLINE DAGCycleDetector::ArcDel::ArcDel(NodeId tail, NodeId head) noexcept :
      DAGCycleDetector::Change(DAGCycleDetector::ChangeType::ARC_DELETION, tail, head) {
    GUM_CONSTRUCTOR(DAGCycleDetector::ArcDel);
  }

  /// copy constructor
  INLINE DAGCycleDetector::ArcDel::ArcDel(const DAGCycleDetector::ArcDel& from) noexcept :
      DAGCycleDetector::Change(from.type(), from.tail(), from.head()) {
    GUM_CONS_CPY(DAGCycleDetector::ArcDel);
  }

  /// move constructor
  INLINE DAGCycleDetector::ArcDel::ArcDel(DAGCycleDetector::ArcDel&& from) noexcept :
      DAGCycleDetector::Change(std::move(from.type()),
                               std::move(from.tail()),
                               std::move(from.head())) {
    GUM_CONS_MOV(DAGCycleDetector::ArcDel);
  }

  /// destructor
  INLINE DAGCycleDetector::ArcDel::~ArcDel() noexcept { GUM_DESTRUCTOR(DAGCycleDetector::ArcDel); }

  /// copy operator
  INLINE DAGCycleDetector::ArcDel&
     DAGCycleDetector::ArcDel::operator=(const DAGCycleDetector::ArcDel& from) noexcept {
    DAGCycleDetector::Change::operator=(from);
    return *this;
  }

  /// move operator
  INLINE DAGCycleDetector::ArcDel&
     DAGCycleDetector::ArcDel::operator=(DAGCycleDetector::ArcDel&& from) noexcept {
    DAGCycleDetector::Change::operator=(std::move(from));
    return *this;
  }

  /* ===========================================================================
   */
  // ArcReverse
  /* ===========================================================================
   */

  /// default constructor
  INLINE DAGCycleDetector::ArcReverse::ArcReverse(NodeId tail, NodeId head) noexcept :
      DAGCycleDetector::Change(DAGCycleDetector::ChangeType::ARC_REVERSAL, tail, head) {
    GUM_CONSTRUCTOR(DAGCycleDetector::ArcReverse);
  }

  /// copy constructor
  INLINE DAGCycleDetector::ArcReverse::ArcReverse(const DAGCycleDetector::ArcReverse& from) noexcept
      :
      DAGCycleDetector::Change(from.type(), from.tail(), from.head()) {
    GUM_CONS_CPY(DAGCycleDetector::ArcReverse);
  }

  /// move constructor
  INLINE DAGCycleDetector::ArcReverse::ArcReverse(DAGCycleDetector::ArcReverse&& from) noexcept :
      DAGCycleDetector::Change(std::move(from.type()),
                               std::move(from.tail()),
                               std::move(from.head())) {
    GUM_CONS_MOV(DAGCycleDetector::ArcReverse);
  }

  /// destructor
  INLINE DAGCycleDetector::ArcReverse::~ArcReverse() noexcept {
    GUM_DESTRUCTOR(DAGCycleDetector::ArcReverse);
  }

  /// copy operator
  INLINE DAGCycleDetector::ArcReverse&
     DAGCycleDetector::ArcReverse::operator=(const DAGCycleDetector::ArcReverse& from) noexcept {
    DAGCycleDetector::Change::operator=(from);
    return *this;
  }

  /// move operator
  INLINE DAGCycleDetector::ArcReverse&
     DAGCycleDetector::ArcReverse::operator=(DAGCycleDetector::ArcReverse&& from) noexcept {
    DAGCycleDetector::Change::operator=(std::move(from));
    return *this;
  }

  /* ===========================================================================
   */
  // DAGCycleDetector
  /* ===========================================================================
   */

  /// default constructor
  INLINE DAGCycleDetector::DAGCycleDetector() noexcept { GUM_CONSTRUCTOR(DAGCycleDetector); }

  /// copy constructor
  INLINE DAGCycleDetector::DAGCycleDetector(const DAGCycleDetector& from) :
      _dag_(from._dag_), _ancestors_(from._ancestors_), _descendants_(from._descendants_) {
    GUM_CONS_CPY(DAGCycleDetector);
  }

  /// move constructor
  INLINE DAGCycleDetector::DAGCycleDetector(DAGCycleDetector&& from) :
      _dag_(std::move(from._dag_)), _ancestors_(std::move(from._ancestors_)),
      _descendants_(std::move(from._descendants_)) {
    GUM_CONS_MOV(DAGCycleDetector);
  }

  /// destructor
  INLINE DAGCycleDetector::~DAGCycleDetector() { GUM_DESTRUCTOR(DAGCycleDetector); }

  /// copy operator
  INLINE
  DAGCycleDetector& DAGCycleDetector::operator=(const DAGCycleDetector& from) {
    if (this != &from) {
      _dag_         = from._dag_;
      _ancestors_   = from._ancestors_;
      _descendants_ = from._descendants_;
    }

    return *this;
  }

  /// move operator
  INLINE DAGCycleDetector& DAGCycleDetector::operator=(DAGCycleDetector&& from) {
    if (this != &from) {
      _dag_         = std::move(from._dag_);
      _ancestors_   = std::move(from._ancestors_);
      _descendants_ = std::move(from._descendants_);
    }

    return *this;
  }

  /// indicates whether an arc addition would create a cycle
  INLINE bool DAGCycleDetector::hasCycleFromAddition(NodeId x, NodeId y) const noexcept {
    return _descendants_[y].exists(x);
  }

  /// indicates wether an arc reversal would create a cycle
  INLINE bool DAGCycleDetector::hasCycleFromReversal(NodeId x, NodeId y) const noexcept {
    return (_ancestors_[y][x] > 1);
  }

  /// indicates whether an arc deletion would create a cycle
  INLINE bool DAGCycleDetector::hasCycleFromDeletion(NodeId x, NodeId y) const noexcept {
    return false;
  }

  /// adds a nodeset to another (nodes are weighted, so weights are added)
  INLINE
  void DAGCycleDetector::_addWeightedSet_(NodeProperty< Size >&       nodeset,
                                          const NodeProperty< Size >& set_to_add,
                                          Size                        multiplier) const {
    for (auto iter = set_to_add.cbegin(); iter != set_to_add.cend(); ++iter) {
      if (nodeset.exists(iter.key())) {
        nodeset[iter.key()] += iter.val() * multiplier;
      } else {
        nodeset.insert(iter.key(), iter.val() * multiplier);
      }
    }
  }

  /// removes a weighted nodeset from another (weights are subtracted)
  INLINE
  void DAGCycleDetector::_delWeightedSet_(NodeProperty< Size >&       nodeset,
                                          const NodeProperty< Size >& set_to_del,
                                          Size                        multiplier) const {
    for (auto iter = set_to_del.cbegin(); iter != set_to_del.cend(); ++iter) {
      if (nodeset.exists(iter.key())) {
        Size& weight = nodeset[iter.key()];
        weight -= iter.val() * multiplier;

        if (!weight) { nodeset.erase(iter.key()); }
      }
    }
  }

  /** @brief put into a weighted nodeset the nodes of another weighted set that
   * belong to a set of arc extremities */
  INLINE
  void DAGCycleDetector::_restrictWeightedSet_(NodeProperty< Size >&       result_set,
                                               const NodeProperty< Size >& set_to_restrict,
                                               const NodeSet&              extremities) const {
    for (auto iter = set_to_restrict.cbegin(); iter != set_to_restrict.cend(); ++iter) {
      if (extremities.exists(iter.key())) { result_set.insert(iter.key(), iter.val()); }
    }
  }

  /// reverses an arc from the DAG
  INLINE void DAGCycleDetector::reverseArc(NodeId tail, NodeId head) {
    if (hasCycleFromReversal(tail, head)) {
      GUM_ERROR(InvalidDirectedCycle, "the arc would create a directed into a DAG")
    }

    eraseArc(tail, head);
    addArc(head, tail);
  }

  /// check the equality between two DAGCycleDetectors
  INLINE bool DAGCycleDetector::operator==(const DAGCycleDetector& from) const {
    return (   //(  _dagmodel_ == from. _dagmodel_ ) &&
       (_ancestors_ == from._ancestors_) && (_descendants_ == from._descendants_));
  }

  /// check the inequality between two DAGCycleDetectors
  INLINE bool DAGCycleDetector::operator!=(const DAGCycleDetector& from) const {
    return !operator==(from);
  }

} /* namespace gum */
