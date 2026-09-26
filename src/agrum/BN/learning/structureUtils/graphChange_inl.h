/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


/** @file
 * @brief A class to account for changes in a graph
 *
 * This class shall be used by learning algorithms to notify scores, structural
 * constraints, etc, that the learnt graph has been modified.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/BN/learning/structureUtils/graphChange.h>   // to ease IDE parser
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    // Constructors, destructor and assignment operators are defined
    // out-of-line in graphChange.cpp on purpose -- see the comment there.

    /// returns the type of the operation
    INLINE GraphChangeType GraphChange::type() const noexcept { return type_; }

    /// returns the first node involved in the modification
    INLINE NodeId GraphChange::node1() const noexcept { return NodeId(nodes_[0]); }

    /// returns the second node involved in the modification
    INLINE NodeId GraphChange::node2() const noexcept { return NodeId(nodes_[1]); }

    /// returns the third node involved in the modification
    INLINE NodeId GraphChange::node3() const {
      if ((type_ != GraphChangeType::ARC_TRIANGLE_DELETION1)
          && (type_ != GraphChangeType::ARC_TRIANGLE_DELETION2)) {
        GUM_ERROR(InvalidNode, "GraphChange " << (int)(type_) << " does not involve a third node")
      }
      return NodeId(nodes_[2]);
    }

    /// returns whether two graph changes are identical or not
    INLINE bool GraphChange::operator==(const GraphChange& from) const noexcept {
      // Here, we know that nodes_ and type_ are of the same type, which
      // is 32bit long (and aligned accordingly). In addition, type_ is
      // defined just after nodes_ in Class GraphChange. Hence, memcmping 4
      // elements starting from pointer nodes_ will compare efficiently both
      // the 3 elements of nodes_ and type_
      return std::memcmp(nodes_, from.nodes_, 4 * sizeof(LearnNodeId)) == 0;
    }

    /// returns whether two graph changes are different or not
    INLINE bool GraphChange::operator!=(const GraphChange& from) const noexcept {
      return !operator==(from);
    }

    // ===========================================================================

    // Constructors, destructor and assignment operators are defined
    // out-of-line in graphChange.cpp on purpose -- see the comment there.

    /// returns whether two graph changes are identical or not
    INLINE bool ArcAddition::operator==(const ArcAddition& from) const noexcept {
      // compare nodes_[0] and nodes_[1] in this and from
      return std::memcmp(nodes_, from.nodes_, 2 * sizeof(LearnNodeId)) == 0;
    }

    /// returns whether two graph changes are different or not
    INLINE bool ArcAddition::operator!=(const ArcAddition& from) const noexcept {
      return !operator==(from);
    }

    // ===========================================================================

    // Constructors, destructor and assignment operators are defined
    // out-of-line in graphChange.cpp on purpose -- see the comment there.

    /// returns whether two graph changes are identical or not
    INLINE bool ArcDeletion::operator==(const ArcDeletion& from) const noexcept {
      // compare nodes_[0] and nodes_[1] in this and from
      return std::memcmp(nodes_, from.nodes_, 2 * sizeof(LearnNodeId)) == 0;
    }

    /// returns whether two graph changes are different or not
    INLINE bool ArcDeletion::operator!=(const ArcDeletion& from) const noexcept {
      return !operator==(from);
    }

    // ===========================================================================

    // Constructors, destructor and assignment operators are defined
    // out-of-line in graphChange.cpp on purpose -- see the comment there.

    /// returns whether two arc reversals are identical or not
    INLINE bool ArcReversal::operator==(const ArcReversal& from) const noexcept {
      // compare nodes_[0] and nodes_[1] in this and from
      return std::memcmp(nodes_, from.nodes_, 2 * sizeof(LearnNodeId)) == 0;
    }

    /// returns whether two arc reversals are different or not
    INLINE bool ArcReversal::operator!=(const ArcReversal& from) const noexcept {
      return !operator==(from);
    }

    // ===========================================================================

    // Constructors, destructor and assignment operators are defined
    // out-of-line in graphChange.cpp on purpose -- see the comment there.

    /// returns whether two arc reversals are identical or not
    INLINE bool ArcTriangleDeletion1::operator==(const ArcTriangleDeletion1& from) const noexcept {
      // compare nodes_[0], nodes_[1] and nodes_[2] in this and from
      return std::memcmp(nodes_, from.nodes_, 3 * sizeof(LearnNodeId)) == 0;
    }

    /// returns whether two arc reversals are different or not
    INLINE bool ArcTriangleDeletion1::operator!=(const ArcTriangleDeletion1& from) const noexcept {
      return !operator==(from);
    }

    /// returns the third node involved in the modification
    INLINE NodeId ArcTriangleDeletion1::node3() const { return NodeId(nodes_[2]); }

    // ===========================================================================

    // Constructors, destructor and assignment operators are defined
    // out-of-line in graphChange.cpp on purpose -- see the comment there.

    /// returns whether two arc reversals are identical or not
    INLINE bool ArcTriangleDeletion2::operator==(const ArcTriangleDeletion2& from) const noexcept {
      // compare nodes_[0], nodes_[1] and nodes_[2] in this and from
      return std::memcmp(nodes_, from.nodes_, 3 * sizeof(LearnNodeId)) == 0;
    }

    /// returns whether two arc reversals are different or not
    INLINE bool ArcTriangleDeletion2::operator!=(const ArcTriangleDeletion2& from) const noexcept {
      return !operator==(from);
    }

    /// returns the third node involved in the modification
    INLINE NodeId ArcTriangleDeletion2::node3() const { return NodeId(nodes_[2]); }

    // ===========================================================================

    // Constructors, destructor and assignment operators are defined
    // out-of-line in graphChange.cpp on purpose -- see the comment there.

    /// returns whether two graph changes are identical or not
    INLINE bool EdgeAddition::operator==(const EdgeAddition& from) const noexcept {
      // compare nodes_[0] and nodes_[1] in this and from
      return std::memcmp(nodes_, from.nodes_, 2 * sizeof(LearnNodeId)) == 0;
    }

    /// returns whether two graph changes are different or not
    INLINE bool EdgeAddition::operator!=(const EdgeAddition& from) const noexcept {
      return !operator==(from);
    }

    // ===========================================================================

    // Constructors, destructor and assignment operators are defined
    // out-of-line in graphChange.cpp on purpose -- see the comment there.

    /// returns whether two graph changes are identical or not
    INLINE bool EdgeDeletion::operator==(const EdgeDeletion& from) const noexcept {
      // compare nodes_[0] and nodes_[1] in this and from
      return std::memcmp(nodes_, from.nodes_, 2 * sizeof(LearnNodeId)) == 0;
    }

    /// returns whether two graph changes are different or not
    INLINE bool EdgeDeletion::operator!=(const EdgeDeletion& from) const noexcept {
      return !operator==(from);
    }


  } /* namespace learning */

  // ===========================================================================

  // Returns the value of a key as a Size.
  INLINE Size HashFunc< learning::GraphChange >::castToSize(const learning::GraphChange& key) {
    if constexpr (sizeof(learning::LearnNodeId) == sizeof(Size)) {
      // here, we assume that it is very unlikely that many triangles share the same
      // arc. Hence, to avoid slowing down the computations of the mapping of
      // ArcAddition, ArcDeletion and ArcReversal while not speeding-up much that
      // of ArcTriangleDeletion1 and ArcTriangleDeletion2, we never take into account
      // node3 in our computations.
      return Size(key.type()) * HashFuncConst::gold + Size(key.node1()) * HashFuncConst::pi
           + Size(key.node2()) * HashFuncConst::sqrt3;
    } else {
      // here we not only take into account the 3 nodes but also the type of
      // the change
      const Size* const nodes = (const Size*)key.nodes_;
      return nodes[0] * HashFuncConst::gold + nodes[1] * HashFuncConst::pi;
    }
  }

  // computes the hashed value of a key
  INLINE Size
      HashFunc< learning::GraphChange >::operator()(const learning::GraphChange& key) const {
    return castToSize(key) >> this->right_shift_;
  }

  // Returns the value of a key as a Size.
  INLINE Size HashFunc< learning::ArcAddition >::castToSize(const learning::ArcAddition& key) {
    if constexpr (sizeof(learning::LearnNodeId) == sizeof(Size)) {
      return Size(key.node1()) * HashFuncConst::gold + Size(key.node2()) * HashFuncConst::pi;
    } else {
      // here we take into account both node1() and node2()
      const Size* const nodes = (Size*)key.nodes_;
      return nodes[0] * HashFuncConst::gold;
    }
  }

  // computes the hashed value of a key
  INLINE Size
      HashFunc< learning::ArcAddition >::operator()(const learning::ArcAddition& key) const {
    return castToSize(key) >> this->right_shift_;
  }

  // Returns the value of a key as a Size.
  INLINE Size HashFunc< learning::ArcDeletion >::castToSize(const learning::ArcDeletion& key) {
    if constexpr (sizeof(learning::LearnNodeId) == sizeof(Size)) {
      return Size(key.node1()) * HashFuncConst::gold + Size(key.node2()) * HashFuncConst::pi;
    } else {
      // here we take into account both node1() and node2()
      const Size* const nodes = (Size*)key.nodes_;
      return nodes[0] * HashFuncConst::gold;
    }
  }

  // computes the hashed value of a key
  INLINE Size
      HashFunc< learning::ArcDeletion >::operator()(const learning::ArcDeletion& key) const {
    return castToSize(key) >> this->right_shift_;
  }

  // Returns the value of a key as a Size.
  INLINE Size HashFunc< learning::ArcReversal >::castToSize(const learning::ArcReversal& key) {
    if constexpr (sizeof(learning::LearnNodeId) == sizeof(Size)) {
      return Size(key.node1()) * HashFuncConst::gold + Size(key.node2()) * HashFuncConst::pi;
    } else {
      // here we take into account both node1() and node2()
      const Size* const nodes = (Size*)key.nodes_;
      return nodes[0] * HashFuncConst::gold;
    }
  }

  // computes the hashed value of a key
  INLINE Size
      HashFunc< learning::ArcReversal >::operator()(const learning::ArcReversal& key) const {
    return castToSize(key) >> this->right_shift_;
  }

  // Returns the value of a key as a Size.
  INLINE Size HashFunc< learning::ArcTriangleDeletion1 >::castToSize(
      const learning::ArcTriangleDeletion1& key) {
    if constexpr (sizeof(learning::LearnNodeId) == sizeof(Size)) {
      return Size(key.node1()) * HashFuncConst::gold + Size(key.node2()) * HashFuncConst::pi
           + Size(key.node3()) * HashFuncConst::sqrt3;
    } else {
      // here we not only take into account the 3 nodes but also the type of
      // the change
      const Size* const nodes = (const Size*)key.nodes_;
      return nodes[0] * HashFuncConst::gold + nodes[1] * HashFuncConst::pi;
    }
  }

  // computes the hashed value of a key
  INLINE Size HashFunc< learning::ArcTriangleDeletion1 >::operator()(
      const learning::ArcTriangleDeletion1& key) const {
    return castToSize(key) >> this->right_shift_;
  }

  // Returns the value of a key as a Size.
  INLINE Size HashFunc< learning::ArcTriangleDeletion2 >::castToSize(
      const learning::ArcTriangleDeletion2& key) {
    if constexpr (sizeof(learning::LearnNodeId) == sizeof(Size)) {
      return Size(key.node1()) * HashFuncConst::gold + Size(key.node2()) * HashFuncConst::pi
           + Size(key.node3()) * HashFuncConst::sqrt3;
    } else {
      // here we not only take into account the 3 nodes but also the type of
      // the change
      const Size* const nodes = (const Size*)key.nodes_;
      return nodes[0] * HashFuncConst::gold + nodes[1] * HashFuncConst::pi;
    }
  }

  // computes the hashed value of a key
  INLINE Size HashFunc< learning::ArcTriangleDeletion2 >::operator()(
      const learning::ArcTriangleDeletion2& key) const {
    return castToSize(key) >> this->right_shift_;
  }

  // Returns the value of a key as a Size.
  INLINE Size HashFunc< learning::EdgeAddition >::castToSize(const learning::EdgeAddition& key) {
    if constexpr (sizeof(learning::LearnNodeId) == sizeof(Size)) {
      return Size(key.node1()) * HashFuncConst::gold + Size(key.node2()) * HashFuncConst::pi;
    } else {
      // here we take into account both node1() and node2()
      const Size* const nodes = (Size*)key.nodes_;
      return nodes[0] * HashFuncConst::gold;
    }
  }

  /// computes the hashed value of a key
  INLINE Size
      HashFunc< learning::EdgeAddition >::operator()(const learning::EdgeAddition& key) const {
    return castToSize(key) >> this->right_shift_;
  }

  // Returns the value of a key as a Size.
  INLINE Size HashFunc< learning::EdgeDeletion >::castToSize(const learning::EdgeDeletion& key) {
    if constexpr (sizeof(learning::LearnNodeId) == sizeof(Size)) {
      return Size(key.node1()) * HashFuncConst::gold + Size(key.node2()) * HashFuncConst::pi;
    } else {
      // here we take into account both node1() and node2()
      const Size* const nodes = (Size*)key.nodes_;
      return nodes[0] * HashFuncConst::gold;
    }
  }

  /// computes the hashed value of a key
  INLINE Size
      HashFunc< learning::EdgeDeletion >::operator()(const learning::EdgeDeletion& key) const {
    return castToSize(key) >> this->right_shift_;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
