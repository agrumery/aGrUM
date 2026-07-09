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
 * @brief the class imposing a N-sized tabu list as a structural constraints for
 * learning algorithms
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/BN/learning/constraints/structuralConstraintTabuList.h>   // to ease IDE parser
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// sets the size of the tabu list

    /// sets a new graph from which we will perform checking
    INLINE void StructuralConstraintTabuList::setGraphAlone(const DiGraph& graph) {
      // compute the hash of the graph
      _current_graph_ = {0, 0};
      for (const auto& arc: graph.arcs()) {
        const auto hash_arc = _hashArc_(arc.tail(), arc.head());
        _current_graph_.first ^= hash_arc.first;
        _current_graph_.second ^= hash_arc.second;
      }

      // set all the elements of the _graph_tabuList_ as the current graph
      _tabuList_offset_ = 0;
      _graph_tabuList_.clear();
      _graph_tabuList_.insert(_current_graph_, _tabuList_size_);
    }

    /// checks whether the constraints enable to add arc (x,y)
    INLINE bool StructuralConstraintTabuList::checkArcAdditionAlone(NodeId x, NodeId y) const {
      return !_graph_tabuList_.existsFirst(_xorWithCurrentGraph_(_hashArc_(x, y)));
    }

    /// checks whether the constraints enable to remove arc (x,y)
    INLINE bool StructuralConstraintTabuList::checkArcDeletionAlone(NodeId x, NodeId y) const {
      return !_graph_tabuList_.existsFirst(_xorWithCurrentGraph_(_hashArc_(x, y)));
    }

    /// checks whether the constraints enable to reverse arc (x,y)
    INLINE bool StructuralConstraintTabuList::checkArcReversalAlone(NodeId x, NodeId y) const {
      // compute the hash of removing arc x -> y and adding arc y -> x
      const auto hashReversal = _xorHashes_(_hashArc_(x, y), _hashArc_(y, x));
      return !_graph_tabuList_.existsFirst(_xorWithCurrentGraph_(hashReversal));
    }

    /// checks whether the constraints enable to apply an ArcTriangleDeletion1
    INLINE bool StructuralConstraintTabuList::checkArcTriangleDeletion1Alone(NodeId node1,
                                                                             NodeId node2,
                                                                             NodeId node3) const {
      const auto hashReversal12 = _xorHashes_(_hashArc_(node1, node2), _hashArc_(node2, node1));
      const auto hashReversal13 = _xorHashes_(_hashArc_(node1, node3), _hashArc_(node3, node1));
      const auto hashDeletion23 = _hashArc_(node2, node3);
      const auto hashTriangle
          = _xorHashes_(_xorHashes_(hashReversal12, hashReversal13), hashDeletion23);
      return !_graph_tabuList_.existsFirst(_xorWithCurrentGraph_(hashTriangle));
    }

    /// checks whether the constraints enable to apply an ArcTriangleDeletion2
    INLINE bool StructuralConstraintTabuList::checkArcTriangleDeletion2Alone(NodeId node1,
                                                                             NodeId node2,
                                                                             NodeId node3) const {
      const auto hashReversal23 = _xorHashes_(_hashArc_(node2, node3), _hashArc_(node3, node2));
      const auto hashDeletion13 = _hashArc_(node1, node3);
      const auto hashTriangle   = _xorHashes_(hashReversal23, hashDeletion13);
      return !_graph_tabuList_.existsFirst(_xorWithCurrentGraph_(hashTriangle));
    }

    /// checks whether the constraints enable to add an arc
    INLINE bool
        StructuralConstraintTabuList::checkModificationAlone(const ArcAddition& change) const {
      return checkArcAdditionAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to remove an arc
    INLINE bool
        StructuralConstraintTabuList::checkModificationAlone(const ArcDeletion& change) const {
      return checkArcDeletionAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to reverse an arc
    INLINE bool
        StructuralConstraintTabuList::checkModificationAlone(const ArcReversal& change) const {
      return checkArcReversalAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to apply an ArcTriangleDeletion1
    INLINE bool StructuralConstraintTabuList::checkModificationAlone(
        const ArcTriangleDeletion1& change) const {
      return checkArcTriangleDeletion1Alone(change.node1(), change.node2(), change.node3());
    }

    /// checks whether the constraints enable to apply an ArcTriangleDeletion2
    INLINE bool StructuralConstraintTabuList::checkModificationAlone(
        const ArcTriangleDeletion2& change) const {
      return checkArcTriangleDeletion2Alone(change.node1(), change.node2(), change.node3());
    }

    /// checks whether the constraints enable to perform a graph change

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintTabuList::modifyGraphAlone(const ArcAddition& change) {
      _graph_tabuList_.eraseSecond(_tabuList_offset_);
      ++_tabuList_offset_;
      _current_graph_ = _xorWithCurrentGraph_(_hashArc_(change.node1(), change.node2()));

      // The difference between the offset of the inserted graph hash and uList_offset_
      // must always be equal to _tabuList_size_ in order to guarantee that the tabu list
      // size is at most equal to _tabuList_size_
      _graph_tabuList_.insert(_current_graph_, _tabuList_offset_ + _tabuList_size_);
    }

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintTabuList::modifyGraphAlone(const ArcDeletion& change) {
      _graph_tabuList_.eraseSecond(_tabuList_offset_);
      ++_tabuList_offset_;
      _current_graph_ = _xorWithCurrentGraph_(_hashArc_(change.node1(), change.node2()));

      // The difference between the offset of the inserted graph hash and uList_offset_
      // must always be equal to _tabuList_size_ in order to guarantee that the tabu list
      // size is at most equal to _tabuList_size_
      _graph_tabuList_.insert(_current_graph_, _tabuList_offset_ + _tabuList_size_);
    }

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintTabuList::modifyGraphAlone(const ArcReversal& change) {
      _graph_tabuList_.eraseSecond(_tabuList_offset_);
      ++_tabuList_offset_;
      const auto hashReversal = _xorHashes_(_hashArc_(change.node1(), change.node2()),
                                            _hashArc_(change.node2(), change.node1()));
      _current_graph_         = _xorWithCurrentGraph_(hashReversal);

      // The difference between the offset of the inserted graph hash and uList_offset_
      // must always be equal to _tabuList_size_ in order to guarantee that the tabu list
      // size is at most equal to _tabuList_size_
      _graph_tabuList_.insert(_current_graph_, _tabuList_offset_ + _tabuList_size_);
    }

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintTabuList::modifyGraphAlone(const ArcTriangleDeletion1& change) {
      _graph_tabuList_.eraseSecond(_tabuList_offset_);
      ++_tabuList_offset_;
      const auto hashReversal12 = _xorHashes_(_hashArc_(change.node1(), change.node2()),
                                              _hashArc_(change.node2(), change.node1()));
      const auto hashReversal13 = _xorHashes_(_hashArc_(change.node1(), change.node3()),
                                              _hashArc_(change.node3(), change.node1()));
      const auto hashDeletion23 = _hashArc_(change.node2(), change.node3());
      const auto hashTriangle
          = _xorHashes_(_xorHashes_(hashReversal12, hashReversal13), hashDeletion23);
      _current_graph_ = _xorWithCurrentGraph_(hashTriangle);

      // The difference between the offset of the inserted graph hash and uList_offset_
      // must always be equal to _tabuList_size_ in order to guarantee that the tabu list
      // size is at most equal to _tabuList_size_
      _graph_tabuList_.insert(_current_graph_, _tabuList_offset_ + _tabuList_size_);
    }

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintTabuList::modifyGraphAlone(const ArcTriangleDeletion2& change) {
      _graph_tabuList_.eraseSecond(_tabuList_offset_);
      ++_tabuList_offset_;
      const auto hashReversal23 = _xorHashes_(_hashArc_(change.node2(), change.node3()),
                                              _hashArc_(change.node3(), change.node2()));
      const auto hashDeletion13 = _hashArc_(change.node1(), change.node3());
      const auto hashTriangle   = _xorHashes_(hashReversal23, hashDeletion13);
      _current_graph_           = _xorWithCurrentGraph_(hashTriangle);

      // The difference between the offset of the inserted graph hash and uList_offset_
      // must always be equal to _tabuList_size_ in order to guarantee that the tabu list
      // size is at most equal to _tabuList_size_
      _graph_tabuList_.insert(_current_graph_, _tabuList_offset_ + _tabuList_size_);
    }

    /// notify the constraint of a modification of the graph

    /// indicates whether a change will always violate the constraint
    INLINE bool StructuralConstraintTabuList::isAlwaysInvalidAlone(const GraphChange&) const {
      return false;
    }

// include all the methods applicable to the whole class hierarchy
#  define GUM_CONSTRAINT_CLASS_NAME StructuralConstraintTabuList
#  include <agrum/BN/learning/constraints/structuralConstraintPatternRootInline.h>
#  undef GUM_CONSTRAINT_CLASS_NAME

    INLINE StructuralConstraintTabuList::GraphHash
           StructuralConstraintTabuList::_hashArc_(NodeId node1, NodeId node2) const {
      return {HashFuncConst::gold * node1 + HashFuncConst::sqrt3 * node2,
              HashFuncConst::pi * node2 + HashFuncConst::sqrt3 * node1};
    }

    INLINE StructuralConstraintTabuList::GraphHash
           StructuralConstraintTabuList::_xorHashes_(const GraphHash& hash1,
                                                     const GraphHash& hash2) const {
      return {hash1.first ^ hash2.first, hash1.second ^ hash2.second};
    }

    INLINE StructuralConstraintTabuList::GraphHash
           StructuralConstraintTabuList::_xorWithCurrentGraph_(const GraphHash& hash) const {
      return _xorHashes_(_current_graph_, hash);
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
