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
 * @brief the base class for all the independence tests used for learning
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_LEARNING_PSEUDO_COUNT_H
#define GUM_LEARNING_PSEUDO_COUNT_H

#include <utility>

#include <agrum/agrum.h>

#include <agrum/tools/stattests/recordCounter.h>
#include <agrum/BN/learning/priors/prior.h>

namespace gum {

  namespace learning {

    /** @class PseudoCount
     * @brief The class for giving access to pseudo count : count in the database +
     * prior
     * @headerfile PseudoCount.h <agrum/BN/learning/pseudo-counts_and_tests/PseudoCount.h>
     * @ingroup learning_pseudo-counts
     */
    class PseudoCount: public IThreadNumberManager {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param parser the parser used to parse the database
       * @param external_prior An prior that we add to the computation of
       * the pseudo-count (this should come from expert knowledge): this consists
       * in adding numbers to counts in the contingency tables
       * @param ranges a set of pairs {(X1,Y1),...,(Xn,Yn)} of database's rows
       * indices. The counts are then performed only on the union of the
       * rows [Xi,Yi), i in {1,...,n}. This is useful, e.g, when performing
       * cross validation tasks, in which part of the database should be ignored.
       * An empty set of ranges is equivalent to an interval [X,Y) ranging over
       * the whole database.
       * @param nodeId2Columns a mapping from the ids of the nodes in the
       * graphical model to the corresponding column in the DatabaseTable
       * parsed by the parser. This enables estimating from a database in
       * which variable A corresponds to the 2nd column the parameters of a BN
       * in which variable A has a NodeId of 5. An empty nodeId2Columns
       * bijection means that the mapping is an identity, i.e., the value of a
       * NodeId is equal to the index of the column in the DatabaseTable.
       * @warning If nodeId2columns is not empty, then only the pseudo-counts over
       * the ids belonging to this bijection can be computed: applying method
       * pseudo-count() over other ids will raise exception NotFound. */
      PseudoCount(const DBRowGeneratorParser&                                 parser,
                  const Prior&                                                external_prior,
                  const std::vector< std::pair< std::size_t, std::size_t > >& ranges,
                  const Bijection< NodeId, std::size_t >&                     nodeId2columns
                  = Bijection< NodeId, std::size_t >());


      /// default constructor
      /** @param parser the parser used to parse the database
       * @param external_prior An prior that we add to the computation of
       * the pseudo-count (this should come from expert knowledge): this consists
       * in adding numbers to counts in the contingency tables
       * @param nodeId2Columns a mapping from the ids of the nodes in the
       * graphical model to the corresponding column in the DatabaseTable
       * parsed by the parser. This enables estimating from a database in
       * which variable A corresponds to the 2nd column the parameters of a BN
       * in which variable A has a NodeId of 5. An empty nodeId2Columns
       * bijection means that the mapping is an identity, i.e., the value of a
       * NodeId is equal to the index of the column in the DatabaseTable.
       * @warning If nodeId2columns is not empty, then only the pseudo-counts over
       * the ids belonging to this bijection can be computed: applying method
       * pseudo-count() over other ids will raise exception NotFound. */
      PseudoCount(const DBRowGeneratorParser&             parser,
                  const Prior&                            external_prior,
                  const Bijection< NodeId, std::size_t >& nodeId2columns
                  = Bijection< NodeId, std::size_t >());

      /// destructor
      virtual ~PseudoCount();


      /// copy constructor
      PseudoCount(const PseudoCount& from);

      /// move constructor
      PseudoCount(PseudoCount&& from);

      /// copy operator
      PseudoCount& operator=(const PseudoCount& from);

      /// move operator
      PseudoCount& operator=(PseudoCount&& from);
      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// sets the number max of threads that can be used
      /**
       * @param nb the number max of threads to be used. If this number is set to 0, then
       * it is defaulted to aGrUM's max number of threads
       */
      virtual void setNumberOfThreads(Size nb);

      /// returns the current max number of threads of the scheduler
      virtual Size getNumberOfThreads() const;

      /// indicates whether the user set herself the number of threads
      virtual bool isGumNumberOfThreadsOverriden() const;

      /** @brief changes the number min of rows a thread should process in a
       * multithreading context
       *
       * When computing pseudo-count, several threads are used by record counters
       * to perform counts on the rows of the database, the MinNbRowsPerThread
       * method indicates how many rows each thread should at least process.
       * This is used to compute the number of threads actually run. This number
       * is equal to the min between the max number of threads allowed and the
       * number of records in the database divided by nb. */
      virtual void setMinNbRowsPerThread(const std::size_t nb) const;

      /// returns the minimum of rows that each thread should process
      virtual std::size_t minNbRowsPerThread() const;

      /// sets new ranges to perform the counts used by the independence test
      /** @param ranges a set of pairs {(X1,Y1),...,(Xn,Yn)} of database's rows
       * indices. The counts are then performed only on the union of the
       * rows [Xi,Yi), i in {1,...,n}. This is useful, e.g, when performing
       * cross validation tasks, in which part of the database should be ignored.
       * An empty set of ranges is equivalent to an interval [X,Y) ranging over
       * the whole database. */
      void setRanges(const std::vector< std::pair< std::size_t, std::size_t > >& new_ranges);

      /// reset the ranges to the one range corresponding to the whole database
      void clearRanges();

      /// returns the current ranges
      const std::vector< std::pair< std::size_t, std::size_t > >& ranges() const;

      /// returns the pseudo-count of a pair of nodes given some other nodes
      /** @param var1 the first variable on the left side of the conditioning bar
       * @param var2 the second variable on the left side of the conditioning bar
       * @param rhs_ids the set of variables on the right side of the
       * conditioning bar */
      std::vector< double > get(const std::vector< NodeId >& ids);

      /// clears all the data structures from memory, including the cache
      virtual void clear();

      /// return the mapping between the columns of the database and the node ids
      /** @warning An empty nodeId2Columns bijection means that the mapping is
       * an identity, i.e., the value of a NodeId is equal to the index of the
       * column in the DatabaseTable. */
      const Bijection< NodeId, std::size_t >& nodeId2Columns() const;

      /// return the database used by the pseudo-count
      const DatabaseTable& database() const;

      /// @}


      protected:
      /// the expert knowledge a priorwe add to the contingency tables
      Prior* prior_{nullptr};

      /// the record counter used for the counts over discrete variables
      RecordCounter counter_;

      /// an empty vector
      const std::vector< NodeId > empty_ids_;
    }; /* namespace learning */
  }    // namespace learning
} /* namespace gum */

// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/tools/stattests/pseudoCount_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_PSEUDO_COUNT_H */
