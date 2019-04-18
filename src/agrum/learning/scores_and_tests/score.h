
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
 * @brief the base class for all the scores used for learning (BIC, BDeu, etc)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_SCORE_H
#define GUM_LEARNING_SCORE_H

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

    /** @class Score
     * @brief The base class for all the scores used for learning (BIC, BDeu, etc)
     * @headerfile score.h <agrum/learning/scores_and_tests/score.h>
     * @ingroup learning_scores
     */
    template < template < typename > class ALLOC = std::allocator >
    class Score {
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
       * the score (this should come from expert knowledge)
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
       * Score.
       * @warning If nodeId2columns is not empty, then only the scores over the
       * ids belonging to this bijection can be computed: applying method
       * score() over other ids will raise exception NotFound. */
      Score(const DBRowGeneratorParser< ALLOC >& parser,
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
       * the score (this should come from expert knowledge)
       * @param nodeId2Columns a mapping from the ids of the nodes in the
       * graphical model to the corresponding column in the DatabaseTable
       * parsed by the parser. This enables estimating from a database in
       * which variable A corresponds to the 2nd column the parameters of a BN
       * in which variable A has a NodeId of 5. An empty nodeId2Columns
       * bijection means that the mapping is an identity, i.e., the value of a
       * NodeId is equal to the index of the column in the DatabaseTable.
       * @param alloc the allocator used to allocate the structures within the
       * Score.
       * @warning If nodeId2columns is not empty, then only the scores over the
       * ids belonging to this bijection can be computed: applying method
       * score() over other ids will raise exception NotFound. */
      Score(const DBRowGeneratorParser< ALLOC >& parser,
            const Apriori< ALLOC >&              external_apriori,
            const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
               nodeId2columns =
                  Bijection< NodeId, std::size_t, ALLOC< std::size_t > >(),
            const allocator_type& alloc = allocator_type());

      /// virtual copy constructor
      virtual Score< ALLOC >* clone() const = 0;

      /// virtual copy constructor with a given allocator
      virtual Score< ALLOC >* clone(const allocator_type& alloc) const = 0;

      /// destructor
      virtual ~Score();

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

      /// sets new ranges to perform the countings used by the score
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

      /// returns the score of a single node
      double score(const NodeId var);

      /// returns the score of a single node given some other nodes
      /** @param var the variable on the left side of the conditioning bar
       * @param rhs_ids the set of variables on the right side of the
       * conditioning bar */
      double score(const NodeId                                  var,
                   const std::vector< NodeId, ALLOC< NodeId > >& rhs_ids);

      /// clears all the data structures from memory, including the cache
      void clear();

      /// clears the current cache
      void clearCache();

      /// turn on/off the use of a cache of the previously computed score
      void useCache(const bool on_off);

      /// indicates whether the score uses a cache
      bool isUsingCache() const;

      /// return the mapping between the columns of the database and the node ids
      /** @warning An empty nodeId2Columns bijection means that the mapping is
       * an identity, i.e., the value of a NodeId is equal to the index of the
       * column in the DatabaseTable. */
      const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
         nodeId2Columns() const;

      /// return the database used by the score
      const DatabaseTable< ALLOC >& database() const;

      /// indicates whether the apriori is compatible (meaningful) with the score
      /** The combination of some scores and aprioris can be meaningless. For
       * instance, adding a Dirichlet apriori to the K2 score is not very
       * meaningful since K2 corresonds to a BD score with a 1-smoothing
       * apriori.
       * aGrUM allows you to perform such combination, but you can check with
       * method isAprioriCompatible () whether the result the score will give
       * you is meaningful or not. */
      virtual std::string isAprioriCompatible() const = 0;

      /// returns the internal apriori of the score
      /** Some scores include an apriori. For instance, the K2 score is a BD
       * score with a Laplace Apriori ( smoothing(1) ). BDeu is a BD score with
       * a N'/(r_i * q_i) apriori, where N' is an effective sample size and r_i
       * is the domain size of the target variable and q_i is the domain size of
       * the Cartesian product of its parents. The goal of the score's internal
       * apriori classes is to enable to account for these aprioris outside the
       * score, e.g., when performing parameter estimation. It is important to
       * note that, to be meaningful, a structure + parameter learning requires
       * that the same aprioris are taken into account during structure learning
       * and parameter learning. */
      virtual const Apriori< ALLOC >& internalApriori() const = 0;

      /// returns the allocator used by the score
      allocator_type getAllocator() const;

      /// @}


      protected:
      /// 1 / log(2)
      const double _1log2{M_LOG2E};

      /// the expert knowledge a priori we add to the score
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
      Score(const Score< ALLOC >& from);

      /// copy constructor with a given allocator
      Score(const Score< ALLOC >& from, const allocator_type& alloc);

      /// move constructor
      Score(Score< ALLOC >&& from);

      /// move constructor with a given allocator
      Score(Score< ALLOC >&& from, const allocator_type& alloc);

      /// copy operator
      Score< ALLOC >& operator=(const Score< ALLOC >& from);

      /// move operator
      Score< ALLOC >& operator=(Score< ALLOC >&& from);

      /// returns the score for a given IdSet
      /** @throws OperationNotAllowed is raised if the score does not support
       * calling method score such an idset (due to too many/too few variables
       * in the left hand side or the right hand side of the idset). */
      virtual double _score(const IdSet< ALLOC >& idset) = 0;

      /// returns a counting vector where variables are marginalized from N_xyz
      /** @param X_id the id of the variable to marginalize (this is the first
       * variable in table N_xyz
       * @param N_xyz a counting vector of dimension X * cond_vars (in this order)
       */
      std::vector< double, ALLOC< double > >
         _marginalize(const NodeId                                  X_id,
                      const std::vector< double, ALLOC< double > >& N_xyz) const;
    };

  } /* namespace learning */

} /* namespace gum */


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::learning::Score<>;
#endif


/// include the template implementation
#include <agrum/learning/scores_and_tests/score_tpl.h>

#endif /* GUM_LEARNING_SCORE_H */
