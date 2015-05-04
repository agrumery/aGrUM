/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
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
 * @brief The basic class for computing the set of digraph changes allowed by
 * the user to be executed by the learning algorithms
 *
 * Structure learning algorithm try different modifications of the graph. Class
 * GraphChangesGenerator4DiGraph provides a simple way to compute those that we
 * wish to perform. For instance, in the basic LocalSearch algorithm for learning
 * directed graphs, one may expect that all possible arc additions, deletions and
 * reversals can be applied and GraphChangesGenerator4DiGraph provides exactly
 * this set of operations. However, there may be cases where we would like to
 * apply these operators, say, only on a subgraph. In this case, we should use
 * the derived class of GraphChangesGenerator4DiGraph named
 * GraphChangesGeneratorOnSubDiGraph. Anyway, all the search generators should
 * have the following minimal methods:
 *   - void setGraph ( const DiGraph& ) : assigns a new graph as a starting
 *     point to the generator of graph change operators
 *   - void modifyGraph ( const GraphChange& ) : indicate to the generator
 *     that the graph has been changed and that we need to compute the new
 *     operators that result from this change
 *   - void clearChanges () : empty the set of possible changes
 *   - methods begin () and end () that return iterators allowing to parse the
 *     available set of changes.
 *
 * Basically, the idea is to use method setGraph at the beginning of the
 * structure learning in order to initialize the possible set of changes.
 * Then, parse this set using a for ( auto iter = generator.begin ();
 * iter != generator.end (); ++iter ) loop and compute the scores
 * induced by these changes. When this is done, flush the generator by
 * calling method clearChanges. Then iterate changes and after each new change
 * applied, used again the iterator for loop, and so on. Note that, whenever
 * you execute method modifyGraph, this will automatically flush the current
 * list of changes and put into the list only the changes that are affected
 * by the graph modification.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_GRAPH_CHANGES_GENERATOR_4_K2_H
#define GUM_LEARNING_GRAPH_CHANGES_GENERATOR_4_K2_H

#include <agrum/config.h>
#include <agrum/core/set.h>
#include <agrum/core/OMPThreads.h>
#include <agrum/core/sequence.h>

#include <agrum/graphs/diGraph.h>
#include <agrum/learning/structureUtils/graphChange.h>
#include <agrum/learning/structureUtils/IGraphChangesGenerator4DiGraph.h>

namespace gum {

  namespace learning {

    // a dummy class used to check that the generator is adapted to K2
    struct __GraphChangesGenerator4K2 {};

    /** @class GraphChangesGenerator4K2
     * @brief The basic class for computing the next graph changes possible in a
     * structure learning algorithm
     *
     * Structure learning algorithm try different modifications of the graph. Class
     * GraphChangesGenerator4K2 provides a simple way to compute those that
     * we wish to perform. For instance, in the basic LocalSearch algorithm for
     * learning directed graphs, one may expect that all possible arc additions,
     * deletions and reversals can be applied and GraphChangesGenerator4K2
     * provides exactly this set of operations. However, there may be cases where
     * we would like to apply these operators, say, only on a subgraph. In this
     * case, we should use the derived class of GraphChangesGenerator4K2 named
     * GraphChangesGeneratorOnSubDiGraph. Anyway, all the search generators
     * should have the following minimal methods:
     *   - void setGraph ( const DiGraph& ) : assigns a new graph as a starting
     *     point to the generator of graph change operators
     *   - void modifyGraph ( const GraphChange& ) : indicate to the generator
     *     that the graph has been changed and that we need to compute the new
     *     operators that result from this change
     *   - void clearChanges () : empty the set of possible changes
     *   - methods begin () and end () that return iterators allowing to parse the
     *     available set of changes.
     *
     * Basically, the idea is to use method setGraph at the beginning of the
     * structure learning in order to initialize the possible set of operators.
     * Then, parse this set using a for ( auto iter = generator.begin ();
     * iter != generator.end (); ++iter ) loop and compute the scores
     * induced by these changes. When this is done, flush the generator by
     * calling method clearChanges. Then iterate changes and after each new change
     * applied, used again the iterator for loop, and so on. Note that, whenever
     * you execute method modifyGraph, this will automatically flush the current
     * list of changes and put into the list only the changes that are affected
     * by the graph modification.
     *
     * @ingroup learning_group
     */
    template <typename STRUCT_CONSTRAINT>
    class GraphChangesGenerator4K2 : public IGraphChangesGenerator4DiGraph,
                                     __GraphChangesGenerator4K2 {
      public:
      /// the iterator for parsing the list of possible graph change operators
      using iterator = typename Set<GraphChange>::const_iterator;

      /// the const iterator for parsing the list of graph change operators
      using const_iterator = iterator;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      GraphChangesGenerator4K2(STRUCT_CONSTRAINT &constraint);

      /// copy constructor
      GraphChangesGenerator4K2(
          const GraphChangesGenerator4K2<STRUCT_CONSTRAINT> &from);

      /// move operator
      GraphChangesGenerator4K2(GraphChangesGenerator4K2<STRUCT_CONSTRAINT> &&from);

      /// destructor
      virtual ~GraphChangesGenerator4K2();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      GraphChangesGenerator4K2<STRUCT_CONSTRAINT> &
      operator=(const GraphChangesGenerator4K2<STRUCT_CONSTRAINT> &from);

      /// move operator
      GraphChangesGenerator4K2<STRUCT_CONSTRAINT> &
      operator=(GraphChangesGenerator4K2<STRUCT_CONSTRAINT> &&from);

      /// @}

      // ##########################################################################
      /// @name Iterators
      // ##########################################################################
      /// @{

      /// returns an (unsafe) iterator on the beginning of the list of operators
      iterator begin() const;

      /// returns an (unsafe) iterator on the end of the list of operators
      const iterator &end() const;

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// returns the constraint that is used by the generator
      STRUCT_CONSTRAINT &constraint() const noexcept;

      /// sets a new graph from which the generator will compute possible changes
      void setGraph(const DiGraph &graph);

      /// notify the generator of a change applied to the graph
      void modifyGraph(const ArcAddition &change);

      /// notify the generator of a change applied to the graph
      void modifyGraph(const ArcDeletion &change);

      /// notify the generator of a change applied to the graph
      void modifyGraph(const ArcReversal &change);

      /// notify the generator of a change applied to the graph
      void modifyGraph(const GraphChange &change);

      /// empty the set of possible change operators that can be applied
      void clearChanges() noexcept;

      /// notifies the generator that we have parsed all its legal changes
      void notifyGetCompleted();

      /// sets the maximum number of threads used to compute the set of changes
      void setMaxNbThreads(unsigned int nb) noexcept;

      /// set a new order on the random variables
      void setOrder(const Sequence<NodeId> &order);

      /// set a new order on the random variables
      void setOrder(const std::vector<NodeId> &order);

      /// @}

      protected:
      /// the graph on which we generate operators
      DiGraph _graph;

      /// the structural constraint used to restrict the changes
      STRUCT_CONSTRAINT *_constraint;

      /// the order on the variables
      Sequence<NodeId> _order;

      /// the current set of graph changes
      Set<GraphChange> _legal_changes;

      /// create the set of legal and illegal changes from a given graph
      void _createChanges();

      private:
/// the max number of threads authorized
#if defined(_OPENMP) && defined(NDEBUG)
      unsigned int __max_threads_number{getMaxNumberOfThreads()};
#else
      unsigned int __max_threads_number{1};
#endif /* NDEBUG */
    };

  } /* namespace learning */

} /* namespace gum */

/// always include the templated functions
#include <agrum/learning/structureUtils/graphChangesGenerator4K2.tcc>

#endif /* GUM_LEARNING_GRAPH_CHANGES_GENERATOR_4_K2_H */
