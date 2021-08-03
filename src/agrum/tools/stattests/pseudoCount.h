/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
#include <agrum/tools/core/math/math_utils.h>
#include <agrum/tools/core/OMPThreads.h>

#include <agrum/tools/stattests/recordCounter.h>
#include <agrum/BN/learning/aprioris/apriori.h>
#include <agrum/tools/variables/discreteVariable.h>
#include <agrum/tools/multidim/potential.h>

namespace gum {

  namespace learning {

    /** @class PseudoCount
     * @brief The class for giving access to pseudo count : count in the database +
     * prior
     * @headerfile PseudoCount.h <agrum/BN/learning/pseudo-counts_and_tests/PseudoCount.h>
     * @ingroup learning_pseudo-counts
     */
    template < template < typename > class ALLOC = std::allocator >
    class PseudoCount {
      public:
      /// type for the allocators passed in arguments of methods
      using allocator_type = ALLOC< NodeId >;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param parser the parser used to parse the database
       * @param external_apriori An apriori that we add to the computation of
       * the pseudo-count (this should come from expert knowledge): this consists
       * in adding numbers to countings in the contingency tables
       * @param ranges a set of pairs {(X1,Y1),...,(Xn,Yn)} of database's rows
       * indices. The countings are then performed only on the union of the
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
       * @param alloc the allocator used to allocate the structures within the
       * PseudoCount.
       * @warning If nodeId2columns is not empty, then only the pseudo-counts over
       * the ids belonging to this bijection can be computed: applying method
       * pseudo-count() over other ids will raise exception NotFound. */
      PseudoCount(const DBRowGeneratorParser< ALLOC >& parser,
                  const Apriori< ALLOC >&              external_apriori,
                  const std::vector< std::pair< std::size_t, std::size_t >,
                                     ALLOC< std::pair< std::size_t, std::size_t > > >& ranges,
                  const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >& nodeId2columns
                  = Bijection< NodeId, std::size_t, ALLOC< std::size_t > >(),
                  const allocator_type& alloc = allocator_type());


      /// default constructor
      /** @param parser the parser used to parse the database
       * @param external_apriori An apriori that we add to the computation of
       * the pseudo-count (this should come from expert knowledge): this consists
       * in adding numbers to countings in the contingency tables
       * @param nodeId2Columns a mapping from the ids of the nodes in the
       * graphical model to the corresponding column in the DatabaseTable
       * parsed by the parser. This enables estimating from a database in
       * which variable A corresponds to the 2nd column the parameters of a BN
       * in which variable A has a NodeId of 5. An empty nodeId2Columns
       * bijection means that the mapping is an identity, i.e., the value of a
       * NodeId is equal to the index of the column in the DatabaseTable.
       * @param alloc the allocator used to allocate the structures within the
       * PseudoCount.
       * @warning If nodeId2columns is not empty, then only the pseudo-counts over
       * the ids belonging to this bijection can be computed: applying method
       * pseudo-count() over other ids will raise exception NotFound. */
      PseudoCount(const DBRowGeneratorParser< ALLOC >&                          parser,
                  const Apriori< ALLOC >&                                       external_apriori,
                  const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >& nodeId2columns
                  = Bijection< NodeId, std::size_t, ALLOC< std::size_t > >(),
                  const allocator_type& alloc = allocator_type());

      /// destructor
      virtual ~PseudoCount();


      /// copy constructor
      PseudoCount(const PseudoCount< ALLOC >& from);

      /// copy constructor with a given allocator
      PseudoCount(const PseudoCount< ALLOC >& from, const allocator_type& alloc);

      /// move constructor
      PseudoCount(PseudoCount< ALLOC >&& from);

      /// move constructor with a given allocator
      PseudoCount(PseudoCount< ALLOC >&& from, const allocator_type& alloc);

      /// copy operator
      PseudoCount< ALLOC >& operator=(const PseudoCount< ALLOC >& from);

      /// move operator
      PseudoCount< ALLOC >& operator=(PseudoCount< ALLOC >&& from);
      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// changes the max number of threads used to parse the database
      virtual void setMaxNbThreads(std::size_t nb) const;

      /// returns the number of threads used to parse the database
      virtual std::size_t nbThreads() const;

      /** @brief changes the number min of rows a thread should process in a
       * multithreading context
       *
       * When computing pseudo-count, several threads are used by record counters
       * to perform countings on the rows of the database, the MinNbRowsPerThread
       * method indicates how many rows each thread should at least process.
       * This is used to compute the number of threads actually run. This number
       * is equal to the min between the max number of threads allowed and the
       * number of records in the database divided by nb. */
      virtual void setMinNbRowsPerThread(const std::size_t nb) const;

      /// returns the minimum of rows that each thread should process
      virtual std::size_t minNbRowsPerThread() const;

      /// sets new ranges to perform the countings used by the independence test
      /** @param ranges a set of pairs {(X1,Y1),...,(Xn,Yn)} of database's rows
       * indices. The countings are then performed only on the union of the
       * rows [Xi,Yi), i in {1,...,n}. This is useful, e.g, when performing
       * cross validation tasks, in which part of the database should be ignored.
       * An empty set of ranges is equivalent to an interval [X,Y) ranging over
       * the whole database. */
      template < template < typename > class XALLOC >
      void setRanges(
         const std::vector< std::pair< std::size_t, std::size_t >,
                            XALLOC< std::pair< std::size_t, std::size_t > > >& new_ranges);

      /// reset the ranges to the one range corresponding to the whole database
      void clearRanges();

      /// returns the current ranges
      const std::vector< std::pair< std::size_t, std::size_t >,
                         ALLOC< std::pair< std::size_t, std::size_t > > >&
         ranges() const;

      /// returns the pseudo-count of a pair of nodes given some other nodes
      /** @param var1 the first variable on the left side of the conditioning bar
       * @param var2 the second variable on the left side of the conditioning bar
       * @param rhs_ids the set of variables on the right side of the
       * conditioning bar */
      std::vector< double, ALLOC< double > > get(const std::vector< NodeId, ALLOC< NodeId > >& ids);

      /// clears all the data structures from memory, including the cache
      virtual void clear();

      /// return the mapping between the columns of the database and the node ids
      /** @warning An empty nodeId2Columns bijection means that the mapping is
       * an identity, i.e., the value of a NodeId is equal to the index of the
       * column in the DatabaseTable. */
      const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >& nodeId2Columns() const;

      /// return the database used by the pseudo-count
      const DatabaseTable< ALLOC >& database() const;

      /// returns the allocator used by the pseudo-count
      allocator_type getAllocator() const;

      /// @}


      protected:
      /// the expert knowledge a priori we add to the contingency tables
      Apriori< ALLOC >* apriori_{nullptr};

      /// the record counter used for the countings over discrete variables
      RecordCounter< ALLOC > counter_;

      /// an empty vector
      const std::vector< NodeId, ALLOC< NodeId > > empty_ids_;
    }; /* namespace learning */
  }    // namespace learning
} /* namespace gum */


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::learning::PseudoCount<>;
#endif


/// include the template implementation
#include <agrum/tools/stattests/pseudoCount_tpl.h>

#endif /* GUM_LEARNING_PSEUDO_COUNT_H */
