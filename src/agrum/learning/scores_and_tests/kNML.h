
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


/**
 * @file
 * @brief The class for the NML penalty used in 3off2
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_LEARNING_K_NML_H
#define GUM_LEARNING_K_NML_H

#include <vector>

#include <agrum/core/math/math.h>
#include <agrum/core/math/variableLog2ParamComplexity.h>
#include <agrum/learning/scores_and_tests/independenceTest.h>

namespace gum {

  namespace learning {


    /** @class KNML
     * @brief the class for computing the NML penalty used by 3off2
     * @ingroup learning_scores
     *
     */
    template < template < typename > class ALLOC = std::allocator >
    class KNML : private IndependenceTest< ALLOC > {
      public:
      /// type for the allocators passed in arguments of methods
      using allocator_type = ALLOC< NodeId >;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param parser the parser used to parse the database
       * @param apriori An apriori that we add to the computation of
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
       * Score.
       * @warning If nodeId2columns is not empty, then only the scores over the
       * ids belonging to this bijection can be computed: applying method
       * score() over other ids will raise exception NotFound. */
      KNML(const DBRowGeneratorParser< ALLOC >& parser,
           const Apriori< ALLOC >&              apriori,
           const std::vector< std::pair< std::size_t, std::size_t >,
                              ALLOC< std::pair< std::size_t, std::size_t > > >&
              ranges,
           const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
              nodeId2columns =
                 Bijection< NodeId, std::size_t, ALLOC< std::size_t > >(),
           const allocator_type& alloc = allocator_type());


      /// default constructor
      /** @param parser the parser used to parse the database
       * @param apriori An apriori that we add to the computation of
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
       * Score.
       * @warning If nodeId2columns is not empty, then only the scores over the
       * ids belonging to this bijection can be computed: applying method
       * score() over other ids will raise exception NotFound. */
      KNML(const DBRowGeneratorParser< ALLOC >& parser,
           const Apriori< ALLOC >&              apriori,
           const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
              nodeId2columns =
                 Bijection< NodeId, std::size_t, ALLOC< std::size_t > >(),
           const allocator_type& alloc = allocator_type());

      /// copy constructor
      KNML(const KNML< ALLOC >& from);

      /// copy constructor with a given allocator
      KNML(const KNML< ALLOC >& from, const allocator_type& alloc);

      /// move constructor
      KNML(KNML< ALLOC >&& from);

      /// move constructor with a given allocator
      KNML(KNML< ALLOC >&& from, const allocator_type& alloc);

      /// virtual copy constructor
      virtual KNML< ALLOC >* clone() const;

      /// virtual copy constructor with a given allocator
      virtual KNML< ALLOC >* clone(const allocator_type& alloc) const;

      /// destructor
      virtual ~KNML();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      KNML< ALLOC >& operator=(const KNML< ALLOC >& from);

      /// move operator
      KNML< ALLOC >& operator=(KNML< ALLOC >&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// changes the max number of threads used to parse the database
      using IndependenceTest< ALLOC >::setMaxNbThreads;

      /// returns the number of threads used to parse the database
      using IndependenceTest< ALLOC >::nbThreads;

      /** @brief changes the number min of rows a thread should process in a
       * multithreading context
       *
       * When computing score, several threads are used by record counters to
       * perform countings on the rows of the database, the MinNbRowsPerThread
       * method indicates how many rows each thread should at least process.
       * This is used to compute the number of threads actually run. This number
       * is equal to the min between the max number of threads allowed and the
       * number of records in the database divided by nb. */
      using IndependenceTest< ALLOC >::setMinNbRowsPerThread;

      /// returns the minimum of rows that each thread should process
      using IndependenceTest< ALLOC >::minNbRowsPerThread;

      /// sets new ranges to perform the countings used by kNML
      /** @param ranges a set of pairs {(X1,Y1),...,(Xn,Yn)} of database's rows
       * indices. The countings are then performed only on the union of the
       * rows [Xi,Yi), i in {1,...,n}. This is useful, e.g, when performing
       * cross validation tasks, in which part of the database should be ignored.
       * An empty set of ranges is equivalent to an interval [X,Y) ranging over
       * the whole database. */
      using IndependenceTest< ALLOC >::setRanges;

      /// reset the ranges to the one range corresponding to the whole database
      using IndependenceTest< ALLOC >::clearRanges;

      /// returns the current ranges
      using IndependenceTest< ALLOC >::ranges;

      /// the scores
      using IndependenceTest< ALLOC >::score;

      /// clears all the data structures from memory, including the C_n^r cache
      virtual void clear();

      /// clears the current C_n^r cache
      virtual void clearCache();

      /// turn on/off the use of the C_n^r cache
      virtual void useCache(const bool on_off);

      /// return the mapping between the columns of the database and the node ids
      /** @warning An empty nodeId2Columns bijection means that the mapping is
       * an identity, i.e., the value of a NodeId is equal to the index of the
       * column in the DatabaseTable. */
      using IndependenceTest< ALLOC >::nodeId2Columns;

      /// return the database used by the score
      using IndependenceTest< ALLOC >::database;

      /// returns the allocator used by the score
      using IndependenceTest< ALLOC >::getAllocator;

      /// @}


      protected:
      /// returns the score for a given IdSet
      /** @throws OperationNotAllowed is raised if the score does not support
       * calling method score such an idset (due to too many/too few variables
       * in the left hand side or the right hand side of the idset). */
      virtual double _score(const IdSet< ALLOC >& idset) final;


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      /// the CTable computation
      VariableLog2ParamComplexity< ALLOC > __param_complexity;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */


// always include the template implementation
#include <agrum/learning/scores_and_tests/kNML_tpl.h>

#endif /* GUM_LEARNING_K_NML_H */
