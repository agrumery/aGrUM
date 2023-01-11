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
 * @brief The basic class for computing the next graph changes possible in an
 * undirected structure learning algorithm
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template < typename STRUCT_CONSTRAINT >
    GraphChangesGenerator4UndiGraph< STRUCT_CONSTRAINT >::GraphChangesGenerator4UndiGraph(
       STRUCT_CONSTRAINT& constraint) :
        constraint_(&constraint) {
      GUM_CONSTRUCTOR(GraphChangesGenerator4UndiGraph);
    }

    /// copy constructor
    template < typename STRUCT_CONSTRAINT >
    GraphChangesGenerator4UndiGraph< STRUCT_CONSTRAINT >::GraphChangesGenerator4UndiGraph(
       const GraphChangesGenerator4UndiGraph& from) :
        graph_(from.graph_),
        constraint_(from.constraint_), legal_changes_(from.legal_changes_),
        _max_threads_number_(from._max_threads_number_) {
      GUM_CONS_CPY(GraphChangesGenerator4UndiGraph);
    }

    /// move operator
    template < typename STRUCT_CONSTRAINT >
    GraphChangesGenerator4UndiGraph< STRUCT_CONSTRAINT >::GraphChangesGenerator4UndiGraph(
       GraphChangesGenerator4UndiGraph&& from) :
        graph_(std::move(from.graph_)),
        constraint_(from.constraint_), legal_changes_(std::move(from.legal_changes_)),
        _max_threads_number_(from._max_threads_number_) {
      GUM_CONS_MOV(GraphChangesGenerator4UndiGraph);
    }

    /// destructor
    template < typename STRUCT_CONSTRAINT >
    GraphChangesGenerator4UndiGraph< STRUCT_CONSTRAINT >::~GraphChangesGenerator4UndiGraph() {
      GUM_DESTRUCTOR(GraphChangesGenerator4UndiGraph);
    }

    /// copy operator
    template < typename STRUCT_CONSTRAINT >
    GraphChangesGenerator4UndiGraph< STRUCT_CONSTRAINT >&
       GraphChangesGenerator4UndiGraph< STRUCT_CONSTRAINT >::operator=(
          const GraphChangesGenerator4UndiGraph< STRUCT_CONSTRAINT >& from) {
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
    GraphChangesGenerator4UndiGraph< STRUCT_CONSTRAINT >&
       GraphChangesGenerator4UndiGraph< STRUCT_CONSTRAINT >::operator=(
          GraphChangesGenerator4UndiGraph< STRUCT_CONSTRAINT >&& from) {
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
    void GraphChangesGenerator4UndiGraph< STRUCT_CONSTRAINT >::createChanges_() {
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
                // try edge additions
                EdgeAddition edge_add(node1, node2);
                if (!this->constraint_->isAlwaysInvalid(edge_add)) {
                  legal_changes[this_thread].insert(std::move(edge_add));
                }

                // try edge deletion
                EdgeDeletion edge_del(node1, node2);
                if (!this->constraint_->isAlwaysInvalid(edge_del)) {
                  legal_changes[this_thread].insert(std::move(edge_del));
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
      // GraphChangesGenerator4UndiGraph
      for (const auto& changes: legal_changes) {
        for (const auto& change: changes) {
          legal_changes_.insert(std::move(change));
        }
      }
    }

    /// sets a new graph from which the operator will compute possible changes
    template < typename STRUCT_CONSTRAINT >
    INLINE void
       GraphChangesGenerator4UndiGraph< STRUCT_CONSTRAINT >::setGraph(const UndiGraph& graph) {
      // sets the current graph
      graph_ = graph;

      // generate the set of all changes
      createChanges_();
    }

    /// empty the set of possible change operators that can be applied
    template < typename STRUCT_CONSTRAINT >
    INLINE void GraphChangesGenerator4UndiGraph< STRUCT_CONSTRAINT >::clearChanges() noexcept {
      legal_changes_.clear();
    }

    /// returns an (unsafe) iterator on the beginning of the list of operators
    template < typename STRUCT_CONSTRAINT >
    INLINE typename GraphChangesGenerator4UndiGraph< STRUCT_CONSTRAINT >::iterator
       GraphChangesGenerator4UndiGraph< STRUCT_CONSTRAINT >::begin() const {
      return legal_changes_.cbegin();
    }

    /// returns an (unsafe) iterator on the end of the list of operators
    template < typename STRUCT_CONSTRAINT >
    INLINE const typename GraphChangesGenerator4UndiGraph< STRUCT_CONSTRAINT >::iterator&
       GraphChangesGenerator4UndiGraph< STRUCT_CONSTRAINT >::end() const {
      return legal_changes_.cend();
    }

    /// notify the operator set of a change applied to the graph
    template < typename STRUCT_CONSTRAINT >
    INLINE void GraphChangesGenerator4UndiGraph< STRUCT_CONSTRAINT >::modifyGraph(
       const EdgeAddition& change) {}

    /// notify the operator set of a change applied to the graph
    template < typename STRUCT_CONSTRAINT >
    INLINE void GraphChangesGenerator4UndiGraph< STRUCT_CONSTRAINT >::modifyGraph(
       const EdgeDeletion& change) {}

    /// notify the operator set of a change applied to the graph
    template < typename STRUCT_CONSTRAINT >
    INLINE void GraphChangesGenerator4UndiGraph< STRUCT_CONSTRAINT >::modifyGraph(
       const GraphChange& change) {}

    /// notifies the generator that we have parsed all its legal changes
    template < typename STRUCT_CONSTRAINT >
    INLINE void GraphChangesGenerator4UndiGraph< STRUCT_CONSTRAINT >::notifyGetCompleted() {
      if (legal_changes_.size()) legal_changes_.clear();
    }

    /// sets the maximum number of threads used to perform counts
    template < typename STRUCT_CONSTRAINT >
    INLINE void
       GraphChangesGenerator4UndiGraph< STRUCT_CONSTRAINT >::setMaxNbThreads(Size nb) noexcept {
      if (nb == 0) nb = gum::getNumberOfThreads();
      _max_threads_number_ = nb;
    }

    /// returns the constraint that is used by the generator
    template < typename STRUCT_CONSTRAINT >
    INLINE STRUCT_CONSTRAINT&
       GraphChangesGenerator4UndiGraph< STRUCT_CONSTRAINT >::constraint() const noexcept {
      return *constraint_;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
