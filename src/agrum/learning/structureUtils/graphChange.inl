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
 * @brief A class to account for changes in a graph
 *
 * This class shall be used by learning algorithms to notify scores, structural
 * constraints, etc, that the learnt graph has been modified.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    INLINE GraphChange::GraphChange(GraphChangeType type, NodeId node1,
                                    NodeId node2) noexcept : __type{type},
                                                             __node1{node1},
                                                             __node2{node2} {
      GUM_CONSTRUCTOR(GraphChange);
    }

    /// copy constructor
    INLINE GraphChange::GraphChange(const GraphChange &from) noexcept
        : __type{from.__type},
          __node1{from.__node1},
          __node2{from.__node2} {
      GUM_CONS_CPY(GraphChange);
    }

    /// move constructor
    INLINE GraphChange::GraphChange(GraphChange &&from) noexcept
        : __type{from.__type},
          __node1{from.__node1},
          __node2{from.__node2} {
      GUM_CONS_MOV(GraphChange);
    }

    /// destructor
    INLINE GraphChange::~GraphChange() noexcept { GUM_DESTRUCTOR(GraphChange); }

    /// copy constructor
    INLINE GraphChange &GraphChange::operator=(const GraphChange &from) noexcept {
      __type = from.__type;
      __node1 = from.__node1;
      __node2 = from.__node2;
      return *this;
    }

    /// move operator
    INLINE GraphChange &GraphChange::operator=(GraphChange &&from) noexcept {
      __type = from.__type;
      __node1 = from.__node1;
      __node2 = from.__node2;
      return *this;
    }

    /// returns the type of the operation
    INLINE GraphChangeType GraphChange::type() const noexcept { return __type; }

    /// returns the first node involved in the modification
    INLINE NodeId GraphChange::node1() const noexcept { return __node1; }

    /// returns the second node involved in the modification
    INLINE NodeId GraphChange::node2() const noexcept { return __node2; }

    /// returns whether two graph changes are identical or not
    INLINE bool GraphChange::operator==(const GraphChange &from) const noexcept {
      return ((__node1 == from.__node1) && (__node2 == from.__node2) &&
              (__type == from.__type));
    }

    /// returns whether two graph changes are different or not
    INLINE bool GraphChange::operator!=(const GraphChange &from) const noexcept {
      return !operator==(from);
    }

    // ===========================================================================

    /// default constructor
    INLINE ArcAddition::ArcAddition(NodeId node1, NodeId node2) noexcept
        : GraphChange(GraphChangeType::ARC_ADDITION, node1, node2) {
      // do not use GUM_CONSTRUCTOR here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// copy constructor
    INLINE ArcAddition::ArcAddition(const ArcAddition &from) noexcept
        : GraphChange(from) {
      // do not use GUM_CONS_CPY here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// move constructor
    INLINE ArcAddition::ArcAddition(ArcAddition &&from) noexcept
        : GraphChange(std::move(from)) {
      // do not use GUM_CONS_MOV here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// destructor
    INLINE ArcAddition::~ArcAddition() noexcept {
      // do not use GUM_DESTRUCTOR here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// copy constructor
    INLINE ArcAddition &ArcAddition::operator=(const ArcAddition &from) noexcept {
      GraphChange::operator=(from);
      return *this;
    }

    /// move operator
    INLINE ArcAddition &ArcAddition::operator=(ArcAddition &&from) noexcept {
      GraphChange::operator=(std::move(from));
      return *this;
    }

    /// returns whether two graph changes are identical or not
    INLINE bool ArcAddition::operator==(const ArcAddition &from) const noexcept {
      return ((node1() == from.node1()) && (node2() == from.node2()));
    }

    /// returns whether two graph changes are different or not
    INLINE bool ArcAddition::operator!=(const ArcAddition &from) const noexcept {
      return !operator==(from);
    }

    // ===========================================================================

    /// default constructor
    INLINE ArcDeletion::ArcDeletion(NodeId node1, NodeId node2) noexcept
        : GraphChange(GraphChangeType::ARC_DELETION, node1, node2) {
      // do not use GUM_CONSTRUCTOR here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// copy constructor
    INLINE ArcDeletion::ArcDeletion(const ArcDeletion &from) noexcept
        : GraphChange(from) {
      // do not use GUM_CONS_CPY here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// move constructor
    INLINE ArcDeletion::ArcDeletion(ArcDeletion &&from) noexcept
        : GraphChange(std::move(from)) {
      // do not use GUM_CONS_MOV here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// destructor
    INLINE ArcDeletion::~ArcDeletion() noexcept {
      // do not use GUM_DESTRUCTOR here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// copy constructor
    INLINE ArcDeletion &ArcDeletion::operator=(const ArcDeletion &from) noexcept {
      GraphChange::operator=(from);
      return *this;
    }

    /// move operator
    INLINE ArcDeletion &ArcDeletion::operator=(ArcDeletion &&from) noexcept {
      GraphChange::operator=(std::move(from));
      return *this;
    }

    /// returns whether two graph changes are identical or not
    INLINE bool ArcDeletion::operator==(const ArcDeletion &from) const noexcept {
      return ((node1() == from.node1()) && (node2() == from.node2()));
    }

    /// returns whether two graph changes are different or not
    INLINE bool ArcDeletion::operator!=(const ArcDeletion &from) const noexcept {
      return !operator==(from);
    }

    // ===========================================================================

    /// default constructor
    INLINE ArcReversal::ArcReversal(NodeId node1, NodeId node2) noexcept
        : GraphChange(GraphChangeType::ARC_REVERSAL, node1, node2) {
      // do not use GUM_CONSTRUCTOR here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// copy constructor
    INLINE ArcReversal::ArcReversal(const ArcReversal &from) noexcept
        : GraphChange(from) {
      // do not use GUM_CONS_CPY here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// move constructor
    INLINE ArcReversal::ArcReversal(ArcReversal &&from) noexcept
        : GraphChange(std::move(from)) {
      // do not use GUM_CONS_MOV here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// destructor
    INLINE ArcReversal::~ArcReversal() noexcept {
      // do not use GUM_DESTRUCTOR here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// copy constructor
    INLINE ArcReversal &ArcReversal::operator=(const ArcReversal &from) noexcept {
      GraphChange::operator=(from);
      return *this;
    }

    /// move operator
    INLINE ArcReversal &ArcReversal::operator=(ArcReversal &&from) noexcept {
      GraphChange::operator=(std::move(from));
      return *this;
    }

    /// returns whether two arc reversals are identical or not
    INLINE bool ArcReversal::operator==(const ArcReversal &from) const noexcept {
      return ((node1() == from.node1()) && (node2() == from.node2()));
    }

    /// returns whether two arc reversals are different or not
    INLINE bool ArcReversal::operator!=(const ArcReversal &from) const noexcept {
      return !operator==(from);
    }

    // ===========================================================================

    /// default constructor
    INLINE EdgeAddition::EdgeAddition(NodeId node1, NodeId node2) noexcept
        : GraphChange(GraphChangeType::EDGE_ADDITION, node1, node2) {
      // do not use GUM_CONSTRUCTOR here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// copy constructor
    INLINE EdgeAddition::EdgeAddition(const EdgeAddition &from) noexcept
        : GraphChange(from) {
      // do not use GUM_CONS_CPY here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// move constructor
    INLINE EdgeAddition::EdgeAddition(EdgeAddition &&from) noexcept
        : GraphChange(std::move(from)) {
      // do not use GUM_CONS_MOV here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// destructor
    INLINE EdgeAddition::~EdgeAddition() noexcept {
      // do not use GUM_DESTRUCTOR here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// copy constructor
    INLINE EdgeAddition &EdgeAddition::operator=(const EdgeAddition &from) noexcept {
      GraphChange::operator=(from);
      return *this;
    }

    /// move operator
    INLINE EdgeAddition &EdgeAddition::operator=(EdgeAddition &&from) noexcept {
      GraphChange::operator=(std::move(from));
      return *this;
    }

    /// returns whether two graph changes are identical or not
    INLINE bool EdgeAddition::operator==(const EdgeAddition &from) const noexcept {
      return (((node1() == from.node1()) && (node2() == from.node2())) ||
              ((node1() == from.node2()) && (node2() == from.node1())));
    }

    /// returns whether two graph changes are different or not
    INLINE bool EdgeAddition::operator!=(const EdgeAddition &from) const noexcept {
      return !operator==(from);
    }

    // ===========================================================================

    /// default constructor
    INLINE EdgeDeletion::EdgeDeletion(NodeId node1, NodeId node2) noexcept
        : GraphChange(GraphChangeType::EDGE_DELETION, node1, node2) {
      // do not use GUM_CONSTRUCTOR here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// copy constructor
    INLINE EdgeDeletion::EdgeDeletion(const EdgeDeletion &from) noexcept
        : GraphChange(from) {
      // do not use GUM_CONS_CPY here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// move constructor
    INLINE EdgeDeletion::EdgeDeletion(EdgeDeletion &&from) noexcept
        : GraphChange(std::move(from)) {
      // do not use GUM_CONS_MOV here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// destructor
    INLINE EdgeDeletion::~EdgeDeletion() noexcept {
      // do not use GUM_DESTRUCTOR here because, to speed up GraphChange's
      // destructor, we did not make the latter's destructor virtual.
    }

    /// copy constructor
    INLINE EdgeDeletion &EdgeDeletion::operator=(const EdgeDeletion &from) noexcept {
      GraphChange::operator=(from);
      return *this;
    }

    /// move operator
    INLINE EdgeDeletion &EdgeDeletion::operator=(EdgeDeletion &&from) noexcept {
      GraphChange::operator=(std::move(from));
      return *this;
    }

    /// returns whether two graph changes are identical or not
    INLINE bool EdgeDeletion::operator==(const EdgeDeletion &from) const noexcept {
      return (((node1() == from.node1()) && (node2() == from.node2())) ||
              ((node1() == from.node2()) && (node2() == from.node1())));
    }

    /// returns whether two graph changes are different or not
    INLINE bool EdgeDeletion::operator!=(const EdgeDeletion &from) const noexcept {
      return !operator==(from);
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
