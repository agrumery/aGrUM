/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
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
 * @brief The basic class for computing the next graph changes possible in a
 * (directed) structure learning algorithm
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template < typename STRUCT_CONSTRAINT >
    GraphChangesGenerator4DiGraph< STRUCT_CONSTRAINT >::GraphChangesGenerator4DiGraph(
        STRUCT_CONSTRAINT& constraint) : constraint_(&constraint) {
      GUM_CONSTRUCTOR(GraphChangesGenerator4DiGraph);
    }

    /// copy constructor
    template < typename STRUCT_CONSTRAINT >
    GraphChangesGenerator4DiGraph< STRUCT_CONSTRAINT >::GraphChangesGenerator4DiGraph(
        const GraphChangesGenerator4DiGraph& from) :
        graph_(from.graph_), constraint_(from.constraint_), legal_changes_(from.legal_changes_),
        _max_threads_number_(from._max_threads_number_) {
      GUM_CONS_CPY(GraphChangesGenerator4DiGraph);
    }

    /// move operator
    template < typename STRUCT_CONSTRAINT >
    GraphChangesGenerator4DiGraph< STRUCT_CONSTRAINT >::GraphChangesGenerator4DiGraph(
        GraphChangesGenerator4DiGraph&& from) :
        graph_(std::move(from.graph_)), constraint_(from.constraint_),
        legal_changes_(std::move(from.legal_changes_)),
        _max_threads_number_(from._max_threads_number_) {
      GUM_CONS_MOV(GraphChangesGenerator4DiGraph);
    }

    /// destructor
    template < typename STRUCT_CONSTRAINT >
    GraphChangesGenerator4DiGraph< STRUCT_CONSTRAINT >::~GraphChangesGenerator4DiGraph() {
      GUM_DESTRUCTOR(GraphChangesGenerator4DiGraph);
    }

    /// copy operator
    template < typename STRUCT_CONSTRAINT >
    GraphChangesGenerator4DiGraph< STRUCT_CONSTRAINT >&
        GraphChangesGenerator4DiGraph< STRUCT_CONSTRAINT >::operator=(
            const GraphChangesGenerator4DiGraph< STRUCT_CONSTRAINT >& from) {
      if (this != &from) {
        graph_               = from.graph_;
        constraint_          = from.constraint_;
        legal_changes_       = from.legal_changes_;
        _max_threads_number_ = from._max_threads_number_;
      }
      return *this;
    }

    /// move operator
    template < typename STRUCT_CONSTRAINT >
    GraphChangesGenerator4DiGraph< STRUCT_CONSTRAINT >&
        GraphChangesGenerator4DiGraph< STRUCT_CONSTRAINT >::operator=(
            GraphChangesGenerator4DiGraph< STRUCT_CONSTRAINT >&& from) {
      if (this != &from) {
        graph_               = std::move(from.graph_);
        constraint_          = std::move(from.constraint_);
        legal_changes_       = std::move(from.legal_changes_);
        _max_threads_number_ = from._max_threads_number_;
      }
      return *this;
    }

    /// create the set of legal and illegal changes from a given graph
    template < typename STRUCT_CONSTRAINT >
    void GraphChangesGenerator4DiGraph< STRUCT_CONSTRAINT >::createChanges_() {
      legal_changes_.clear();

      // for all the pairs of nodes, consider adding, reverse and removing arcs
      // do it for each thread
      const Size                        nb_threads = _max_threads_number_;
      std::vector< Set< GraphChange > > legal_changes(nb_threads);

      // create the lambda that will be used to fill the legal changes
      auto threadedLegalSet = [this, &legal_changes](const std::size_t this_thread,
                                                     const std::size_t nb_threads) -> void {
        Idx i = 0;
        for (const auto node1: this->graph_) {
          if (i == this_thread) {
            for (const auto node2: this->graph_) {
              if (node1 != node2) {
                // try arc additions
                ArcAddition arc_add(node1, node2);
                if (!this->constraint_->isAlwaysInvalid(arc_add)) {
                  legal_changes[this_thread].insert(std::move(arc_add));
                }

                // try arc deletion
                ArcDeletion arc_del(node1, node2);
                if (!this->constraint_->isAlwaysInvalid(arc_del)) {
                  legal_changes[this_thread].insert(std::move(arc_del));
                }

                // try arc reversal
                ArcReversal arc_rev(node1, node2);
                if (!this->constraint_->isAlwaysInvalid(arc_rev)) {
                  legal_changes[this_thread].insert(std::move(arc_rev));
                }
              }
            }
          }
          ++i;
          i %= nb_threads;
        }
      };

      // launch the threads
      ThreadExecutor::execute(nb_threads, threadedLegalSet);


      // now store the changes into the protected vectors of the
      // GraphChangesGenerator4DiGraph
      for (const auto& changes: legal_changes) {
        for (const auto& change: changes) {
          legal_changes_.insert(std::move(change));
        }
      }
    }

    /// sets a new graph from which the operator will compute possible changes
    template < typename STRUCT_CONSTRAINT >
    INLINE void GraphChangesGenerator4DiGraph< STRUCT_CONSTRAINT >::setGraph(const DiGraph& graph) {
      // sets the current graph
      graph_ = graph;

      // generate the set of all changes
      createChanges_();
    }

    /// empty the set of possible change operators that can be applied
    template < typename STRUCT_CONSTRAINT >
    INLINE void GraphChangesGenerator4DiGraph< STRUCT_CONSTRAINT >::clearChanges() noexcept {
      legal_changes_.clear();
    }

    /// returns an (unsafe) iterator on the beginning of the list of operators
    template < typename STRUCT_CONSTRAINT >
    INLINE typename GraphChangesGenerator4DiGraph< STRUCT_CONSTRAINT >::iterator
        GraphChangesGenerator4DiGraph< STRUCT_CONSTRAINT >::begin() const {
      return legal_changes_.cbegin();
    }

    /// returns an (unsafe) iterator on the end of the list of operators
    template < typename STRUCT_CONSTRAINT >
    INLINE const typename GraphChangesGenerator4DiGraph< STRUCT_CONSTRAINT >::iterator&
        GraphChangesGenerator4DiGraph< STRUCT_CONSTRAINT >::end() const {
      return legal_changes_.cend();
    }

    /// notify the operator set of a change applied to the graph
    template < typename STRUCT_CONSTRAINT >
    INLINE void
        GraphChangesGenerator4DiGraph< STRUCT_CONSTRAINT >::modifyGraph(const ArcAddition& change) {
    }

    /// notify the operator set of a change applied to the graph
    template < typename STRUCT_CONSTRAINT >
    INLINE void
        GraphChangesGenerator4DiGraph< STRUCT_CONSTRAINT >::modifyGraph(const ArcDeletion& change) {
    }

    /// notify the operator set of a change applied to the graph
    template < typename STRUCT_CONSTRAINT >
    INLINE void
        GraphChangesGenerator4DiGraph< STRUCT_CONSTRAINT >::modifyGraph(const ArcReversal& change) {
    }

    /// notify the operator set of a change applied to the graph
    template < typename STRUCT_CONSTRAINT >
    INLINE void
        GraphChangesGenerator4DiGraph< STRUCT_CONSTRAINT >::modifyGraph(const GraphChange& change) {
    }

    /// notifies the generator that we have parsed all its legal changes
    template < typename STRUCT_CONSTRAINT >
    INLINE void GraphChangesGenerator4DiGraph< STRUCT_CONSTRAINT >::notifyGetCompleted() {
      if (legal_changes_.size()) legal_changes_.clear();
    }

    /// sets the maximum number of threads used to perform counts
    template < typename STRUCT_CONSTRAINT >
    INLINE void
        GraphChangesGenerator4DiGraph< STRUCT_CONSTRAINT >::setMaxNbThreads(Size nb) noexcept {
      if (nb == 0) nb = gum::getNumberOfThreads();
      _max_threads_number_ = nb;
    }

    /// returns the constraint that is used by the generator
    template < typename STRUCT_CONSTRAINT >
    INLINE STRUCT_CONSTRAINT&
        GraphChangesGenerator4DiGraph< STRUCT_CONSTRAINT >::constraint() const noexcept {
      return *constraint_;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
