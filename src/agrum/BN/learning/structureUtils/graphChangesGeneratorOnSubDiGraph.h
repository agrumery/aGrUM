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
 * @brief The class for computing the set of graph changes (over a subgraph)
 * transmitted to learning algorithms
 *
 * Structure learning algorithm try different modifications of the graph. Class
 * GraphChangesGeneratorOnSubDiGraph provides a simple way to compute those that
 * we wish to perform when we wish to limit the set of changes over a subgraph
 *of
 * the graph we learn. For instance, we may wish to relearn an already partially
 * learnt structure just around nodes A,B,C. In this case, we can set the set of
 * target nodes of the GraphChangesGeneratorOnSubDiGraph to {A,B,C} and the set
 * of starting nodes as all the nodes of the graph. In this case, only the arcs
 * whose heads are A, B or C will be trnasmitted to the learning algorithm.
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
#ifndef GUM_LEARNING_GRAPH_CHANGES_GENERATOR_ON_SUBDIGRAPH_H
#define GUM_LEARNING_GRAPH_CHANGES_GENERATOR_ON_SUBDIGRAPH_H

#include <agrum/agrum.h>
#include <agrum/tools/core/threads.h>
#include <agrum/tools/core/threadExecutor.h>
#include <agrum/BN/learning/structureUtils/IGraphChangesGenerator4DiGraph.h>

namespace gum {

  namespace learning {

    /** @class GraphChangesGeneratorOnSubDiGraph
     * @brief The basic class for computing the next graph changes possible in a
     * structure learning algorithm
     *
     * Structure learning algorithm try different modifications of the graph.
     *Class
     * GraphChangesGeneratorOnSubDiGraph provides a simple way to compute those
     * that we wish to perform when we wish to limit the set of changes over a
     * subgraph of the graph we learn. For instance, we may wish to relearn an
     * already partially learnt structure just around nodes A,B,C. In this case,
     * we can set the set of target nodes of the
     *GraphChangesGeneratorOnSubDiGraph
     * to {A,B,C} and the set of "tail" nodes as all the nodes of the graph. In
     * this case, only the arcs whose heads are A, B or C will be trnasmitted to
     * the learning algorithm.
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
    class GraphChangesGeneratorOnSubDiGraph: public IGraphChangesGenerator4DiGraph {
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
      GraphChangesGeneratorOnSubDiGraph(STRUCT_CONSTRAINT& constraint);

      /// copy constructor
      GraphChangesGeneratorOnSubDiGraph(
         const GraphChangesGeneratorOnSubDiGraph< STRUCT_CONSTRAINT >& from);

      /// move operator
      GraphChangesGeneratorOnSubDiGraph(
         GraphChangesGeneratorOnSubDiGraph< STRUCT_CONSTRAINT >&& from);

      /// destructor
      virtual ~GraphChangesGeneratorOnSubDiGraph();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      GraphChangesGeneratorOnSubDiGraph< STRUCT_CONSTRAINT >&
         operator=(const GraphChangesGeneratorOnSubDiGraph< STRUCT_CONSTRAINT >& from);

      /// move operator
      GraphChangesGeneratorOnSubDiGraph< STRUCT_CONSTRAINT >&
         operator=(GraphChangesGeneratorOnSubDiGraph< STRUCT_CONSTRAINT >&& from);

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
      void setGraph(const DiGraph& graph);

      /// assign a set of target nodes
      void setTargets(const NodeSet& nodes);

      /// adds a new target node
      void addTarget(NodeId node);

      /// removes a target
      void eraseTarget(NodeId node);

      /// assign a set of "tail" nodes
      void setTails(const NodeSet& nodes);

      /// assign a set of "tail" nodes from 0 to nb_nodes - 1
      void setTails(Size nb_nodes);

      /// adds a new "tail" node
      void addTail(NodeId node);

      /// removes a tail node
      void eraseTail(NodeId node);

      /// notify the operator set of a change applied to the graph
      void modifyGraph(const ArcAddition& change);

      /// notify the operator set of a change applied to the graph
      void modifyGraph(const ArcDeletion& change);

      /// notify the operator set of a change applied to the graph
      void modifyGraph(const ArcReversal& change);

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
      /// a reference on the structural constraint used to restrict the changes
      STRUCT_CONSTRAINT* constraint_;

      /// the set of target nodes
      NodeSet target_nodes_;

      /// the tail nodes (other extremities than the targets)
      NodeSet tail_nodes_;

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
#include <agrum/BN/learning/structureUtils/graphChangesGeneratorOnSubDiGraph_tpl.h>

#endif /* GUM_LEARNING_GRAPH_CHANGES_GENERATOR_ON_SUBDIGRAPH_H */
