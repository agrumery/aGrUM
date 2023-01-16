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
 * @brief The basic class for computing the set of undigraph changes allowed by
 * the user to be executed by the learning algorithms
 *
 * Structure learning algorithm try different modifications of the graph. Class
 * GraphChangesGenerator4UndiGraph provides a simple way to compute those that
 *we
 * wish to perform. For instance, in the basic PC algorithm for learning
 * undirected graphs, one may expect that all possible edge additions and
 * deletions can be applied and GraphChangesGenerator4UndiGraph provides exactly
 * this set of operations. This class provides the following minimal methods:
 *   - void setGraph ( const UndiGraph& ) : assigns a new graph as a starting
 *     point to the generator of graph change operators
 *   - void modifyGraph ( const GraphChange& ) : indicate to the operator set
 *     that the graph has been changed and that we need to compute the new
 *     operators that result from this change
 *   - void clearChanges () : empty the set of possible operators
 *   - methods begin () and end () that return iterators allowing to parse the
 *     available set of operators.
 *
 * Basically, the idea is to use method setGraph at the beginning of the
 * structure learning in order to initialize the possible set of operators.
 * Then, parse this set using a for ( auto iter = operator_set.begin ();
 * iter != operator_set.end (); ++iter ) loop and compute the scores
 * induced by these changes. When this is done, flush the operator set by
 * calling method clearChanges. Then iterate changes and after each new change
 * applied, used again the iterator for loop, and so on. Note that, whenever
 * you execute method modifyGraph, this will automatically flush the current
 * list of changes and put into the list only the changes that are affected
 * by the graph modification.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_GRAPH_CHANGES_GENERATOR_4_UNDIGRAPH_H
#define GUM_LEARNING_GRAPH_CHANGES_GENERATOR_4_UNDIGRAPH_H

#include <agrum/agrum.h>
#include <agrum/tools/core/threads.h>
#include <agrum/tools/core/threadExecutor.h>
#include <agrum/BN/learning/structureUtils/IGraphChangesGenerator4UndiGraph.h>

namespace gum {

  namespace learning {

    /** @class GraphChangesGenerator4UndiGraph
     * @brief The basic class for computing the next graph changes possible in
     *an
     * undirected structure learning algorithm
     *
     * Structure learning algorithm try different modifications of the graph.
     *Class
     * GraphChangesGenerator4UndiGraph provides a simple way to compute those
     *that
     * we wish to perform. For instance, in the basic PC algorithm for learning
     * undirected graphs, one may expect that all possible edge additions and
     * deletions can be applied and GraphChangesGenerator4UndiGraph provides
     * exactly this set of operations. This class provides the following minimal
     * methods:
     *   - void setGraph ( const UndiGraph& ) : assigns a new graph as a
     *starting
     *     point to the generator of graph change operators
     *   - void modifyGraph ( const GraphChange& ) : indicate to the operator
     *set
     *     that the graph has been changed and that we need to compute the new
     *     operators that result from this change
     *   - void clearChanges () : empty the set of possible operators
     *   - methods begin () and end () that return iterators allowing to parse
     *the
     *     available set of operators.
     *
     * Basically, the idea is to use method setGraph at the beginning of the
     * structure learning in order to initialize the possible set of operators.
     * Then, parse this set using a for ( auto iter = operator_set.begin ();
     * iter != operator_set.end (); ++iter ) loop and compute the scores
     * induced by these changes. When this is done, flush the operator set by
     * calling method clearChanges. Then iterate changes and after each new
     *change
     * applied, used again the iterator for loop, and so on. Note that, whenever
     * you execute method modifyGraph, this will automatically flush the current
     * list of changes and put into the list only the changes that are affected
     * by the graph modification.
     *
     * @ingroup learning_group
     */
    template < typename STRUCT_CONSTRAINT >
    class GraphChangesGenerator4UndiGraph: public IGraphChangesGenerator4UndiGraph {
      public:
      /// the iterator for parsing the list of possible graph change operators
      using iterator = typename Set< GraphChange >::const_iterator;

      /// the const iterator for parsing the list of graph change operators
      using const_iterator = iterator;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      GraphChangesGenerator4UndiGraph(STRUCT_CONSTRAINT& constraint);

      /// copy constructor
      GraphChangesGenerator4UndiGraph(
         const GraphChangesGenerator4UndiGraph< STRUCT_CONSTRAINT >& from);

      /// move operator
      GraphChangesGenerator4UndiGraph(GraphChangesGenerator4UndiGraph< STRUCT_CONSTRAINT >&& from);

      /// destructor
      virtual ~GraphChangesGenerator4UndiGraph();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      GraphChangesGenerator4UndiGraph< STRUCT_CONSTRAINT >&
         operator=(const GraphChangesGenerator4UndiGraph< STRUCT_CONSTRAINT >& from);

      /// move operator
      GraphChangesGenerator4UndiGraph< STRUCT_CONSTRAINT >&
         operator=(GraphChangesGenerator4UndiGraph< STRUCT_CONSTRAINT >&& from);

      /// @}

      // ##########################################################################
      /// @name Iterators
      // ##########################################################################
      /// @{

      /// returns an (unsafe) iterator on the beginning of the list of operators
      iterator begin() const;

      /// returns an (unsafe) iterator on the end of the list of operators
      const iterator& end() const;

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// returns the constraint that is used by the generator
      STRUCT_CONSTRAINT& constraint() const noexcept;

      /// sets a new graph from which the operator will compute possible changes
      void setGraph(const UndiGraph& graph);

      /// notify the operator set of a change applied to the graph
      void modifyGraph(const EdgeAddition& change);

      /// notify the operator set of a change applied to the graph
      void modifyGraph(const EdgeDeletion& change);

      /// notify the operator set of a change applied to the graph
      void modifyGraph(const GraphChange& change);

      /// empty the set of possible change operators that can be applied
      void clearChanges() noexcept;

      /// notifies the generator that we have parsed all its legal changes
      void notifyGetCompleted();

      /// sets the maximum number of threads used to compute the set of changes
      void setMaxNbThreads(Size nb) noexcept;

      /// @}

      protected:
      /// the graph on which we generate operators
      UndiGraph graph_;

      /// a reference on the structural constraint used to restrict the changes
      STRUCT_CONSTRAINT* constraint_;

      /// the current set of operators
      Set< GraphChange > legal_changes_;

      /// create the set of legal and illegal changes from a given graph
      void createChanges_();

      private:
      /// the max number of threads authorized
      Size _max_threads_number_{gum::getNumberOfThreads()};
    };

  } /* namespace learning */

} /* namespace gum */

/// always include the templated functions
#include <agrum/BN/learning/structureUtils/graphChangesGenerator4UndiGraph_tpl.h>

#endif /* GUM_LEARNING_GRAPH_CHANGES_GENERATOR_4_UNDIGRAPH_H */
