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
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// sets the size of the tabu list
    INLINE
    void StructuralConstraintTabuList::setTabuListSize(Size new_size) {
      if (new_size == _tabuList_size_) return;

      if (_graph_tabuList_.size() > new_size) {
        // here, there are more graphs than allowed by the new size.
        // So remove the oldest elements, so that only new_size elements remain
        while (_graph_tabuList_.size() > new_size) {
          _graph_tabuList_.eraseSecond(_tabuList_offset_);
          ++_tabuList_offset_;
        }
      } else {
        // here, we should keep all the elements in _graph_tabuList_ but we must
        // ensure that the difference between the max offset in _graph_tabuList_ and
        // _tabuList_offset_ is equal to new_size
        if (_tabuList_offset_ + _tabuList_size_ >= new_size) {
          // we can modify _tabuList_offset_ so that the aforementioned difference
          // is now equal to new_size
          _tabuList_offset_ = (_tabuList_offset_ + _tabuList_size_) - new_size;
        } else {
          // basically, we should add _tabuList_size_ - new_size to _tabuList_offset_,
          // except that the value of _tabuList_offset_ would be strictly negative,
          // which is impossible for an attribute of type Size. Hence, here, instead
          // of changing _tabuList_offset_, we will add (new_size - _tabuList_size_)
          // to all the offsets in the tabu list
          const Size delta = new_size - _tabuList_size_;
          Bijection< GraphHash, Size > new_tabuList(2 * new_size);
          for (auto iter = _graph_tabuList_.begin(); iter != _graph_tabuList_.end(); ++iter) {
            new_tabuList.emplace(iter.first(), iter.second() + delta);
          }
          _graph_tabuList_ = std::move(new_tabuList);
        }
      }

      _tabuList_size_ = new_size;
    }

    /// sets a new graph from which we will perform checking
    INLINE void StructuralConstraintTabuList::setGraphAlone(const DiGraph& graph) {
      // compute the hash of the graph
      _current_graph_ = {0, 0};
      for (const auto& arc : graph.arcs()) {
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
      const auto hashTriangle   = _xorHashes_(_xorHashes_(hashReversal12, hashReversal13),
                                            hashDeletion23);
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
    INLINE bool
        StructuralConstraintTabuList::checkModificationAlone(const GraphChange& change) const {
      switch (change.type()) {
        case GraphChangeType::ARC_ADDITION :
          return checkArcAdditionAlone(change.node1(), change.node2());

        case GraphChangeType::ARC_DELETION :
          return checkArcDeletionAlone(change.node1(), change.node2());

        case GraphChangeType::ARC_REVERSAL :
          return checkArcReversalAlone(change.node1(), change.node2());

        case GraphChangeType::ARC_TRIANGLE_DELETION1 :
          return checkArcTriangleDeletion1Alone(change.node1(), change.node2(), change.node3());

        case GraphChangeType::ARC_TRIANGLE_DELETION2 :
          return checkArcTriangleDeletion2Alone(change.node1(), change.node2(), change.node3());

        default :
          GUM_ERROR(OperationNotAllowed,
                    "Graph change operation "
                        << change.typeAsString()
                        << "is not supported by the Tabu List structural constraint");
      }
    }

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
      _current_graph_ = _xorWithCurrentGraph_(hashReversal);

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
      const auto hashTriangle   = _xorHashes_(_xorHashes_(hashReversal12, hashReversal13),
                                            hashDeletion23);
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
      _current_graph_ = _xorWithCurrentGraph_(hashTriangle);

      // The difference between the offset of the inserted graph hash and uList_offset_
      // must always be equal to _tabuList_size_ in order to guarantee that the tabu list
      // size is at most equal to _tabuList_size_
      _graph_tabuList_.insert(_current_graph_, _tabuList_offset_ + _tabuList_size_);
    }

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintTabuList::modifyGraphAlone(const GraphChange& change) {
      switch (change.type()) {
        case GraphChangeType::ARC_ADDITION :
          modifyGraphAlone(static_cast< const ArcAddition& >(change));
          break;

        case GraphChangeType::ARC_DELETION :
          modifyGraphAlone(static_cast< const ArcDeletion& >(change));
          break;

        case GraphChangeType::ARC_REVERSAL :
          modifyGraphAlone(static_cast< const ArcReversal& >(change));
          break;

        case GraphChangeType::ARC_TRIANGLE_DELETION1 :
          modifyGraphAlone(static_cast< const ArcTriangleDeletion1& >(change));
          break;

        case GraphChangeType::ARC_TRIANGLE_DELETION2 :
          modifyGraphAlone(static_cast< const ArcTriangleDeletion2& >(change));
          break;

        default :
          GUM_ERROR(OperationNotAllowed,
                    "Graph change operation " << change.typeAsString()
                                              << " is not supported by Tabu List structural constraint")
      }
    }

    /// indicates whether a change will always violate the constraint
    INLINE bool StructuralConstraintTabuList::isAlwaysInvalidAlone(const GraphChange&) const {
      return false;
    }

// include all the methods applicable to the whole class hierarchy
#  define GUM_CONSTRAINT_CLASS_NAME StructuralConstraintTabuList
#  include <agrum/BN/learning/constraints/structuralConstraintPatternRootInline.h>
#  undef GUM_CONSTRAINT_CLASS_NAME

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
