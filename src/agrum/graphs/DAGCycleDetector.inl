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
 * @brief A class for detecting directed cycles in DAGs when trying to apply
 * many changes to the graph
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

namespace gum {

  /* =========================================================================== */
  // CHANGES
  /* =========================================================================== */

  // default constructor
  INLINE DAGCycleDetector::Change::Change(ChangeType type, NodeId tail,
                                          NodeId head) noexcept : __type{type},
                                                                  __tail{tail},
                                                                  __head{head} {
    GUM_CONSTRUCTOR(DAGCycleDetector::Change);
  }

  // copy constructor
  INLINE
  DAGCycleDetector::Change::Change(const DAGCycleDetector::Change &from) noexcept
      : __type{from.__type},
        __tail{from.__tail},
        __head{from.__head} {
    GUM_CONS_CPY(DAGCycleDetector::Change);
  }

  // move constructor
  INLINE DAGCycleDetector::Change::Change(DAGCycleDetector::Change &&from) noexcept
      : __type{from.__type},
        __tail{from.__tail},
        __head{from.__head} {
    GUM_CONS_MOV(DAGCycleDetector::Change);
  }

  // destructor
  INLINE DAGCycleDetector::Change::~Change() noexcept {
    GUM_DESTRUCTOR(DAGCycleDetector::Change);
  }

  // copy operator
  INLINE DAGCycleDetector::Change &DAGCycleDetector::Change::
  operator=(const DAGCycleDetector::Change &from) noexcept {
    __type = from.__type;
    __tail = from.__tail;
    __head = from.__head;
    return *this;
  }

  // move operator
  INLINE DAGCycleDetector::Change &DAGCycleDetector::Change::
  operator=(DAGCycleDetector::Change &&from) noexcept {
    __type = from.__type;
    __tail = from.__tail;
    __head = from.__head;
    return *this;
  }

  /// returns the type of the operation
  INLINE DAGCycleDetector::ChangeType DAGCycleDetector::Change::type() const
      noexcept {
    return __type;
  }

  /// indicates the tail of the arc involved in the modification
  INLINE NodeId DAGCycleDetector::Change::tail() const noexcept { return __tail; }

  /// indicates the head of the arc involved in the modification
  INLINE NodeId DAGCycleDetector::Change::head() const noexcept { return __head; }

  /* =========================================================================== */
  // ArcAdd
  /* =========================================================================== */

  /// default constructor
  INLINE DAGCycleDetector::ArcAdd::ArcAdd(NodeId tail, NodeId head) noexcept
      : DAGCycleDetector::Change(DAGCycleDetector::ChangeType::ARC_ADDITION, tail,
                                 head) {
    GUM_CONSTRUCTOR(DAGCycleDetector::ArcAdd);
  }

  /// copy constructor
  INLINE
  DAGCycleDetector::ArcAdd::ArcAdd(const DAGCycleDetector::ArcAdd &from) noexcept
      : DAGCycleDetector::Change(from.type(), from.tail(), from.head()) {
    GUM_CONS_CPY(DAGCycleDetector::ArcAdd);
  }

  /// move constructor
  INLINE DAGCycleDetector::ArcAdd::ArcAdd(DAGCycleDetector::ArcAdd &&from) noexcept
      : DAGCycleDetector::Change(std::move(from.type()), std::move(from.tail()),
                                 std::move(from.head())) {
    GUM_CONS_MOV(DAGCycleDetector::ArcAdd);
  }

  /// destructor
  INLINE DAGCycleDetector::ArcAdd::~ArcAdd() noexcept {
    GUM_DESTRUCTOR(DAGCycleDetector::ArcAdd);
  }

  /// copy operator
  INLINE DAGCycleDetector::ArcAdd &DAGCycleDetector::ArcAdd::
  operator=(const DAGCycleDetector::ArcAdd &from) noexcept {
    DAGCycleDetector::Change::operator=(from);
    return *this;
  }

  /// move operator
  INLINE DAGCycleDetector::ArcAdd &DAGCycleDetector::ArcAdd::
  operator=(DAGCycleDetector::ArcAdd &&from) noexcept {
    DAGCycleDetector::Change::operator=(std::move(from));
    return *this;
  }

  /* =========================================================================== */
  // ArcDel
  /* =========================================================================== */

  /// default constructor
  INLINE DAGCycleDetector::ArcDel::ArcDel(NodeId tail, NodeId head) noexcept
      : DAGCycleDetector::Change(DAGCycleDetector::ChangeType::ARC_DELETION, tail,
                                 head) {
    GUM_CONSTRUCTOR(DAGCycleDetector::ArcDel);
  }

  /// copy constructor
  INLINE
  DAGCycleDetector::ArcDel::ArcDel(const DAGCycleDetector::ArcDel &from) noexcept
      : DAGCycleDetector::Change(from.type(), from.tail(), from.head()) {
    GUM_CONS_CPY(DAGCycleDetector::ArcDel);
  }

  /// move constructor
  INLINE DAGCycleDetector::ArcDel::ArcDel(DAGCycleDetector::ArcDel &&from) noexcept
      : DAGCycleDetector::Change(std::move(from.type()), std::move(from.tail()),
                                 std::move(from.head())) {
    GUM_CONS_MOV(DAGCycleDetector::ArcDel);
  }

  /// destructor
  INLINE DAGCycleDetector::ArcDel::~ArcDel() noexcept {
    GUM_DESTRUCTOR(DAGCycleDetector::ArcDel);
  }

  /// copy operator
  INLINE DAGCycleDetector::ArcDel &DAGCycleDetector::ArcDel::
  operator=(const DAGCycleDetector::ArcDel &from) noexcept {
    DAGCycleDetector::Change::operator=(from);
    return *this;
  }

  /// move operator
  INLINE DAGCycleDetector::ArcDel &DAGCycleDetector::ArcDel::
  operator=(DAGCycleDetector::ArcDel &&from) noexcept {
    DAGCycleDetector::Change::operator=(std::move(from));
    return *this;
  }

  /* =========================================================================== */
  // ArcReverse
  /* =========================================================================== */

  /// default constructor
  INLINE DAGCycleDetector::ArcReverse::ArcReverse(NodeId tail, NodeId head) noexcept
      : DAGCycleDetector::Change(DAGCycleDetector::ChangeType::ARC_REVERSAL, tail,
                                 head) {
    GUM_CONSTRUCTOR(DAGCycleDetector::ArcReverse);
  }

  /// copy constructor
  INLINE DAGCycleDetector::ArcReverse::ArcReverse(
      const DAGCycleDetector::ArcReverse &from) noexcept
      : DAGCycleDetector::Change(from.type(), from.tail(), from.head()) {
    GUM_CONS_CPY(DAGCycleDetector::ArcReverse);
  }

  /// move constructor
  INLINE DAGCycleDetector::ArcReverse::ArcReverse(
      DAGCycleDetector::ArcReverse &&from) noexcept
      : DAGCycleDetector::Change(std::move(from.type()), std::move(from.tail()),
                                 std::move(from.head())) {
    GUM_CONS_MOV(DAGCycleDetector::ArcReverse);
  }

  /// destructor
  INLINE DAGCycleDetector::ArcReverse::~ArcReverse() noexcept {
    GUM_DESTRUCTOR(DAGCycleDetector::ArcReverse);
  }

  /// copy operator
  INLINE DAGCycleDetector::ArcReverse &DAGCycleDetector::ArcReverse::
  operator=(const DAGCycleDetector::ArcReverse &from) noexcept {
    DAGCycleDetector::Change::operator=(from);
    return *this;
  }

  /// move operator
  INLINE DAGCycleDetector::ArcReverse &DAGCycleDetector::ArcReverse::
  operator=(DAGCycleDetector::ArcReverse &&from) noexcept {
    DAGCycleDetector::Change::operator=(std::move(from));
    return *this;
  }

  /* =========================================================================== */
  // DAGCycleDetector
  /* =========================================================================== */

  /// default constructor
  INLINE DAGCycleDetector::DAGCycleDetector() noexcept {
    GUM_CONSTRUCTOR(DAGCycleDetector);
  }

  /// copy constructor
  INLINE DAGCycleDetector::DAGCycleDetector(const DAGCycleDetector &from)
      : __dag(from.__dag), __ancestors(from.__ancestors),
        __descendants(from.__descendants) {
    GUM_CONS_CPY(DAGCycleDetector);
  }

  /// move constructor
  INLINE DAGCycleDetector::DAGCycleDetector(DAGCycleDetector &&from)
      : __dag(std::move(from.__dag)), __ancestors(std::move(from.__ancestors)),
        __descendants(std::move(from.__descendants)) {
    GUM_CONS_MOV(DAGCycleDetector);
  }

  /// destructor
  INLINE DAGCycleDetector::~DAGCycleDetector() { GUM_DESTRUCTOR(DAGCycleDetector); }

  /// copy operator
  INLINE DAGCycleDetector &DAGCycleDetector::
  operator=(const DAGCycleDetector &from) {
    if (this != &from) {
      __dag = from.__dag;
      __ancestors = from.__ancestors;
      __descendants = from.__descendants;
    }

    return *this;
  }

  /// move operator
  INLINE DAGCycleDetector &DAGCycleDetector::operator=(DAGCycleDetector &&from) {
    if (this != &from) {
      __dag = std::move(from.__dag);
      __ancestors = std::move(from.__ancestors);
      __descendants = std::move(from.__descendants);
    }

    return *this;
  }

  /// indicates whether an arc addition would create a cycle
  INLINE bool DAGCycleDetector::hasCycleFromAddition(NodeId x, NodeId y) const
      noexcept {
    return __descendants[y].exists(x);
  }

  /// indicates wether an arc reversal would create a cycle
  INLINE bool DAGCycleDetector::hasCycleFromReversal(NodeId x, NodeId y) const
      noexcept {
    return (__ancestors[y][x] > 1);
  }

  /// indicates whether an arc deletion would create a cycle
  INLINE bool DAGCycleDetector::hasCycleFromDeletion(NodeId x, NodeId y) const
      noexcept {
    return false;
  }

  /// adds a nodeset to another (nodes are weighted, so weights are added)
  INLINE
  void
  DAGCycleDetector::__addWeightedSet(NodeProperty<unsigned int> &nodeset,
                                     const NodeProperty<unsigned int> &set_to_add,
                                     unsigned int multiplier) const {
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
  void
  DAGCycleDetector::__delWeightedSet(NodeProperty<unsigned int> &nodeset,
                                     const NodeProperty<unsigned int> &set_to_del,
                                     unsigned int multiplier) const {
    for (auto iter = set_to_del.cbegin(); iter != set_to_del.cend(); ++iter) {
      if (nodeset.exists(iter.key())) {
        unsigned int &weight = nodeset[iter.key()];
        weight -= iter.val() * multiplier;

        if (!weight) {
          nodeset.erase(iter.key());
        }
      }
    }
  }

  /** @brief put into a weighted nodeset the nodes of another weighted set that
   * belong to a set of arc extremities */
  INLINE
  void DAGCycleDetector::__restrictWeightedSet(
      NodeProperty<unsigned int> &result_set,
      const NodeProperty<unsigned int> &set_to_restrict,
      const NodeSet &extremities) const {
    for (auto iter = set_to_restrict.cbegin(); iter != set_to_restrict.cend();
         ++iter) {
      if (extremities.exists(iter.key())) {
        result_set.insert(iter.key(), iter.val());
      }
    }
  }

  /// reverses an arc from the DAG
  INLINE void DAGCycleDetector::reverseArc(NodeId tail, NodeId head) {
    if (hasCycleFromReversal(tail, head)) {
      GUM_ERROR(InvalidDirectedCycle, "the arc would create a directed into a DAG");
    }

    eraseArc(tail, head);
    addArc(head, tail);
  }

  /// check the equality between two DAGCycleDetectors
  INLINE bool DAGCycleDetector::operator==(const DAGCycleDetector &from) const {
    return ( //( __dag == from.__dag ) &&
        (__ancestors == from.__ancestors) && (__descendants == from.__descendants));
  }

  /// check the inequality between two DAGCycleDetectors
  INLINE bool DAGCycleDetector::operator!=(const DAGCycleDetector &from) const {
    return !operator==(from);
  }

} /* namespace gum */
