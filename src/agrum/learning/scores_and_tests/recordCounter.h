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
 * compute countings of observations from tabular databases. It calls
 * as many RecordCounterThreads as possible to do the job in parallel.
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
#include <agrum/core/bijection.h>
#include <agrum/core/OMPThreads.h>
#include <agrum/graphs/DAG.h>
#include <agrum/learning/database/DBRowFilter.h>
#include <agrum/learning/scores_and_tests/idSet.h>

namespace gum {

  namespace learning {

    template <typename IdSetAlloc, typename CountAlloc> class Counter;

    /* ========================================================================= */
    /* ===                      THREAD RECORD COUNTER BASE                   === */
    /* ========================================================================= */
    /** @class RecordCounterThreadBase
     * @brief The database-independent class for preparing the computation of the
     * number of observations of tuples of variables in a database
     *
     * A RecordCounterThreadBase just prepares structures for the computation of
     * the number of records observed for each value of its target nodes in the
     * database (a target node usually represents either a single Discrete
     * Variable or a pair of Discrete Variables). It is not intended to be used as
     * is but rather to be created by class RecordCounterThread.
     */
    template <typename IdSetAlloc = std::allocator<unsigned int>,
              typename CountAlloc = std::allocator<float>>
    class RecordCounterThreadBase {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      RecordCounterThreadBase(const std::vector<unsigned int> &var_modalities);

      /// copy constructor
      RecordCounterThreadBase(
          const RecordCounterThreadBase<IdSetAlloc, CountAlloc> &from);

      /// move operator
      RecordCounterThreadBase(
          RecordCounterThreadBase<IdSetAlloc, CountAlloc> &&from);

      /// virtual copy constructor
      virtual RecordCounterThreadBase<IdSetAlloc, CountAlloc> *
      copyFactory() const = 0;

      /// destructor
      virtual ~RecordCounterThreadBase();

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// adds a new target nodeset to be counted
      /** @return the id of the nodeset counter created */
      unsigned int addNodeSet(const std::vector<unsigned int, IdSetAlloc> &ids);

      /// remove all the current target nodesets
      void clearNodeSets() noexcept;

      /// update all the countings of all the nodesets by parsing the database
      virtual void count() = 0;

      /// returns the size of the database
      virtual unsigned long DBSize() noexcept = 0;

      /// sets the interval of records on which countings should be performed
      virtual void setRange(unsigned long min_index, unsigned long max_index) = 0;

      /// returns the countings for the nodeset specified in argument
      const std::vector<float, CountAlloc> &getCounts(unsigned int nodeset_id) const
          noexcept;

      /// @}

      protected:
      /// the modalities of the variables
      const std::vector<unsigned int> *_modalities{nullptr};

      /// the nodesets whose observations will be counted
      std::vector<const std::vector<unsigned int, IdSetAlloc> *> _nodesets;

      /// the nodesets countings
      std::vector<std::vector<float, CountAlloc>> _countings;

      /// the size of the cache used to prevent cacheline omp parallel problems
      static constexpr unsigned int _cache_size{128};

      /// used to prevent cacheline omp parallel problems
      const char _align[_cache_size]{};
    };

    /* ========================================================================= */
    /* ===                        THREAD RECORD COUNTER                      === */
    /* ========================================================================= */
    /** @class RecordCounterThread
     * @brief This class computes the number of observations of tuples of variables
     * in the database
     * A RecordCounterThread just computes the number of records observed for
     * each value of its target nodes in the database (a target node usually
     * represents either a single Discrete Variable or a pair of Discrete
     * Variables). It is not intended to be used as is but rather to be created
     * by class RecordCounter. When the latter has to create several threads to
     * parse the database, it does so by creating RecorCounterThreads.
     */
    template <typename RowFilter, typename IdSetAlloc = std::allocator<unsigned int>,
              typename CountAlloc = std::allocator<float>>
    class RecordCounterThread
        : public RecordCounterThreadBase<IdSetAlloc, CountAlloc> {

      using Base = RecordCounterThreadBase<IdSetAlloc, CountAlloc>;

      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      RecordCounterThread(const RowFilter &filter,
                          const std::vector<unsigned int> &var_modalities);

      /// copy constructor
      RecordCounterThread(
          const RecordCounterThread<RowFilter, IdSetAlloc, CountAlloc> &from);

      /// move operator
      RecordCounterThread(
          RecordCounterThread<RowFilter, IdSetAlloc, CountAlloc> &&from);

      /// virtual copy constructor
      virtual RecordCounterThread<RowFilter, IdSetAlloc, CountAlloc> *
      copyFactory() const;

      /// destructor
      virtual ~RecordCounterThread();

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// adds a new target nodeset to be counted
      /** @return the id of the nodeset counter created */
      using Base::addNodeSet;

      /// remove all the current target nodesets
      using Base::clearNodeSets;

      /// update all the countings of all the nodesets by parsing the database
      void count();

      /// returns the countings for the nodeset specified in argument
      using Base::getCounts;

      /// returns the size of the database
      unsigned long DBSize() noexcept;

      /// sets the interval of records on which countings should be performed
      void setRange(unsigned long min_index, unsigned long max_index);

      /// returns the filter used for the countings
      RowFilter &filter() noexcept;

      /// @}

      private:
      /// the DBRowFilter used to parse the database
      RowFilter __filter;
    };

    /* ========================================================================= */
    /* ===                            RECORD COUNTER                         === */
    /* ========================================================================= */
    /** @class RecordCounter
     * @ingroup learning_group
     * @brief The class that computes countings of observations from the database.
     *
     * This class is the one to be called by scores and independence tests to
     * compute countings of observations from tabular databases. It calls
     * as many RecordCounterThreads as possible to do the job in parallel.
     */
    template <typename IdSetAlloc = std::allocator<unsigned int>,
              typename CountAlloc = std::allocator<float>>
    class RecordCounter {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      template <typename RowFilter>
      RecordCounter(const RowFilter &filter,
                    const std::vector<unsigned int> &var_modalities);

      /// copy constructor
      RecordCounter(const RecordCounter<IdSetAlloc, CountAlloc> &from);

      /// move constructor
      RecordCounter(RecordCounter<IdSetAlloc, CountAlloc> &&from);

      /// destructor
      ~RecordCounter();

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// add a new nodeset to count
      unsigned int addNodeSet(const std::vector<unsigned int, IdSetAlloc> &ids);

      /// returns the size of the database
      unsigned long DBSize() noexcept;

      /// performs countings from the database by cutting it into several pieces
      /** This method implements a parallel counting strategy which consists of
       * cutting the database into a set of more or less equal-size pieces and
       * to call one RecordCounterThread for each such piece. The latter then
       * perform countings for all the sets of ids non-included into other sets
       * (i.e., proper supersets). When all the database has been parsed, the
       * countings are aggregated to result in countings over the whole
       * database. */
      void countOnSubDatabase();

      /// performs the countings of the ids' subsets from those of their supersets
      void countSubsets();

      /// perform the countings of all the sets of ids
      /** This method selects the most appropriate parallel counting method and
       * performs it. */
      void count();

      /// returns the counts performed for a given idSet
      const std::vector<float, CountAlloc> &getCounts(unsigned int idset) const
          noexcept;

      /// resets the counter, i.e., remove all its sets of ids and counting vectors
      void clearNodeSets() noexcept;

      /// returns the modalities of the variables in the database
      const std::vector<unsigned int> &modalities() const;

      /// sets the maximum number of threads used to perform countings
      void setMaxNbThreads(unsigned int nb) noexcept;

      /// @}

      private:
      template <typename I, typename C> friend class Counter;

      /// the modalities of the variables
      const std::vector<unsigned int> *__modalities{nullptr};

      /// the set of ordered vectors of ids + their indices in __nodesets
      /** The goal of this structure is essentially to store the vectors of
       * ordered ids as IdSet<> in such a way that the memory locations of these
       * idsets never change, even if we add new IdSets (this feature is
       * guaranteed by aGrUM's hashtables). In addition, it allows to easily
       * detect identical IdSets (such detection allows for fast computations).
       * As such, the indices stored as values in the hashtable are the indices
       * in __nodesets ONLY of one copy of identical sets, the others are
       * deduced from it. IdSets are used to quickly determine which sets are
       * included into others. */
      Bijection<IdSet<IdSetAlloc>, unsigned int> __idsets;

      /// the vector of the unordered ids' vectors used to generate the idsets
      /** When the user add nodes (i.e., vectors of ids), those are unordered
       * and will be processed (counted) as such. However, in order to determine
       * which vectors of ids are contained into other vectors, we create IdSets
       * from them (those are ordered vectors that will enable fast subset
       * computations) */
      std::vector<const std::vector<unsigned int, IdSetAlloc> *> __nodesets;

      /// a table associating to each variable the IdSets that contain it
      /** This table is used to quickly compute the IdSets that are contained
       * in other IdSets */
      HashTable<unsigned int, std::vector<const IdSet<IdSetAlloc> *>> __var2idsets;

      /// the possible states of a set of ids
      enum SetState {
        NOT_SUBSET,    // this is a proper nonempty superset
        STRICT_SUBSET, // the set is included into another one
        COPY_SET,      // this set is a copy of another one
        EMPTY_SET      // the set is empty
      };

      /// a table indicating whether each IdSet is a subset of another idSet
      std::vector<SetState> __set_state;

      /// a vector for computing the countings of the IdSets which are subsets
      /** These countings are derived from the countings of the supersets */
      std::vector<std::vector<float, CountAlloc>> __countings;

      /// a hashtable associating to each IdSet its index in __set2thread_id
      /** This table indicates for each @e distinct set its index in
       * __set2thread_id or, equivalently, in __nodesets. By @e distinct set,
       * we mean that, when several sets are identical, only the first one
       * inserted by addNodeSet is put into the hashtable. This table is
       * used as a helper in __computeSubsets to determine quickly the
       * indices of supersets in __nodesets (the content of __idset2index is
       * similar to that of __idsets, except that it is faster to search
       * within hashtables pointer to idsets rather than idsets themselves. */
      HashTable<const IdSet<IdSetAlloc> *, unsigned int> __idset2index;

      /// a table associating to each IdSet its index in the threadRecordCounters
      /** For the IdSets which are subsets of other IdSets, the index corresponds
       * to that of its superset in __set2thread_id */
      std::vector<std::pair<const IdSet<IdSetAlloc> *, unsigned int>>
          __set2thread_id;

      /// a partial lattice indicating the relations between subsets and supersets
      /** In this lattice, an arc X -> Y indicates that X is a superset of Y. As
       * such, we should perform countings of X before deducing those of Y. */
      DAG __subset_lattice;

      /// the set of ThreadCounters
      std::vector<RecordCounterThreadBase<IdSetAlloc, CountAlloc> *>
          __thread_counters;

      /// the number of thread counter used by the last count ()
      unsigned int __nb_thread_counters{0};

/// the max number of threads authorized
#if defined(_OPENMP) && defined(NDEBUG)
      unsigned int __max_threads_number{getMaxNumberOfThreads()};
#else
      unsigned int __max_threads_number{1};
#endif /* NDEBUG */

      /// the minimal number of rows to parse (on average) by thread
      unsigned int __min_nb_rows_per_thread{100};

      /// determine which sets are subsets
      void __computeSubsets();

      // computes the countings of one subset from those of its superset
      void __countOneSubset(unsigned int i);

      /// returns the counting performed
      std::vector<std::vector<float, CountAlloc>> &__getCounts() noexcept;
    };

  } /* namespace learning */

} /* namespace gum */

// always include the template implementation
#include <agrum/learning/scores_and_tests/recordCounter.tcc>

#endif /* GUM_LEARNING_RECORD_COUNTER_H */
