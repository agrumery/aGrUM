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
 * @brief A class used by RecordCounter to detect subsets of variables
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_RECORD_COUNTER_H
#define GUM_LEARNING_RECORD_COUNTER_H

#include <type_traits>
#include <vector>
#include <cstring>
#include <algorithm>
#include <initializer_list>
#include <climits>

#include <agrum/core/hashTable.h>
#include <agrum/core/OMPThreads.h>
#include <agrum/learning/DBRowFilter.h>
#include <agrum/learning/idSet.h>


// a buffer used to ensure that thread counters will have no cacheline problems
#define CACHE_SIZE 1024


namespace gum {

  
  namespace learning {


    /* ========================================================================= */
    /* ===                        THREAD RECORD COUNTER                      === */
    /* ========================================================================= */
    /** @class RecordCounterThread
     * @ingroup learning_group
     * @brief This class computes the number of observations of tuples of variables
     * in the database
     * A RecordCounterThread just computes the number of records observed for
     * each value of its target nodes in the database (a target node usually
     * represents either a single Discrete Variable or a pair of Discrete
     * Variables). It is not intended to be used as is but rather to be created
     * by class RecordCounter. When the latter has to create several threads to
     * parse the database, it does so by creating RecorCounterThreads.
     */
    template <typename RowFilter,
              typename IdSetAlloc = std::allocator<unsigned int>,
              typename CountAlloc = std::allocator<float> >
    class RecordCounterThread {      
    public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{
      
      /// default constructor
      RecordCounterThread ( const RowFilter& filter,
                            const std::vector<unsigned int>& var_modalities );

      /// copy constructor
      RecordCounterThread
      ( const RecordCounterThread<RowFilter,IdSetAlloc,CountAlloc>& from );

      /// move operator
      RecordCounterThread
      ( RecordCounterThread<RowFilter,IdSetAlloc,CountAlloc>&& from );

      /// destructor
      ~RecordCounterThread ();
     
      /// @}

      
      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// adds a new target nodeset to be counted
      /** @return the id of the nodeset counter created */
      unsigned int addNodeSet ( const std::vector<unsigned int,IdSetAlloc>& ids );

      /// remove all the current target nodesets
      void clearNodeSets () noexcept;

      /// update all the countings of all the nodesets by parsing the database
      void count ();

      /// returns the countings for the nodeset specified in argument
      const std::vector<float,CountAlloc>&
      getCounts ( unsigned int nodeset_id ) const noexcept;
      
      /// returns the filter used for the countings
      RowFilter& filter () noexcept;

      /// @}


    private:

      /// the modalities of the variables
      const std::vector<unsigned int>* __modalities { nullptr };

      /// the nodesets whose observations will be counted
      std::vector< const std::vector<unsigned int,IdSetAlloc>* > __nodesets;

      /// the DBRowFilter used to parse the database
      RowFilter __filter;

      /// the nodesets countings 
      std::vector< std::vector<float,CountAlloc> > __countings;

      /// used to prevent cacheline omp parallel problems
      char __align[CACHE_SIZE];

    };


      




    
    /* ========================================================================= */
    /* ===                            RECORD COUNTER                         === */
    /* ========================================================================= */
    /** @class RecordCounter
     * @ingroup learning_group
     * @brief The class that computes countings of observations from the database.
     *
     * This calls is the one to be called by scores and independence tests to
     * compute countings of observations from tabular databases. It calls
     * as many RecordCounterThreads as possible to do the job in parallel. 
     */
    template <typename RowFilter,
              typename IdSetAlloc = std::allocator<unsigned int>,
              typename CountAlloc = std::allocator<float> >
    class RecordCounter {
    public:
      
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      RecordCounter ( const RowFilter& filter,
                      const std::vector<unsigned int>& var_modalities );

      /// copy constructor
      RecordCounter ( const RecordCounter<RowFilter,IdSetAlloc,CountAlloc>& from );

      /// move constructor
      RecordCounter ( RecordCounter<RowFilter,IdSetAlloc,CountAlloc>&& from );

      /// destructor
      ~RecordCounter ();

      /// @}

      
      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// add a new nodeset to count
      unsigned int addNodeSet ( const std::vector<unsigned int,IdSetAlloc>& ids );

      /// performs countings from the database by cutting it into several pieces
      /** This method implements a parallel counting strategy which consists of
       * cutting the database into a set of more or less equal-size pieces and
       * to call one RecordThreadCounter for each such piece. The latter then
       * perform countings for all the non-included sets of ids. When all the
       * database has been parsed, the countings are aggregated to result in
       * countings over the whole database. */
      void countOnSubDatabase ();

      /// performs the countings of the ids' subsets from those of their supersets
      void countSubsets ();

      /// perform the countings of all the sets of ids
      /** This method selects the most appropriate parallel counting method and
       * performs it. */
      void count ();

      /// returns the filter used for the countings
      RowFilter& filter () noexcept;
      
      /// returns the counts performed for a given idSet
      const std::vector<float,CountAlloc>&
      getCounts ( unsigned int idset ) const noexcept;
      
      /// resets the counter, i.e., remove all its sets of ids and counting vectors
      void clearNodeSets () noexcept;

      /// returns the modalities of the variables in the database
      const std::vector<unsigned int>& modalities () const;

      /// @}
      

    private:
      /// the row filter used to filter the database
      RowFilter __row_filter;
      
      /// the modalities of the variables
      const std::vector<unsigned int>* __modalities { nullptr };

      /// the set of ordered vectors of ids
      HashTable<IdSet<IdSetAlloc>, unsigned int> __idsets;
      
      /// the vector of the unordered ids' vectors used to generate the idsets
      /** When the user add nodes (i.e., vectors of ids), those are unordered
       * and will be processed (counted) as such. However, in order to determine
       * which vectors of ids are contained into other vectors, we create IdSets
       * from them (those are ordered vectors that will enable fast subset
       * computations) */
      std::vector<const std::vector<unsigned int,IdSetAlloc>* > __nodesets; 

      /// a table associating to each variable the IdSets that contain it
      /** This table is used to quickly compute the IdSets that are contained
       * in other IdSets */
      HashTable<unsigned int, std::vector<const IdSet<IdSetAlloc>*> >
      __var2idsets;

      /// a table indicating whether each IdSet is a subset of another idSet
      std::vector<bool> __is_id_subset;

      /// a vector for computing the countings of the IdSets which are subsets
      /** These countings are derived from the countings of the supersets */
      std::vector< std::vector<float,CountAlloc> > __countings;

      /// a hashtable associating to each IdSet its index in __set2thread_id
      HashTable<const IdSet<IdSetAlloc>*, unsigned int> __idset2index;

      /// a table associating to each IdSet its index in the threadRecordCounters
      /** For the IdSets which are subsets of other IdSets, the index corresponds
       * to that of its superset in __set2thread_id */
      std::vector<std::pair<const IdSet<IdSetAlloc>*, unsigned int> >
      __set2thread_id;

      /// the set of ThreadCounters
      std::vector< RecordCounterThread<RowFilter,IdSetAlloc,CountAlloc> >
      __thread_counters;

      /// the number of thread counter used by the last count ()
      unsigned int __nb_thread_counters { 0 };
      


      /// determine which sets are subsets
      void __computeSubsets ();
      
      /// the function used to sort vectors of IdSets by increasing size order
      static bool __sortIdSetBySize ( const IdSet<IdSetAlloc>* set1,
                                      const IdSet<IdSetAlloc>* set2 );

    };


  } /* namespace learning */

  
} /* namespace gum */


// always include the template implementation
#include <agrum/learning/recordCounter.tcc>


#endif /* GUM_LEARNING_RECORD_COUNTER_H */

