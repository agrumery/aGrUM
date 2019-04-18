
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_INDEPENDENCE_TEST_H
#define GUM_LEARNING_INDEPENDENCE_TEST_H

#include <utility>

#include <agrum/agrum.h>
#include <agrum/core/math/math.h>
#include <agrum/core/OMPThreads.h>

#include <agrum/learning/scores_and_tests/recordCounter.h>
#include <agrum/learning/scores_and_tests/scoringCache.h>
#include <agrum/learning/aprioris/apriori.h>
#include <agrum/learning/structureUtils/graphChange.h>

namespace gum {

  namespace learning {

    /** @class IndependenceTest
     * @brief The base class for all the independence tests used for learning
     * @headerfile independenceTest.h <agrum/learning/scores_and_tests/independenceTest.h>
     * @ingroup learning_scores
     */
    template < template < typename > class ALLOC = std::allocator >
    class IndependenceTest {
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
       * the score (this should come from expert knowledge): this consists in
       * adding numbers to countings in the contingency tables
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
       * IndependenceTest.
       * @warning If nodeId2columns is not empty, then only the scores over the
       * ids belonging to this bijection can be computed: applying method
       * score() over other ids will raise exception NotFound. */
      IndependenceTest(
         const DBRowGeneratorParser< ALLOC >& parser,
         const Apriori< ALLOC >&              external_apriori,
         const std::vector< std::pair< std::size_t, std::size_t >,
                            ALLOC< std::pair< std::size_t, std::size_t > > >&
            ranges,
         const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
            nodeId2columns =
               Bijection< NodeId, std::size_t, ALLOC< std::size_t > >(),
         const allocator_type& alloc = allocator_type());


      /// default constructor
      /** @param parser the parser used to parse the database
       * @param external_apriori An apriori that we add to the computation of
       * the score (this should come from expert knowledge): this consists in
       * adding numbers to countings in the contingency tables
       * @param nodeId2Columns a mapping from the ids of the nodes in the
       * graphical model to the corresponding column in the DatabaseTable
       * parsed by the parser. This enables estimating from a database in
       * which variable A corresponds to the 2nd column the parameters of a BN
       * in which variable A has a NodeId of 5. An empty nodeId2Columns
       * bijection means that the mapping is an identity, i.e., the value of a
       * NodeId is equal to the index of the column in the DatabaseTable.
       * @param alloc the allocator used to allocate the structures within the
       * IndependenceTest.
       * @warning If nodeId2columns is not empty, then only the scores over the
       * ids belonging to this bijection can be computed: applying method
       * score() over other ids will raise exception NotFound. */
      IndependenceTest(
         const DBRowGeneratorParser< ALLOC >& parser,
         const Apriori< ALLOC >&              external_apriori,
         const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
            nodeId2columns =
               Bijection< NodeId, std::size_t, ALLOC< std::size_t > >(),
         const allocator_type& alloc = allocator_type());

      /// virtual copy constructor
      virtual IndependenceTest< ALLOC >* clone() const = 0;

      /// virtual copy constructor with a given allocator
      virtual IndependenceTest< ALLOC >*
         clone(const allocator_type& alloc) const = 0;

      /// destructor
      virtual ~IndependenceTest();

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
       * When computing score, several threads are used by record counters to
       * perform countings on the rows of the database, the MinNbRowsPerThread
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
                            XALLOC< std::pair< std::size_t, std::size_t > > >&
            new_ranges);

      /// reset the ranges to the one range corresponding to the whole database
      void clearRanges();

      /// returns the current ranges
      const std::vector< std::pair< std::size_t, std::size_t >,
                         ALLOC< std::pair< std::size_t, std::size_t > > >&
         ranges() const;


      /// returns the score of a pair of nodes
      double score(const NodeId var1, const NodeId var2);

      /// returns the score of a pair of nodes given some other nodes
      /** @param var1 the first variable on the left side of the conditioning bar
       * @param var2 the second variable on the left side of the conditioning bar
       * @param rhs_ids the set of variables on the right side of the
       * conditioning bar */
      double score(const NodeId                                  var1,
                   const NodeId                                  var2,
                   const std::vector< NodeId, ALLOC< NodeId > >& rhs_ids);

      /// clears all the data structures from memory, including the cache
      virtual void clear();

      /// clears the current cache
      virtual void clearCache();

      /// turn on/off the use of a cache of the previously computed score
      virtual void useCache(const bool on_off);

      /// return the mapping between the columns of the database and the node ids
      /** @warning An empty nodeId2Columns bijection means that the mapping is
       * an identity, i.e., the value of a NodeId is equal to the index of the
       * column in the DatabaseTable. */
      const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
         nodeId2Columns() const;

      /// return the database used by the score
      const DatabaseTable< ALLOC >& database() const;

      /// returns the allocator used by the score
      allocator_type getAllocator() const;

      /// @}


      protected:
      /// 1 / log(2)
      const double _1log2{M_LOG2E};

      /// the expert knowledge a priori we add to the contongency tables
      Apriori< ALLOC >* _apriori{nullptr};

      /// the record counter used for the countings over discrete variables
      RecordCounter< ALLOC > _counter;

      /// the scoring cache
      ScoringCache< ALLOC > _cache;

      /// a Boolean indicating whether we wish to use the cache
      bool _use_cache{true};

      /// an empty vector
      const std::vector< NodeId, ALLOC< NodeId > > _empty_ids;


      /// copy constructor
      IndependenceTest(const IndependenceTest< ALLOC >& from);

      /// copy constructor with a given allocator
      IndependenceTest(const IndependenceTest< ALLOC >& from,
                       const allocator_type&            alloc);

      /// move constructor
      IndependenceTest(IndependenceTest< ALLOC >&& from);

      /// move constructor with a given allocator
      IndependenceTest(IndependenceTest< ALLOC >&& from,
                       const allocator_type&       alloc);

      /// copy operator
      IndependenceTest< ALLOC >& operator=(const IndependenceTest< ALLOC >& from);

      /// move operator
      IndependenceTest< ALLOC >& operator=(IndependenceTest< ALLOC >&& from);

      /// returns the score for a given IdSet
      /** @throws OperationNotAllowed is raised if the score does not support
       * calling method score such an idset (due to too many/too few variables
       * in the left hand side or the right hand side of the idset). */
      virtual double _score(const IdSet< ALLOC >& idset) = 0;

      /// returns a counting vector where variables are marginalized from N_xyz
      /** @param node_2_marginalize indicates which node(s) shall be marginalized:
       * - 0 means that X should be marginalized
       * - 1 means that Y should be marginalized
       * - 2 means that Z should be marginalized
       * @param X_size the domain size of variable X
       * @param Y_size the domain size of variable Y
       * @param Z_size the domain size of the set of conditioning variables Z
       * @param N_xyz a counting vector of dimension X * Y * Z (in this order)
       */
      std::vector< double, ALLOC< double > >
         _marginalize(const std::size_t node_2_marginalize,
                      const std::size_t X_size,
                      const std::size_t Y_size,
                      const std::size_t Z_size,
                      const std::vector< double, ALLOC< double > >& N_xyz) const;
    };

  } /* namespace learning */

} /* namespace gum */


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::learning::IndependenceTest<>;
#endif


/// include the template implementation
#include <agrum/learning/scores_and_tests/independenceTest_tpl.h>

#endif /* GUM_LEARNING_INDEPENDENCE_TEST_H */
