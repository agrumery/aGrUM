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
 * @brief Template implementation of RecordCounters and RecordCounterThreads
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <agrum/core/set.h>

namespace gum {

  namespace learning {

    /// default constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE RecordCounterThreadBase<IdSetAlloc, CountAlloc>::RecordCounterThreadBase(
        const std::vector<unsigned int> &var_modalities)
        : _modalities(&var_modalities) {
      GUM_CONSTRUCTOR(RecordCounterThreadBase);
    }

    /// copy constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE RecordCounterThreadBase<IdSetAlloc, CountAlloc>::RecordCounterThreadBase(
        const RecordCounterThreadBase<IdSetAlloc, CountAlloc> &from)
        : _modalities(from._modalities) {
      GUM_CONS_CPY(RecordCounterThreadBase);
    }

    /// move operator
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE RecordCounterThreadBase<IdSetAlloc, CountAlloc>::RecordCounterThreadBase(
        RecordCounterThreadBase<IdSetAlloc, CountAlloc> &&from)
        : _modalities(from._modalities), _nodesets(std::move(from._nodesets)),
          _countings(std::move(from._countings)) {
      GUM_CONS_MOV(RecordCounterThreadBase);
    }

    /// destructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE
        RecordCounterThreadBase<IdSetAlloc, CountAlloc>::~RecordCounterThreadBase() {
      GUM_DESTRUCTOR(RecordCounterThreadBase);
    }

    /// adds a new target nodeset to be counted
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE unsigned int RecordCounterThreadBase<IdSetAlloc, CountAlloc>::addNodeSet(
        const std::vector<unsigned int, IdSetAlloc> &ids) {
      // adds the nodeset
      const unsigned int nodeset_id = _nodesets.size();
      _nodesets.push_back(&ids);

      // compute the size of the nodeset and allocate a counting correspondingly
      unsigned int size = 1;

      for (const auto id : ids) {
        size *= _modalities->operator[](id);
      }

      // allocate the counting set
      _countings.push_back(std::vector<float, CountAlloc>(size, 0));

      return nodeset_id;
    }

    /// remove all the current target nodesets
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE void
    RecordCounterThreadBase<IdSetAlloc, CountAlloc>::clearNodeSets() noexcept {
      _nodesets.clear();
      _countings.clear();
    }

    /// returns the countings for the nodeset specified in argument
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE const std::vector<float, CountAlloc> &
    RecordCounterThreadBase<IdSetAlloc, CountAlloc>::getCounts(
        unsigned int nodeset_id) const noexcept {
      return _countings[nodeset_id];
    }

    // ============================================================================

    /// default constructor
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    INLINE
    RecordCounterThread<RowFilter, IdSetAlloc, CountAlloc>::RecordCounterThread(
        const RowFilter &filter, const std::vector<unsigned int> &var_modalities)
        : Base(var_modalities), __filter(filter) {
      GUM_CONSTRUCTOR(RecordCounterThread);
    }

    /// copy constructor
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    INLINE
    RecordCounterThread<RowFilter, IdSetAlloc, CountAlloc>::RecordCounterThread(
        const RecordCounterThread<RowFilter, IdSetAlloc, CountAlloc> &from)
        : Base(from), __filter(from.__filter) {
      GUM_CONS_CPY(RecordCounterThread);
    }

    /// move operator
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    INLINE
    RecordCounterThread<RowFilter, IdSetAlloc, CountAlloc>::RecordCounterThread(
        RecordCounterThread<RowFilter, IdSetAlloc, CountAlloc> &&from)
        : Base(std::move(from)), __filter(std::move(from.__filter)) {
      GUM_CONS_MOV(RecordCounterThread);
    }

    /// virtual copy constructor
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    INLINE RecordCounterThread<RowFilter, IdSetAlloc, CountAlloc> *
    RecordCounterThread<RowFilter, IdSetAlloc, CountAlloc>::copyFactory() const {
      return new RecordCounterThread<RowFilter, IdSetAlloc, CountAlloc>(*this);
    }

    /// destructor
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    INLINE RecordCounterThread<RowFilter, IdSetAlloc,
                               CountAlloc>::~RecordCounterThread() {
      GUM_DESTRUCTOR(RecordCounterThread);
    }

    /// update all the countings of all the nodesets by parsing the database
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    INLINE void RecordCounterThread<RowFilter, IdSetAlloc, CountAlloc>::count() {
      __filter.reset();

      // get the number of nodesets to process
      const unsigned int size = this->_nodesets.size();

      try {
        while (__filter.hasRows()) {
          // get the observed filtered rows
          FilteredRow &row = __filter.row();

          // fill the counts for the ith nodeset
          for (unsigned int i = 0; i < size; ++i) {
            const std::vector<unsigned int> &var_ids = *(this->_nodesets[i]);
            unsigned int offset = 0;
            unsigned int dim = 1;

            for (unsigned int j = 0, vsize = var_ids.size(); j < vsize; ++j) {
              offset += row[var_ids[j]] * dim;
              dim *= this->_modalities->operator[](var_ids[j]);
            }

            this->_countings[i][offset] += row.weight();
          }
        }
      } catch (NotFound &) {
      } // this exception is raised by the row filter if the
        // row generators create no output row from the last
        // rows of the database
    }

    /// returns the filter used for the countings
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    INLINE RowFilter &
    RecordCounterThread<RowFilter, IdSetAlloc, CountAlloc>::filter() noexcept {
      return __filter;
    }

    /// returns the size of the database
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    INLINE unsigned long
    RecordCounterThread<RowFilter, IdSetAlloc, CountAlloc>::DBSize() noexcept {
      return __filter.handler().DBSize();
    }

    /// sets the interval of records on which countings should be performed
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    INLINE void RecordCounterThread<RowFilter, IdSetAlloc, CountAlloc>::setRange(
        unsigned long min_range, unsigned long max_range) {
      __filter.handler().setRange(min_range, max_range);
    }

    // ============================================================================

    /// default constructor
    template <typename IdSetAlloc, typename CountAlloc>
    template <typename RowFilter>
    INLINE RecordCounter<IdSetAlloc, CountAlloc>::RecordCounter(
        const RowFilter &filter, const std::vector<unsigned int> &var_modalities)
        : __modalities(&var_modalities) {
      // create a first recordCounter so that we can store the row filter
      // and apply subsequently virtual copy constructors to create other
      // thread counters
      __thread_counters.reserve(__max_threads_number);
      __thread_counters.push_back(
          new RecordCounterThread<RowFilter, IdSetAlloc, CountAlloc>(
              filter, var_modalities));

      GUM_CONSTRUCTOR(RecordCounter);
    }

    /// copy constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE RecordCounter<IdSetAlloc, CountAlloc>::RecordCounter(
        const RecordCounter<IdSetAlloc, CountAlloc> &from)
        : __modalities(from.__modalities), __idsets(from.__idsets),
          __nodesets(from.__nodesets), __set_state(from.__set_state),
          __countings(from.__countings), __subset_lattice(from.__subset_lattice),
          __nb_thread_counters(from.__nb_thread_counters) {
      // create the thread counters
      __thread_counters.reserve(from.__thread_counters.size());

      try {
        for (unsigned int i = 0, size = from.__thread_counters.size(); i < size;
             ++i) {
          __thread_counters.push_back(from.__thread_counters[i]->copyFactory());
        }
      } catch (...) {
        for (auto elt : __thread_counters)
          delete elt;

        throw;
      }

      // create the idsets2index
      for (auto iter = __idsets.begin(); iter != __idsets.end(); ++iter) {
        __idset2index.insert(&(iter.first()), iter.second());
      }

      // fill the set2thread_id
      __set2thread_id.reserve(from.__set2thread_id.size());
      for (const auto &thepair : from.__set2thread_id) {
        if (thepair.first != nullptr) {
          __set2thread_id.push_back(
              std::pair<const IdSet<IdSetAlloc> *, unsigned int>(
                  &(__idsets.first(thepair.second)), thepair.second));
        } else {
          __set2thread_id.push_back(
              std::pair<const IdSet<IdSetAlloc> *, unsigned int>(nullptr,
                                                                 thepair.second));
        }
      }

      // fill the var2idsets
      for (auto iter = __idsets.begin(); iter != __idsets.end(); ++iter) {
        const IdSet<IdSetAlloc> &idset = iter.first();
        for (const auto id : idset.ids()) {
          try {
            __var2idsets[id].push_back(&idset);
          } catch (NotFound &) {
            __var2idsets.insert(id, std::vector<const IdSet<IdSetAlloc> *>());
            __var2idsets[id].push_back(&idset);
          }
        }
      }

      GUM_CONS_CPY(RecordCounter);
    }

    /// move constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE RecordCounter<IdSetAlloc, CountAlloc>::RecordCounter(
        RecordCounter<IdSetAlloc, CountAlloc> &&from)
        : __modalities(from.__modalities), __idsets(from.__idsets),
          __nodesets(std::move(from.__nodesets)),
          __var2idsets(std::move(from.__var2idsets)),
          __set_state(std::move(from.__set_state)),
          __countings(std::move(from.__countings)),
          __idset2index(std::move(from.__idset2index)),
          __set2thread_id(std::move(from.__set2thread_id)),
          __subset_lattice(std::move(from.__subset_lattice)),
          __thread_counters(std::move(from.__thread_counters)),
          __nb_thread_counters(from.__nb_thread_counters) {
      GUM_CONS_MOV(RecordCounter);
    }

    /// destructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE RecordCounter<IdSetAlloc, CountAlloc>::~RecordCounter() {
      for (auto counter : __thread_counters)
        delete counter;

      GUM_DESTRUCTOR(RecordCounter);
    }

    /// add a new nodeset to count
    template <typename IdSetAlloc, typename CountAlloc>
    unsigned int RecordCounter<IdSetAlloc, CountAlloc>::addNodeSet(
        const std::vector<unsigned int, IdSetAlloc> &ids) {
      // allocate a count vector of the domain size of the idset
      unsigned int set_size = 1;

      for (const auto id : ids) {
        set_size *= __modalities->operator[](id);
      }

      __countings.push_back(std::vector<float, CountAlloc>(set_size, 0.0f));

      // create the corresponding idset
      NodeId node = __nodesets.size();
      __nodesets.push_back(&ids);

      // for empty sets
      if (ids.empty()) {
        __set_state.push_back(SetState::EMPTY_SET);
        __set2thread_id.push_back(
            std::pair<const IdSet<IdSetAlloc> *, unsigned int>(nullptr, 0));
        return node;
      }

      // for nonempty sets, always add the node to the subset lattice
      __subset_lattice.addNode(node);
      IdSet<IdSetAlloc> tmp_idset(ids, set_size);

      if (!__idsets.existsFirst(tmp_idset)) {
        // a priori, the idset is not a subset
        __set_state.push_back(SetState::NOT_SUBSET);

        __idsets.insert(std::move(tmp_idset), std::move(node));
        const IdSet<IdSetAlloc> &idset = __idsets.first(node);
        __idset2index.insert(&idset, node);
        __set2thread_id.push_back(
            std::pair<const IdSet<IdSetAlloc> *, unsigned int>(&idset, node));

        // for each variable in ids, indicate that ids contain this variable
        for (auto id : ids) {
          try {
            __var2idsets[id].push_back(&idset);
          } catch (NotFound &) {
            __var2idsets.insert(id, std::vector<const IdSet<IdSetAlloc> *>());
            __var2idsets[id].push_back(&idset);
          }
        }
      } else {
        // this set is a copy of another set
        __set_state.push_back(SetState::COPY_SET);

        __set2thread_id.push_back(std::pair<const IdSet<IdSetAlloc> *, unsigned int>(
            nullptr, __idsets.second(tmp_idset)));

        __subset_lattice.addArc(__idsets.second(tmp_idset), node);
      }

      return node;
    }

    // an algorithm to parse a subset of the database
    template <typename IdSetAlloc, typename CountAlloc>
    void RecordCounter<IdSetAlloc, CountAlloc>::countOnSubDatabase() {
      // now, for all the non-subsets, compute their countings
      // start parallel ThreadCounters
      const unsigned long db_size = __thread_counters[0]->DBSize();

      // compute the max number of threads to use to parse the database, so that
      // each thread has at least NB elements to parse. This proves useful for
      // small databases and large numbers of processors
      unsigned int max_nb_threads = std::min(db_size / __min_nb_rows_per_thread,
                                             (unsigned long)__max_threads_number);
      const unsigned long max_size_per_thread =
          (db_size + max_nb_threads - 1) / max_nb_threads;
      max_nb_threads = db_size / max_size_per_thread;

#pragma omp parallel num_threads(max_nb_threads)
      {
        // create ThreadCounters if needed
        const unsigned int num_threads = getNumberOfRunningThreads();

#pragma omp single
        {
          while (__thread_counters.size() < num_threads) {
            __thread_counters.push_back(__thread_counters[0]->copyFactory());
          }

          __nb_thread_counters = num_threads;
        }

        // initialize the thread counters
        const int this_thread = getThreadNumber();
        RecordCounterThreadBase<IdSetAlloc, CountAlloc> &thread_counter =
            *(__thread_counters[this_thread]);
        thread_counter.clearNodeSets();

        for (unsigned int i = 0, size = __set_state.size(); i < size; ++i) {
          if (__set_state[i] == SetState::NOT_SUBSET) {
            thread_counter.addNodeSet(*(__nodesets[i]));
          }
        }

        // indicate to the filter which part of the database it must parse
        const unsigned long size_per_thread =
            (db_size + num_threads - 1) / num_threads;
        const unsigned long min_range = size_per_thread * this_thread;
        const unsigned long max_range =
            std::min(min_range + size_per_thread, db_size);
        if (min_range < max_range) {
          thread_counter.setRange(min_range, max_range);

          // compute the counts
          thread_counter.count();
        }

      } // omp parallel

// perform the aggregation of the countings
#pragma omp parallel num_threads(__nb_thread_counters)
      {
        const unsigned int this_thread = getThreadNumber();
        auto &counter = *(__thread_counters[this_thread]);
        unsigned long size_per_thread, min_range, max_range;

        for (unsigned int i = 0, size = __countings.size(); i < size; ++i) {
          if (__set_state[i] == SetState::NOT_SUBSET) {
            // get the ith idset countings computed by the curent thread
            std::vector<float, CountAlloc> &vect =
                const_cast<std::vector<float, CountAlloc> &>(
                    counter.getCounts(__set2thread_id[i].second));
            size_per_thread =
                (vect.size() + __nb_thread_counters - 1) / __nb_thread_counters;

            // add to vect the countings of the other threads
            min_range = size_per_thread * this_thread;
            max_range =
                std::min(min_range + size_per_thread, (unsigned long)vect.size());

            for (unsigned int j = 0; j < __nb_thread_counters; ++j) {
              if (j != this_thread) {
                const std::vector<float, CountAlloc> &othervect =
                    __thread_counters[j]->getCounts(__set2thread_id[i].second);

                for (unsigned int k = min_range; k < max_range; ++k) {
                  vect[k] += othervect[k];
                }
              }
            }

            // now copy what we just computed into the countings of the
            // current object
            std::vector<float, CountAlloc> &final_vect = __countings[i];

            for (unsigned int k = min_range; k < max_range; ++k) {
              final_vect[k] = vect[k];
            }
          }
        }
      }
    }

    // computes the countings of one subset from those of its superset
    template <typename IdSetAlloc, typename CountAlloc>
    void RecordCounter<IdSetAlloc, CountAlloc>::__countOneSubset(unsigned int i) {
      // get the subset and its superset
      const auto &subset_ids = *(__nodesets[i]);
      const auto &superset_ids = *(__nodesets[__set2thread_id[i].second]);
      auto &subset_vect = __countings[i];
      const auto &superset_vect = __countings[__set2thread_id[i].second];

      // Compute the variables that belong to both the (projection) subset
      // and its superset. Store the number of increments in the computation
      // loops at the end of the function before which the variables of the
      // projection set need be incremented (vector before_incr).
      std::vector<unsigned int> result_offset(subset_ids.size());
      std::vector<unsigned int> result_domain(subset_ids.size());
      std::vector<unsigned int> before_incr(subset_ids.size());
      {
        unsigned int result_domain_size = 1;
        unsigned int tmp_before_incr = 1;
        bool has_before_incr = false;
        unsigned int subset_size = subset_ids.size();
        std::vector<unsigned int> superset_order(subset_ids.size());

        // put the subset ids into a Set in order to know quickly if a given
        // id belongs to the subset or not
        HashTable<unsigned int, unsigned int> subset;
        {
          unsigned int i = 0;

          for (auto id : subset_ids) {
            subset.insert(id, i);
            ++i;
          }
        }

        for (unsigned int h = 0, j = 0; h < superset_ids.size(); ++h) {
          if (j < subset_size) {
            if (subset.exists(superset_ids[h])) {
              if (has_before_incr) {
                before_incr[j] = tmp_before_incr - 1;
                has_before_incr = false;
              } else {
                before_incr[j] = 0;
              }

              superset_order[subset[superset_ids[h]]] = j;
              tmp_before_incr = 1;
              ++j;
            } else {
              unsigned int modality = __modalities->operator[](superset_ids[h]);
              tmp_before_incr *= modality;
              has_before_incr = true;
            }
          }
        }

        // compute the offsets in the correct order
        for (unsigned int i = 0; i < subset_ids.size(); ++i) {
          unsigned int modality = __modalities->operator[](subset_ids[i]);
          unsigned int j = superset_order[i];
          result_domain[j] = modality;
          result_offset[j] = result_domain_size;
          result_domain_size *= modality;
        }
      }

      std::vector<unsigned int> result_value = result_domain;
      std::vector<unsigned int> current_incr = before_incr;
      std::vector<unsigned int> result_down = result_offset;

      for (unsigned int j = 0; j < result_down.size(); ++j) {
        result_down[j] = (result_domain[j] - 1) * result_offset[j];
      }

      // now, fill the subset counting vector: first loop over the variables
      // X's in table that do not belong to result and, for each value of
      // these X's, loop over the variables in both table and result. As
      // such, in the internal loop, the offsets of "result" need only be
      // incremented as usually to parse appropriately this table. For
      // result, the problem is slightly more complicated: in the outer for
      // loop, we shall always reset resul_offset to 0. For the inner loop,
      // result_offset should be incremented (++) only when t1
      // before_incr[xxx] steps in the loop have already been made.
      unsigned int the_result_offset = 0;
      unsigned int table_domain_size = superset_vect.size();

      for (unsigned int h = 0; h < table_domain_size; ++h) {
        subset_vect[the_result_offset] += superset_vect[h];

        // update the offset of result
        for (unsigned int k = 0; k < current_incr.size(); ++k) {
          // check if we need modify result_offset
          if (current_incr[k]) {
            --current_incr[k];
            break;
          }

          current_incr[k] = before_incr[k];

          // here we shall modify result_offset
          --result_value[k];

          if (result_value[k]) {
            the_result_offset += result_offset[k];
            break;
          }

          result_value[k] = result_domain[k];
          the_result_offset -= result_down[k];
        }
      }
    }

    // computes the countings of the subsets from those of their supersets
    template <typename IdSetAlloc, typename CountAlloc>
    void RecordCounter<IdSetAlloc, CountAlloc>::countSubsets() {
      // computes a queue of the subsets that can be considered for counting
      // to do so, simply fill it with the nodes without parents in the
      // subset lattice
      List<unsigned int> setFIFO;

      for (const auto node : __subset_lattice) {
        if (__subset_lattice.parents(node).size() == 0) {
          setFIFO.pushBack(node);
        }
      }

      // perform the countings for the subsets
      while (!setFIFO.empty()) {
        // get the next set to count
        unsigned int new_set = setFIFO.front();
        setFIFO.popFront();

        // perform the counting
        __countOneSubset(new_set);

        // update the subset lattice and add, if needed, new sets into setFIFO
        const NodeSet &children = __subset_lattice.children(new_set);

        for (typename NodeSet::const_iterator iter = children.begin();
             iter != children.end(); ++iter) {
          if (__subset_lattice.parents(*iter).size() == 1) {
            setFIFO.pushBack(*iter);
          } else {
            __subset_lattice.eraseArc(Arc(new_set, *iter));
          }
        }
      }
    }

    /// perform the countings of all the sets of ids
    template <typename IdSetAlloc, typename CountAlloc>
    void RecordCounter<IdSetAlloc, CountAlloc>::count() {
      // first, we examine the nodesets to determine which ones are subsets
      // of other nodesets. For those sets, we will derive their countings
      // directly from their supersets
      __computeSubsets();

      // divide the countings among threads w.r.t. the database, i.e., each
      // thread will compute all the countings but only on a subset of the
      // database
      countOnSubDatabase();

      // perform the countings on the subsets
      countSubsets();

      // deallocate the memory used by the thread counters
      for (auto counter : __thread_counters) {
        counter->clearNodeSets();
      }
    }

    /// returns the counts performed for a given idSet
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE const std::vector<float, CountAlloc> &
    RecordCounter<IdSetAlloc, CountAlloc>::getCounts(unsigned int idset) const
        noexcept {
      return __countings[idset];
    }

    /// resets the counter, i.e., remove all its sets of ids and counting vectors
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE void RecordCounter<IdSetAlloc, CountAlloc>::clearNodeSets() noexcept {
      for (auto counter : __thread_counters) {
        counter->clearNodeSets();
      }

      __idsets.clear();
      __var2idsets.clear();
      __set_state.clear();
      __countings.clear();
      __set2thread_id.clear();
      __idset2index.clear();
      __nodesets.clear();
      __subset_lattice.clear();
    }

    /// determine which sets are subsets
    template <typename IdSetAlloc, typename CountAlloc>
    void RecordCounter<IdSetAlloc, CountAlloc>::__computeSubsets() {
      // sort the idSets by increasing size order for each variable
      for (auto iter = __var2idsets.begin(); iter != __var2idsets.end(); ++iter) {
        std::sort(iter.val().begin(), iter.val().end(),
                  [](const IdSet<IdSetAlloc> *set1, const IdSet<IdSetAlloc> *set2)
                      -> bool { return set1->ids().size() < set2->ids().size(); });
      }

      // now, for each IdSet, determine if it is a subset of another IdSet
      for (unsigned int i = 0, j = 0, size = __set2thread_id.size(); i < size; ++i) {
        if (__set_state[i] < SetState::COPY_SET) {
          // the set is not known to be a copy of another set and is not empty

          // get the IdSet to determine
          const IdSet<IdSetAlloc> &ids = *(__set2thread_id[i].first);
          const unsigned int ids_size = ids.ids().size();

          // get all the sets that contain its first variable and parse them
          const std::vector<const IdSet<IdSetAlloc> *> &sets = __var2idsets[ids[0]];
          bool subset = false;
          unsigned int index;

          for (const auto set : sets) {
            if ((set->ids().size() > ids_size) && ids.isSubset(*set)) {
              subset = true;
              index = __idset2index[set];
              break;
            }
          }

          if (subset) {
            // assign the superset to the subset and allocate the subset's counting
            __set_state[i] = SetState::STRICT_SUBSET;
            __set2thread_id[i].second = index;
            __subset_lattice.addArc(index, i);
          } else {
            __set2thread_id[i].second = j;
            ++j;
          }
        }
      }

      // now that we know which nodes are supersets and which ones are not, we
      // can remove from the lattice all the nodes that are supersets
      for (unsigned int i = 0, size = __set2thread_id.size(); i < size; ++i) {
        if (__set_state[i] == SetState::NOT_SUBSET) {
          __subset_lattice.eraseNode(i);
        }
      }
    }

    /// returns the modalities of the variables in the database
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE const std::vector<unsigned int> &
    RecordCounter<IdSetAlloc, CountAlloc>::modalities() const {
      if (__modalities)
        return *__modalities;
      else {
        GUM_ERROR(NullElement,
                  "The record Counter does not have any modalities stored yet");
      }
    }

    /// sets the maximum number of threads used to perform countings
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE void RecordCounter<IdSetAlloc, CountAlloc>::setMaxNbThreads(
        unsigned int nb) noexcept {
#if defined(_OPENMP) && defined(NDEBUG)
      if (nb == 0)
        nb = getMaxNumberOfThreads();

      __max_threads_number = nb;
#else
      __max_threads_number = 1;
#endif /* _OPENMP && NDEBUG */
    }

    /// returns the counting performed
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE std::vector<std::vector<float, CountAlloc>> &
    RecordCounter<IdSetAlloc, CountAlloc>::__getCounts() noexcept {
      return __countings;
    }

    /// returns the size of the database
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE unsigned long RecordCounter<IdSetAlloc, CountAlloc>::DBSize() noexcept {
      return __thread_counters[0]->DBSize();
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
