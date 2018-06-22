/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
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

#include <agrum/agrum.h>
#include <agrum/core/bijection.h>
#include <agrum/core/sequence.h>
#include <agrum/core/thread.h>
#include <agrum/core/threadData.h>
#include <agrum/graphs/DAG.h>
#include <agrum/learning/database/DBRowGeneratorParser.h>

namespace gum {

  namespace learning {

    /** @class RecordCounter
     * @brief The class that computes countings of observations from the database.
     * @headerfile recordCounter.h <agrum/learning/database/recordCounter.h>
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
     */
    template < template < typename > class ALLOC = std::allocator >
    class RecordCounter2 : private ALLOC<DBTranslatedValue>  {
    public:
      
      /// type for the allocators passed in arguments of methods
      using allocator_type = ALLOC<DBTranslatedValue>;
      
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
       * RecordCounter. */
      template < template < typename > class XALLOC > 
      RecordCounter2(const DBRowGeneratorParser<ALLOC>& parser,
                     const std::vector<std::pair<std::size_t,std::size_t>,
                           XALLOC<std::pair<std::size_t,std::size_t>>>& ranges,
                     const Bijection<NodeId,std::size_t>& nodeId2columns =
                     Bijection<NodeId,std::size_t> (),
                     const allocator_type& alloc = allocator_type ());

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
       * RecordCounter. */
      RecordCounter2(const DBRowGeneratorParser<ALLOC>& parser,
                     const Bijection<NodeId,std::size_t>& nodeId2columns =
                     Bijection<NodeId,std::size_t> (),
                     const allocator_type& alloc = allocator_type ());

      /// copy constructor
      RecordCounter2(const RecordCounter2<ALLOC>& from);

      /// copy constructor with a given allocator
      RecordCounter2(const RecordCounter2<ALLOC>& from,
                     const allocator_type& alloc);

      /// move constructor
      RecordCounter2(RecordCounter2<ALLOC>&& from);

      /// move constructor with a given allocator
      RecordCounter2(RecordCounter2<ALLOC>&& from,
                     const allocator_type& alloc);

      /// virtual copy constructor
      virtual RecordCounter2<ALLOC>* clone () const;

      /// virtual copy constructor with a given allocator
      virtual RecordCounter2<ALLOC>*
      clone (const allocator_type& alloc) const;

      /// destructor
      ~RecordCounter2();

      /// @}

      
      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      RecordCounter2<ALLOC>&
      operator=(const RecordCounter2<ALLOC>& from );

      /// move operator
      RecordCounter2<ALLOC>&
      operator=(RecordCounter2<ALLOC>&& from );

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
      std::size_t nbThreads () const;

      /** @brief changes the number min of rows a thread should process in a
       * multithreading context
       *
       * When Method counts executes several threads to perform countings on the
       * rows of the database, the MinNbRowsPerThread indicates how many rows each
       * thread should at least process. This is used to compute the number of
       * threads actually run. This number is equal to the min between the max
       * number of threads allowed and the number of records in the database
       * divided by nb. */
      void setMinNbRowsPerThread ( const std::size_t nb ) const;

      /// returns the minimum of rows that each thread should process
      std::size_t minNbRowsPerThread () const;
 
      /// returns the counts for a given set of nodes
      const std::vector< double, ALLOC<double> >&
      counts( const Sequence<NodeId>& ids ) const;

      /// @}

    private:
  
      // the parsers used by the threads
      std::vector<thread::ThreadData<DBRowGeneratorParser<ALLOC>>,
                  ALLOC<thread::ThreadData<DBRowGeneratorParser<ALLOC>>> >
      __parsers;

      // the set of ranges of the database's rows indices over which we
      // perform the countings
      std::vector<std::pair<std::size_t,std::size_t>,
                  ALLOC<std::pair<std::size_t,std::size_t>>> __ranges;

      // the mapping from the NodeIds of the variables to the indices of the
      // columns in the database
      Bijection<NodeId,std::size_t> __nodeId2columns;

      // the last database-parsed countings
      std::vector<double,ALLOC<double>> __last_DB_countings;

      // the ids of the nodes for the last database-parsed countings
      Sequence<NodeId> __last_DB_ids;

      // the last countings deduced from __last_DB_countings
      std::vector<double,ALLOC<double>> __last_nonDB_countings;

      // the ids of the nodes of last countings deduced from __last_DB_countings
      Sequence<NodeId> __last_nonDB_ids;

      // the maximal number of threads that the database can use
      mutable std::size_t __max_nb_threads
      {std::size_t(thread::getMaxNumberOfThreads())};

      // the min number of rows that a thread should process in a
      // multithreading context
      mutable std::size_t __min_nb_rows_per_thread{100};
 

      /// indicates whether a first set of ids is contained in the second set
      bool __isSubset (
           const Sequence<NodeId>& ids1,
           const Sequence<NodeId>& ids2,
           const std::vector<double,ALLOC<double>>& superset_vect ) const;

      // returns a mapping from the nodes ids to the columns of the database
      // for a given sequence of ids. This is especially convenient when
      // __nodeId2columns is empty (which means that there is an identity mapping)
      HashTable<NodeId, std::size_t>
      __getNodeIds2Columns ( const Sequence<NodeId>& ids ) const;

      /// extracts some new countings from previously computed ones
      std::vector< double, ALLOC<double> >& __extractFromCountings (
           const Sequence<NodeId>& subset_ids,
           const Sequence<NodeId>& superset_ids,
           const std::vector<double,ALLOC<double>>& superset_vect);

      /// parse the database to produce new countings
      std::vector< double, ALLOC<double> >&
      __countFromDatabase ( const Sequence<NodeId>& ids );
      
    };

  } /* namespace learning */

} /* namespace gum */

/// always include the templated implementations
#include <agrum/learning/scores_and_tests/recordCounter2_tpl.h>

#endif /* GUM_LEARNING_RECORD_COUNTER_H */

