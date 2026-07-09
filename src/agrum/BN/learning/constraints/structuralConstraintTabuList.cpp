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


/** @file
 * @brief the class imposing a N-sized tabu list as a structural constraints for
 * learning algorithms
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/BN/learning/constraints/structuralConstraintTabuList.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#  include <agrum/BN/learning/constraints/structuralConstraintTabuList_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    StructuralConstraintTabuList::StructuralConstraintTabuList() :
        _graph_tabuList_(2 * GUM_STRUCTURAL_CONSTRAINT_TABU_LIST_DEFAULT_SIZE),
        _tabuList_size_(GUM_STRUCTURAL_CONSTRAINT_TABU_LIST_DEFAULT_SIZE) {
      // insert the empty graph. The difference between the _tabuList_offset_ and
      // the max offset in _graph_tabuList_ should always be equal to _tabuList_size_
      _graph_tabuList_.insert(_current_graph_, _tabuList_size_);

      GUM_CONSTRUCTOR(StructuralConstraintTabuList);
    }

    /// constructor starting with a given graph
    StructuralConstraintTabuList::StructuralConstraintTabuList(const DiGraph& graph) :
        _graph_tabuList_(2 * GUM_STRUCTURAL_CONSTRAINT_TABU_LIST_DEFAULT_SIZE),
        _tabuList_size_(GUM_STRUCTURAL_CONSTRAINT_TABU_LIST_DEFAULT_SIZE) {
      // compute the hash value of the diGraph
      for (const auto& arc: graph.arcs()) {
        const auto hash_arc = _hashArc_(arc.tail(), arc.head());
        _current_graph_.first ^= hash_arc.first;
        _current_graph_.second ^= hash_arc.second;
      }

      // insert the graph hash. The difference between the _tabuList_offset_ and
      // the max offset in _graph_tabuList_ should always be equal to _tabuList_size_
      _graph_tabuList_.insert(_current_graph_, _tabuList_size_);

      GUM_CONSTRUCTOR(StructuralConstraintTabuList);
    }

    /// copy constructor
    StructuralConstraintTabuList::StructuralConstraintTabuList(
        const StructuralConstraintTabuList& from) :
        _graph_tabuList_(from._graph_tabuList_), _tabuList_offset_(from._tabuList_offset_),
        _tabuList_size_(from._tabuList_size_), _current_graph_(from._current_graph_) {
      GUM_CONS_CPY(StructuralConstraintTabuList);
    }

    /// move constructor
    StructuralConstraintTabuList::StructuralConstraintTabuList(
        StructuralConstraintTabuList&& from) noexcept :
        _graph_tabuList_(std::move(from._graph_tabuList_)),
        _tabuList_offset_(from._tabuList_offset_), _tabuList_size_(from._tabuList_size_),
        _current_graph_(std::move(from._current_graph_)) {
      GUM_CONS_MOV(StructuralConstraintTabuList);
    }

    /// destructor
    StructuralConstraintTabuList::~StructuralConstraintTabuList() {
      GUM_DESTRUCTOR(StructuralConstraintTabuList);
    }

    /// copy operator
    StructuralConstraintTabuList&
        StructuralConstraintTabuList::operator=(const StructuralConstraintTabuList& from) {
      if (this != &from) {
        _graph_tabuList_  = from._graph_tabuList_;
        _tabuList_offset_ = from._tabuList_offset_;
        _tabuList_size_   = from._tabuList_size_;
        _current_graph_   = from._current_graph_;
      }
      return *this;
    }

    /// move operator
    StructuralConstraintTabuList&
        StructuralConstraintTabuList::operator=(StructuralConstraintTabuList&& from) {
      if (this != &from) {
        _graph_tabuList_  = std::move(from._graph_tabuList_);
        _tabuList_offset_ = from._tabuList_offset_;
        _tabuList_size_   = from._tabuList_size_;
        _current_graph_   = from._current_graph_;
      }
      return *this;
    }


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
          const Size                   delta = new_size - _tabuList_size_;
          Bijection< GraphHash, Size > new_tabuList(2 * new_size);
          for (auto iter = _graph_tabuList_.begin(); iter != _graph_tabuList_.end(); ++iter) {
            new_tabuList.emplace(iter.first(), iter.second() + delta);
          }
          _graph_tabuList_ = std::move(new_tabuList);
        }
      }

      _tabuList_size_ = new_size;
    }

    bool
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

    void StructuralConstraintTabuList::modifyGraphAlone(const GraphChange& change) {
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
                    "Graph change operation "
                        << change.typeAsString()
                        << " is not supported by Tabu List structural constraint")
      }
    }
  } /* namespace learning */

} /* namespace gum */
