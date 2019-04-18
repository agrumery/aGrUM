
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
 * @brief The class that computes countings of observations from the database.
 *
 * This class is the one to be called by scores and independence tests to
 * compute countings of observations from tabular databases.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_RECORD_COUNTER_H
#define GUM_LEARNING_RECORD_COUNTER_H

#include <vector>
#include <utility>
#include <sstream>
#include <string>

#include <agrum/agrum.h>
#include <agrum/core/bijection.h>
#include <agrum/core/sequence.h>
#include <agrum/core/OMPThreads.h>
#include <agrum/core/threadData.h>
#include <agrum/graphs/DAG.h>
#include <agrum/learning/database/DBRowGeneratorParser.h>
#include <agrum/learning/scores_and_tests/idSet.h>


namespace gum {

  namespace learning {

    /** @class RecordCounter
     * @brief The class that computes countings of observations from the database.
     * @headerfile recordCounter.h <agrum/learning/scores_and_tests/recordCounter.h>
     * @ingroup learning_scores
     *
     * This class is the one to be called by scores and independence tests to
     * compute the countings of observations from tabular datasets they need.
     * The countings are performed the following way:
     * when asked for the countings over a set X = {X_1,...,X_n} of
     * variables, the RecordCounter first checks whether it already contains
     * some countings over a set Y of variables containing X. If this is the
     * case, then it extracts from the countings over Y those over X (this is
     * usually way faster than determining the countings by parsing the database).
     * Otherwise, it determines the countings over X by parsing in a parallel
     * way the database. Only the result of the last database-parsed countings
     * is available for the subset counting determination. As an example, if
     * we create a RecordCounter and ask it the countings over {A,B,C}, it will
     * parse the database and provide the countings. Then, if we ask it countings
     * over B, it will use the table over {A,B,C} to produce the countings we
     * look for. Then, asking for countings over {A,C} will be performed the same
     * way. Now, asking countings over {B,C,D} will require another database
     * parsing. Finally, if we ask for countings over A, a new database parsing
     * will be performed because only the countings over {B,C,D} are now contained
     * in the RecordCounter.
     *
     * @par Here is an example of how to use the RecordCounter class:
     * @code
     * // here, write the code to construct your database, e.g.:
     * gum::learning::DBInitializerFromCSV<> initializer( "file.csv" );
     * const auto& var_names = initializer.variableNames();
     * const std::size_t nb_vars = var_names.size();
     * gum::learning::DBTranslatorSet<> translator_set;
     * gum::learning::DBTranslator4ContinuousVariable<> translator;
     * for (std::size_t i = 0; i < nb_vars; ++i) {
     *   translator_set.insertTranslator(translator, i);
     * }
     * gum::learning::DatabaseTable<> database(translator_set);
     *
     * // create the parser of the database
     * gum::learning::DBRowGeneratorSet<> genset;
     * gum::learning::DBRowGeneratorParser<> parser(database.handler(), genset);
     *
     * // create the record counter
     * gum::learning::RecordCounter<> counter(parser);
     *
     * // get the counts:
     * gum::learning::IdSet<> ids ( 0, gum::vector<gum::NodeId> {2,1} );
     * const std::vector< double >& counts1 = counter.counts ( ids );
     *
     * // change the rows from which we compute the counts:
     * // they should now be made on rows [500,600) U [1050,1125) U [100,150)
     * std::vector<std::pair<std::size_t,std::size_t>> new_ranges
     *   { std::pair<std::size_t,std::size_t>(500,600),
     *     std::pair<std::size_t,std::size_t>(1050,1125),
     *     std::pair<std::size_t,std::size_t>(100,150) };
     * counter.setRanges ( new_ranges );
     * const std::vector< double >& counts2 = counter.counts ( ids );
     * @endcode
     */
    template < template < typename > class ALLOC = std::allocator >
    class RecordCounter {
      public:
      /// type for the allocators passed in arguments of methods
      using allocator_type = ALLOC< NodeId >;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param parser the parser used to parse the database
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
       * RecordCounter.
       * @warning If nodeId2columns is not empty, then only the counts over the
       * ids belonging to this bijection can be computed: applying method
       * counts() over other ids will raise exception NotFound. */
      RecordCounter(
         const DBRowGeneratorParser< ALLOC >& parser,
         const std::vector< std::pair< std::size_t, std::size_t >,
                            ALLOC< std::pair< std::size_t, std::size_t > > >&
            ranges,
         const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
            nodeId2columns =
               Bijection< NodeId, std::size_t, ALLOC< std::size_t > >(),
         const allocator_type& alloc = allocator_type());

      /// default constructor
      /** @param parser the parser used to parse the database
       * @param nodeId2Columns a mapping from the ids of the nodes in the
       * graphical model to the corresponding column in the DatabaseTable
       * parsed by the parser. This enables estimating from a database in
       * which variable A corresponds to the 2nd column the parameters of a BN
       * in which variable A has a NodeId of 5. An empty nodeId2Columns
       * bijection means that the mapping is an identity, i.e., the value of a
       * NodeId is equal to the index of the column in the DatabaseTable.
       * @param alloc the allocator used to allocate the structures within the
       * RecordCounter.
       * @warning If nodeId2columns is not empty, then only the counts over the
       * ids belonging to this bijection can be computed: applying method
       * counts() over other ids will raise exception NotFound. */
      RecordCounter(const DBRowGeneratorParser< ALLOC >& parser,
                    const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                       nodeId2columns =
                          Bijection< NodeId, std::size_t, ALLOC< std::size_t > >(),
                    const allocator_type& alloc = allocator_type());

      /// copy constructor
      RecordCounter(const RecordCounter< ALLOC >& from);

      /// copy constructor with a given allocator
      RecordCounter(const RecordCounter< ALLOC >& from,
                    const allocator_type&         alloc);

      /// move constructor
      RecordCounter(RecordCounter< ALLOC >&& from);

      /// move constructor with a given allocator
      RecordCounter(RecordCounter< ALLOC >&& from, const allocator_type& alloc);

      /// virtual copy constructor
      virtual RecordCounter< ALLOC >* clone() const;

      /// virtual copy constructor with a given allocator
      virtual RecordCounter< ALLOC >* clone(const allocator_type& alloc) const;

      /// destructor
      virtual ~RecordCounter();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      RecordCounter< ALLOC >& operator=(const RecordCounter< ALLOC >& from);

      /// move operator
      RecordCounter< ALLOC >& operator=(RecordCounter< ALLOC >&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// clears all the last database-parsed countings from memory
      void clear();

      /// changes the max number of threads used to parse the database
      void setMaxNbThreads(const std::size_t nb) const;

      /// returns the number of threads used to parse the database
      std::size_t nbThreads() const;

      /** @brief changes the number min of rows a thread should process in a
       * multithreading context
       *
       * When Method counts executes several threads to perform countings on the
       * rows of the database, the MinNbRowsPerThread indicates how many rows each
       * thread should at least process. This is used to compute the number of
       * threads actually run. This number is equal to the min between the max
       * number of threads allowed and the number of records in the database
       * divided by nb. */
      void setMinNbRowsPerThread(const std::size_t nb) const;

      /// returns the minimum of rows that each thread should process
      std::size_t minNbRowsPerThread() const;

      /// returns the counts over all the variables in an IdSet
      /** @param ids the idset of the variables over which we perform countings.
       * @param check_discrete_vars The record counter can only produce correct
       * results on sets of discrete variables. By default, the method does not
       * check whether the variables corresponding to the IdSet are actually
       * discrete. If check_discrete_vars is set to true, then this check is
       * performed before computing the counting vector. In this case, if a
       * variable is not discrete, a TypeError exception is raised.
       * @return a vector containing the multidimensional contingency table
       * over all the variables corresponding to the ids passed in argument
       * (both at the left hand side and right hand side of the conditioning
       * bar of the IdSet). The first dimension is that of the first variable in
       * the IdSet, i.e., when its value increases by 1, the offset in the
       * output vector also increases by 1. The second dimension is that of the
       * second variable in the IdSet, i.e., when its value increases by 1, the
       * offset in the ouput vector increases by the domain size of the first
       * variable. For the third variable, the offset corresponds to the product
       * of the domain sizes of the first two variables, and so on.
       * @warning The vector returned by the function may differ from one
       * call to another. So, care must be taken. E,g. a code like:
       * @code
       * const std::vector< double, ALLOC<double> >&
       * counts = counter.counts(ids);
       * counts = counter.counts(other_ids);
       * @endcode
       * may be erroneous because the two calls to method counts() may
       * return references to different vectors. The correct way of using method
       * counts() is always to call it declaring a new reference variable:
       * @code
       * const std::vector< double, ALLOC<double> >& counts =
       *   counter.counts(ids);
       * const std::vector< double, ALLOC<double> >& other_counts =
       *   counter.counts(other_ids);
       * @endcode
       * @throw TypeError is raised if check_discrete_vars is set to true (i.e.,
       * we check that all variables in the IdSet are discrete) and if at least
       * one variable is not of a discrete nature.
       */
      const std::vector< double, ALLOC< double > >&
         counts(const IdSet< ALLOC >& ids, const bool check_discrete_vars = false);

      /// sets new ranges to perform the countings
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

      /// assign a new Bayes net to all the counter's generators depending on a BN
      /** Typically, generators based on EM or K-means depend on a model to
       * compute correctly their outputs. Method setBayesNet enables to
       * update their BN model. */
      template < typename GUM_SCALAR >
      void setBayesNet(const BayesNet< GUM_SCALAR >& new_bn);

      /// returns the allocator used
      allocator_type getAllocator() const;

      /// returns the mapping from ids to column positions in the database
      /** @warning An empty nodeId2Columns bijection means that the mapping is
       * an identity, i.e., the value of a NodeId is equal to the index of the
       * column in the DatabaseTable. */
      const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
         nodeId2Columns() const;

      /// returns the database on which we perform the counts
      const DatabaseTable< ALLOC >& database() const;

      /// @}


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      // the parsers used by the threads
      std::vector< ThreadData< DBRowGeneratorParser< ALLOC > >,
                   ALLOC< ThreadData< DBRowGeneratorParser< ALLOC > > > >
         __parsers;

      // the set of ranges of the database's rows indices over which the user
      // wishes to perform the countings
      std::vector< std::pair< std::size_t, std::size_t >,
                   ALLOC< std::pair< std::size_t, std::size_t > > >
         __ranges;

      // the ranges actually used by the threads: there is a hopefully clever
      // algorithm that split the rows ranges into another set of ranges that
      // are assigned to the threads. For instance, if the database has 1000
      // rows and there are 10 threads, each one will be assed a set of 100
      // rows. These sets are precisely what are stored in the field below
      mutable std::vector< std::pair< std::size_t, std::size_t >,
                           ALLOC< std::pair< std::size_t, std::size_t > > >
         __thread_ranges;

      // the mapping from the NodeIds of the variables to the indices of the
      // columns in the database
      Bijection< NodeId, std::size_t, ALLOC< std::size_t > > __nodeId2columns;

      // the last database-parsed countings
      std::vector< double, ALLOC< double > > __last_DB_countings;

      // the ids of the nodes for the last database-parsed countings
      IdSet< ALLOC > __last_DB_ids;

      // the last countings deduced from __last_DB_countings
      std::vector< double, ALLOC< double > > __last_nonDB_countings;

      // the ids of the nodes of last countings deduced from __last_DB_countings
      IdSet< ALLOC > __last_nonDB_ids;

      // the maximal number of threads that the record counter can use
      mutable std::size_t __max_nb_threads{
         std::size_t(gum::getMaxNumberOfThreads())};

      // the min number of rows that a thread should process in a
      // multithreading context
      mutable std::size_t __min_nb_rows_per_thread{100};

      // returns a mapping from the nodes ids to the columns of the database
      // for a given sequence of ids. This is especially convenient when
      // __nodeId2columns is empty (which means that there is an identity mapping)
      HashTable< NodeId, std::size_t >
         __getNodeIds2Columns(const IdSet< ALLOC >& ids) const;

      /// extracts some new countings from previously computed ones
      std::vector< double, ALLOC< double > >& __extractFromCountings(
         const IdSet< ALLOC >&                         subset_ids,
         const IdSet< ALLOC >&                         superset_ids,
         const std::vector< double, ALLOC< double > >& superset_vect);

      /// parse the database to produce new countings
      std::vector< double, ALLOC< double > >&
         __countFromDatabase(const IdSet< ALLOC >& ids);

      /// the method used by threads to produce countings by parsing the database
      void __threadedCount(
         const std::size_t              range_begin,
         const std::size_t              range_end,
         DBRowGeneratorParser< ALLOC >& parser,
         const std::vector< std::pair< std::size_t, std::size_t >,
                            ALLOC< std::pair< std::size_t, std::size_t > > >&
                                                 cols_and_offsets,
         std::vector< double, ALLOC< double > >& countings);

      /// checks that the ranges passed in argument are ok or raise an exception
      /** A range is ok if its upper bound is strictly higher than its lower
       * bound and the latter is also lower than or equal to the number of rows
       * in the database. */
      template < template < typename > class XALLOC >
      void __checkRanges(
         const std::vector< std::pair< std::size_t, std::size_t >,
                            XALLOC< std::pair< std::size_t, std::size_t > > >&
            new_ranges) const;

      /// check that the variables at indices [beg,end) of an idset are discrete
      /** @throw TypeError is raised if at least one variable in ids is
       * of a continuous nature. */
      void __checkDiscreteVariables(const IdSet< ALLOC >& ids) const;

      /// compute and raise the exception when some variables are continuous
      /** This method is used by __checkDiscreteVariables to determine the
       * appropriate message to include in the TypeError exception raised when
       * some variables over which we should perform countings are continuous. */
      void __raiseCheckException(
         const std::vector< std::string, ALLOC< std::string > >& bad_vars) const;

      /// sets the ranges within which each thread will perform its computations
      void __dispatchRangesToThreads();

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */

/// always include the templated implementations
#include <agrum/learning/scores_and_tests/recordCounter_tpl.h>

#endif /* GUM_LEARNING_RECORD_COUNTER_H */
