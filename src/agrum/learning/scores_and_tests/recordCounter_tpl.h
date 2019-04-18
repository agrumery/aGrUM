
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
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/learning/scores_and_tests/recordCounter.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {


    /// returns the allocator used by the translator
    template < template < typename > class ALLOC >
    INLINE typename RecordCounter< ALLOC >::allocator_type
       RecordCounter< ALLOC >::getAllocator() const {
      return __parsers.get_allocator();
    }


    /// default constructor
    template < template < typename > class ALLOC >
    RecordCounter< ALLOC >::RecordCounter(
       const DBRowGeneratorParser< ALLOC >&                                 parser,
       const std::vector< std::pair< std::size_t, std::size_t >,
                          ALLOC< std::pair< std::size_t, std::size_t > > >& ranges,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                                                              nodeId2columns,
       const typename RecordCounter< ALLOC >::allocator_type& alloc) :
        __parsers(alloc),
        __ranges(alloc), __nodeId2columns(nodeId2columns),
        __last_DB_countings(alloc), __last_DB_ids(alloc),
        __last_nonDB_countings(alloc), __last_nonDB_ids(alloc) {
      // check that the columns in nodeId2columns do belong to the database
      const std::size_t db_nb_cols = parser.database().nbVariables();
      for (auto iter = nodeId2columns.cbegin(); iter != nodeId2columns.cend();
           ++iter) {
        if (iter.second() >= db_nb_cols) {
          GUM_ERROR(OutOfBounds,
                    "the mapping between ids and database columns "
                       << "is incorrect because Column " << iter.second()
                       << " does not belong to the database.");
        }
      }

      // create the parsers. There should always be at least one parser
      if (__max_nb_threads < std::size_t(1)) __max_nb_threads = std::size_t(1);
      __parsers.reserve(__max_nb_threads);
      for (std::size_t i = std::size_t(0); i < __max_nb_threads; ++i)
        __parsers.push_back(parser);

      // check that the ranges are within the bounds of the database and
      // save them
      __checkRanges(ranges);
      __ranges.reserve(ranges.size());
      for (const auto& range : ranges)
        __ranges.push_back(range);

      // dispatch the ranges for the threads
      __dispatchRangesToThreads();

      GUM_CONSTRUCTOR(RecordCounter);
    }


    /// default constructor
    template < template < typename > class ALLOC >
    RecordCounter< ALLOC >::RecordCounter(
       const DBRowGeneratorParser< ALLOC >& parser,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                                                              nodeId2columns,
       const typename RecordCounter< ALLOC >::allocator_type& alloc) :
        RecordCounter< ALLOC >(
           parser,
           std::vector< std::pair< std::size_t, std::size_t >,
                        ALLOC< std::pair< std::size_t, std::size_t > > >(),
           nodeId2columns,
           alloc) {}


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    RecordCounter< ALLOC >::RecordCounter(
       const RecordCounter< ALLOC >&                          from,
       const typename RecordCounter< ALLOC >::allocator_type& alloc) :
        __parsers(from.__parsers, alloc),
        __ranges(from.__ranges, alloc),
        __thread_ranges(from.__thread_ranges, alloc),
        __nodeId2columns(from.__nodeId2columns),
        __last_DB_countings(from.__last_DB_countings, alloc),
        __last_DB_ids(from.__last_DB_ids),
        __last_nonDB_countings(from.__last_nonDB_countings, alloc),
        __last_nonDB_ids(from.__last_nonDB_ids),
        __max_nb_threads(from.__max_nb_threads),
        __min_nb_rows_per_thread(from.__min_nb_rows_per_thread) {
      GUM_CONS_CPY(RecordCounter);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    RecordCounter< ALLOC >::RecordCounter(const RecordCounter< ALLOC >& from) :
        RecordCounter< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    RecordCounter< ALLOC >::RecordCounter(
       RecordCounter< ALLOC >&&                               from,
       const typename RecordCounter< ALLOC >::allocator_type& alloc) :
        __parsers(std::move(from.__parsers), alloc),
        __ranges(std::move(from.__ranges), alloc),
        __thread_ranges(std::move(from.__thread_ranges), alloc),
        __nodeId2columns(std::move(from.__nodeId2columns)),
        __last_DB_countings(std::move(from.__last_DB_countings), alloc),
        __last_DB_ids(std::move(from.__last_DB_ids)),
        __last_nonDB_countings(std::move(from.__last_nonDB_countings), alloc),
        __last_nonDB_ids(std::move(from.__last_nonDB_ids)),
        __max_nb_threads(from.__max_nb_threads),
        __min_nb_rows_per_thread(from.__min_nb_rows_per_thread) {
      GUM_CONS_MOV(RecordCounter);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    RecordCounter< ALLOC >::RecordCounter(RecordCounter< ALLOC >&& from) :
        RecordCounter< ALLOC >(std::move(from), from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    RecordCounter< ALLOC >* RecordCounter< ALLOC >::clone(
       const typename RecordCounter< ALLOC >::allocator_type& alloc) const {
      ALLOC< RecordCounter< ALLOC > > allocator(alloc);
      RecordCounter< ALLOC >*         new_counter = allocator.allocate(1);
      try {
        allocator.construct(new_counter, *this, alloc);
      } catch (...) {
        allocator.deallocate(new_counter, 1);
        throw;
      }

      return new_counter;
    }


    /// virtual copy constructor
    template < template < typename > class ALLOC >
    RecordCounter< ALLOC >* RecordCounter< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    RecordCounter< ALLOC >::~RecordCounter() {
      GUM_DESTRUCTOR(RecordCounter);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    RecordCounter< ALLOC >& RecordCounter< ALLOC >::
                            operator=(const RecordCounter< ALLOC >& from) {
      if (this != &from) {
        __parsers = from.__parsers;
        __ranges = from.__ranges;
        __thread_ranges = from.__thread_ranges;
        __nodeId2columns = from.__nodeId2columns;
        __last_DB_countings = from.__last_DB_countings;
        __last_DB_ids = from.__last_DB_ids;
        __last_nonDB_countings = from.__last_nonDB_countings;
        __last_nonDB_ids = from.__last_nonDB_ids;
        __max_nb_threads = from.__max_nb_threads;
        __min_nb_rows_per_thread = from.__min_nb_rows_per_thread;
      }
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    RecordCounter< ALLOC >& RecordCounter< ALLOC >::
                            operator=(RecordCounter< ALLOC >&& from) {
      if (this != &from) {
        __parsers = std::move(from.__parsers);
        __ranges = std::move(from.__ranges);
        __thread_ranges = std::move(from.__thread_ranges);
        __nodeId2columns = std::move(from.__nodeId2columns);
        __last_DB_countings = std::move(from.__last_DB_countings);
        __last_DB_ids = std::move(from.__last_DB_ids);
        __last_nonDB_countings = std::move(from.__last_nonDB_countings);
        __last_nonDB_ids = std::move(from.__last_nonDB_ids);
        __max_nb_threads = from.__max_nb_threads;
        __min_nb_rows_per_thread = from.__min_nb_rows_per_thread;
      }
      return *this;
    }


    /// clears all the last database-parsed countings from memory
    template < template < typename > class ALLOC >
    void RecordCounter< ALLOC >::clear() {
      __last_DB_countings.clear();
      __last_DB_ids.clear();
      __last_nonDB_countings.clear();
      __last_nonDB_ids.clear();
    }


    /// changes the max number of threads used to parse the database
    template < template < typename > class ALLOC >
    void RecordCounter< ALLOC >::setMaxNbThreads(const std::size_t nb) const {
      if (nb == std::size_t(0) || !isOMP())
        __max_nb_threads = std::size_t(1);
      else
        __max_nb_threads = nb;
    }


    /// returns the number of threads used to parse the database
    template < template < typename > class ALLOC >
    INLINE std::size_t RecordCounter< ALLOC >::nbThreads() const {
      return __max_nb_threads;
    }


    // changes the number min of rows a thread should process in a
    // multithreading context
    template < template < typename > class ALLOC >
    void
       RecordCounter< ALLOC >::setMinNbRowsPerThread(const std::size_t nb) const {
      if (nb == std::size_t(0))
        __min_nb_rows_per_thread = std::size_t(1);
      else
        __min_nb_rows_per_thread = nb;
    }


    /// returns the minimum of rows that each thread should process
    template < template < typename > class ALLOC >
    INLINE std::size_t RecordCounter< ALLOC >::minNbRowsPerThread() const {
      return __min_nb_rows_per_thread;
    }


    /// compute and raise the exception when some variables are continuous
    template < template < typename > class ALLOC >
    void RecordCounter< ALLOC >::__raiseCheckException(
       const std::vector< std::string, ALLOC< std::string > >& bad_vars) const {
      // generate the exception
      std::stringstream msg;
      msg << "Counts cannot be performed on continuous variables. ";
      msg << "Unfortunately the following variable";
      if (bad_vars.size() == 1)
        msg << " is continuous: " << bad_vars[0];
      else {
        msg << "s are continuous: ";
        bool deja = false;
        for (const auto& name : bad_vars) {
          if (deja)
            msg << ", ";
          else
            deja = true;
          msg << name;
        }
      }
      GUM_ERROR(TypeError, msg.str());
    }


    /// check that all the variables in an idset are discrete
    template < template < typename > class ALLOC >
    void RecordCounter< ALLOC >::__checkDiscreteVariables(
       const IdSet< ALLOC >& ids) const {
      const std::size_t             size = ids.size();
      const DatabaseTable< ALLOC >& database = __parsers[0].data.database();

      if (__nodeId2columns.empty()) {
        // check all the ids
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          if (database.variable(i).varType() == VarType::Continuous) {
            // here, var i does not correspond to a discrete variable.
            // we check whether there are other non discrete variables, so that
            // we can generate an exception mentioning all these variables
            std::vector< std::string, ALLOC< std::string > > bad_vars{
               database.variable(i).name()};
            for (++i; i < size; ++i) {
              if (database.variable(i).varType() == VarType::Continuous)
                bad_vars.push_back(database.variable(i).name());
            }
            __raiseCheckException(bad_vars);
          }
        }
      } else {
        // check all the ids
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          // get the position of the variable in the database
          std::size_t pos = __nodeId2columns.second(ids[i]);

          if (database.variable(pos).varType() == VarType::Continuous) {
            // here, id does not correspond to a discrete variable.
            // we check whether there are other non discrete variables, so that
            // we can generate an exception mentioning all these variables
            std::vector< std::string, ALLOC< std::string > > bad_vars{
               database.variable(pos).name()};
            for (++i; i < size; ++i) {
              pos = __nodeId2columns.second(ids[i]);
              if (database.variable(pos).varType() == VarType::Continuous)
                bad_vars.push_back(database.variable(pos).name());
            }
            __raiseCheckException(bad_vars);
          }
        }
      }
    }


    /// returns the mapping from ids to column positions in the database
    template < template < typename > class ALLOC >
    INLINE const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                 RecordCounter< ALLOC >::nodeId2Columns() const {
      return __nodeId2columns;
    }


    /// returns the database on which we perform the counts
    template < template < typename > class ALLOC >
    const DatabaseTable< ALLOC >& RecordCounter< ALLOC >::database() const {
      return __parsers[0].data.database();
    }


    /// returns the counts for a given set of nodes
    template < template < typename > class ALLOC >
    INLINE const std::vector< double, ALLOC< double > >&
                 RecordCounter< ALLOC >::counts(const IdSet< ALLOC >& ids,
                                      const bool            check_discrete_vars) {
      // if the idset is empty, return an empty vector
      if (ids.empty()) {
        __last_nonDB_ids.clear();
        __last_nonDB_countings.clear();
        return __last_nonDB_countings;
      }

      // check whether we can extract the vector we wish to return from
      // some already computed counting vector
      if (__last_nonDB_ids.contains(ids))
        return __extractFromCountings(
           ids, __last_nonDB_ids, __last_nonDB_countings);
      else if (__last_DB_ids.contains(ids))
        return __extractFromCountings(ids, __last_DB_ids, __last_DB_countings);
      else {
        if (check_discrete_vars) __checkDiscreteVariables(ids);
        return __countFromDatabase(ids);
      }
    }


    // returns a mapping from the nodes ids to the columns of the database
    // for a given sequence of ids
    template < template < typename > class ALLOC >
    HashTable< NodeId, std::size_t > RecordCounter< ALLOC >::__getNodeIds2Columns(
       const IdSet< ALLOC >& ids) const {
      HashTable< NodeId, std::size_t > res(ids.size());
      if (__nodeId2columns.empty()) {
        for (const auto id : ids) {
          res.insert(id, std::size_t(id));
        }
      } else {
        for (const auto id : ids) {
          res.insert(id, __nodeId2columns.second(id));
        }
      }
      return res;
    }


    /// extracts some new countings from previously computed ones
    template < template < typename > class ALLOC >
    INLINE std::vector< double, ALLOC< double > >&
           RecordCounter< ALLOC >::__extractFromCountings(
          const IdSet< ALLOC >&                         subset_ids,
          const IdSet< ALLOC >&                         superset_ids,
          const std::vector< double, ALLOC< double > >& superset_vect) {
      // get a mapping between the node Ids and their columns in the database.
      // This should be stored into __nodeId2columns, except if the latter is
      // empty, in which case there is an identity mapping
      const auto nodeId2columns = __getNodeIds2Columns(superset_ids);

      // we first determine the size of the output vector, the domain of
      // each of its variables and their offsets in the output vector
      const auto& database = __parsers[0].data.database();
      std::size_t result_vect_size = std::size_t(1);
      for (const auto id : subset_ids) {
        result_vect_size *= database.domainSize(nodeId2columns[id]);
      }

      // we create the output vector
      const std::size_t subset_ids_size = std::size_t(subset_ids.size());
      std::vector< double, ALLOC< double > > result_vect(result_vect_size, 0.0);


      // check if the subset_ids is the beginning of the sequence of superset_ids
      // if this is the case, then we can outer loop over the variables not in
      // subset_ids and, for each iteration of this loop add a vector of size
      // result_size to result_vect
      bool subset_begin = true;
      for (std::size_t i = 0; i < subset_ids_size; ++i) {
        if (superset_ids.pos(subset_ids[i]) != i) {
          subset_begin = false;
          break;
        }
      }

      if (subset_begin) {
        const std::size_t superset_vect_size = superset_vect.size();
        std::size_t       i = std::size_t(0);
        while (i < superset_vect_size) {
          for (std::size_t j = std::size_t(0); j < result_vect_size; ++j, ++i) {
            result_vect[j] += superset_vect[i];
          }
        }

        // save the subset_ids and the result vector
        try {
          __last_nonDB_ids = subset_ids;
          __last_nonDB_countings = std::move(result_vect);
          return __last_nonDB_countings;
        } catch (...) {
          __last_nonDB_ids.clear();
          __last_nonDB_countings.clear();
          throw;
        }
      }


      // check if subset_ids is the end of the sequence of superset_ids.
      // In this case, as above, there are two simple loops to perform the
      // countings
      bool              subset_end = true;
      const std::size_t superset_ids_size = std::size_t(superset_ids.size());
      for (std::size_t i = 0; i < subset_ids_size; ++i) {
        if (superset_ids.pos(subset_ids[i])
            != i + superset_ids_size - subset_ids_size) {
          subset_end = false;
          break;
        }
      }

      if (subset_end) {
        // determine the size of the vector corresponding to the variables
        // not belonging to subset_ids
        std::size_t vect_not_subset_size = std::size_t(1);
        for (std::size_t i = std::size_t(0);
             i < superset_ids_size - subset_ids_size;
             ++i)
          vect_not_subset_size *=
             database.domainSize(nodeId2columns[superset_ids[i]]);

        // perform the two loops
        std::size_t i = std::size_t(0);
        for (std::size_t j = std::size_t(0); j < result_vect_size; ++j) {
          for (std::size_t k = std::size_t(0); k < vect_not_subset_size;
               ++k, ++i) {
            result_vect[j] += superset_vect[i];
          }
        }

        // save the subset_ids and the result vector
        try {
          __last_nonDB_ids = subset_ids;
          __last_nonDB_countings = std::move(result_vect);
          return __last_nonDB_countings;
        } catch (...) {
          __last_nonDB_ids.clear();
          __last_nonDB_countings.clear();
          throw;
        }
      }


      // here subset_ids is a subset of superset_ids neither prefixing nor
      // postfixing it. So the computation is somewhat more complicated.

      // We will parse the superset_vect sequentially (using ++ operator).
      // Sometimes, we will need to change the offset of the cell of result_vect
      // that will be affected, sometimes not. Vector before_incr will indicate
      // whether we need to change the offset (value = 0) or not (value different
      // from 0). Vectors result_domain will indicate how this offset should be
      // computed. Here is an example of the values of these vectors. Assume that
      // superset_ids = <A,B,C,D,E> and subset_ids = <A,D,C>. Then, the three
      // vectors before_incr, result_domain and result_offset are indexed w.r.t.
      // A,C,D, i.e., w.r.t. to the variables in subset_ids but order w.r.t.
      // superset_ids (this is convenient as we will parse superset_vect
      // sequentially. For a variable or a set of variables X, let M_X denote the
      // domain size of X. Then the contents of the three vectors are as follows:
      // before_incr = {0, M_B, 0} (this means that whenever we iterate over B's
      //                       values, the offset in result_vect does not change)
      // result_domain = { M_A, M_C, M_D } (i.e., the domain sizes of the variables
      //                       in subset_ids, order w.r.t. superset_ids)
      // result_offset = { 1, M_A*M_D, M_A } (this corresponds to the offsets
      //                       in result_vect of variables A, C and D)
      // Vector superset_order = { 0, 2, 1} : this is a map from the indices of
      // the variables in subset_ids to the indices of these variables in the
      // three vectors described above. For instance, the "2" means that variable
      // D (which is at index 1 in subset_ids) is located at index 2 in vector
      // before_incr
      std::vector< std::size_t > before_incr(subset_ids_size);
      std::vector< std::size_t > result_domain(subset_ids_size);
      std::vector< std::size_t > result_offset(subset_ids_size);
      {
        std::size_t                result_domain_size = std::size_t(1);
        std::size_t                tmp_before_incr = std::size_t(1);
        std::vector< std::size_t > superset_order(subset_ids_size);

        for (std::size_t h = std::size_t(0), j = std::size_t(0);
             j < subset_ids_size;
             ++h) {
          if (subset_ids.exists(superset_ids[h])) {
            before_incr[j] = tmp_before_incr - 1;
            superset_order[subset_ids.pos(superset_ids[h])] = j;
            tmp_before_incr = 1;
            ++j;
          } else {
            tmp_before_incr *=
               database.domainSize(nodeId2columns[superset_ids[h]]);
          }
        }

        // compute the offsets in the order of the superset_ids
        for (std::size_t i = 0; i < subset_ids.size(); ++i) {
          const std::size_t domain_size =
             database.domainSize(nodeId2columns[subset_ids[i]]);
          const std::size_t j = superset_order[i];
          result_domain[j] = domain_size;
          result_offset[j] = result_domain_size;
          result_domain_size *= domain_size;
        }
      }

      std::vector< std::size_t > result_value(result_domain);
      std::vector< std::size_t > current_incr(before_incr);
      std::vector< std::size_t > result_down(result_offset);

      for (std::size_t j = std::size_t(0); j < result_down.size(); ++j) {
        result_down[j] *= (result_domain[j] - 1);
      }

      // now we can loop over the superset_vect to fill result_vect
      const std::size_t superset_vect_size = superset_vect.size();
      std::size_t       the_result_offset = std::size_t(0);
      for (std::size_t h = std::size_t(0); h < superset_vect_size; ++h) {
        result_vect[the_result_offset] += superset_vect[h];

        // update the offset of result_vect
        for (std::size_t k = 0; k < current_incr.size(); ++k) {
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

      // save the subset_ids and the result vector
      try {
        __last_nonDB_ids = subset_ids;
        __last_nonDB_countings = std::move(result_vect);
        return __last_nonDB_countings;
      } catch (...) {
        __last_nonDB_ids.clear();
        __last_nonDB_countings.clear();
        throw;
      }
    }


    /// parse the database to produce new countings
    template < template < typename > class ALLOC >
    std::vector< double, ALLOC< double > >&
       RecordCounter< ALLOC >::__countFromDatabase(const IdSet< ALLOC >& ids) {
      // if the ids vector is empty or the database is empty, return an
      // empty vector
      const auto& database = __parsers[0].data.database();
      if (ids.empty() || database.empty() || __thread_ranges.empty()) {
        __last_nonDB_countings.clear();
        __last_nonDB_ids.clear();
        return __last_nonDB_countings;
      }

      // we translate the ids into their corresponding columns in the
      // DatabaseTable
      const auto nodeId2columns = __getNodeIds2Columns(ids);

      // we first determine the size of the counting vector, the domain of
      // each of its variables and their offsets in the output vector
      const std::size_t ids_size = ids.size();
      std::size_t       counting_vect_size = std::size_t(1);
      std::vector< std::size_t, ALLOC< std::size_t > > domain_sizes(ids_size);
      std::vector< std::pair< std::size_t, std::size_t >,
                   ALLOC< std::pair< std::size_t, std::size_t > > >
         cols_offsets(ids_size);
      {
        std::size_t i = std::size_t(0);
        for (const auto id : ids) {
          const std::size_t domain_size = database.domainSize(nodeId2columns[id]);
          domain_sizes[i] = domain_size;
          cols_offsets[i].first = nodeId2columns[id];
          cols_offsets[i].second = counting_vect_size;
          counting_vect_size *= domain_size;
          ++i;
        }
      }

      // we sort the columns and offsets by increasing column index. This
      // may speed up threaded countings by improving the cacheline hits
      std::sort(cols_offsets.begin(),
                cols_offsets.end(),
                [](const std::pair< std::size_t, std::size_t >& a,
                   const std::pair< std::size_t, std::size_t >& b) -> bool {
                  return a.first < b.first;
                });

      // create parsers if needed
      const std::size_t nb_ranges = __thread_ranges.size();
      const std::size_t nb_threads =
         nb_ranges <= __max_nb_threads ? nb_ranges : __max_nb_threads;
      while (__parsers.size() < nb_threads) {
        ThreadData< DBRowGeneratorParser< ALLOC > > new_parser(__parsers[0]);
        __parsers.push_back(std::move(new_parser));
      }

      // set the columns of interest for each parser. This specifies to the
      // parser which columns are used for the countings. This is important
      // for parsers like the EM parser that complete unobserved variables.
      std::vector< std::size_t, ALLOC< std::size_t > > cols_of_interest(ids_size);
      for (std::size_t i = std::size_t(0); i < ids_size; ++i) {
        cols_of_interest[i] = cols_offsets[i].first;
      }
      for (auto& parser : __parsers) {
        parser.data.setColumnsOfInterest(cols_of_interest);
      }

      // allocate all the counting vectors, including that which will add
      // all the results provided by the threads. We initialize once and
      // for all these vectors with zeroes
      std::vector< double, ALLOC< double > > counting_vect(counting_vect_size,
                                                           0.0);
      std::vector< ThreadData< std::vector< double, ALLOC< double > > >,
                   ALLOC< ThreadData< std::vector< double, ALLOC< double > > > > >
         thread_countings(
            nb_threads,
            ThreadData< std::vector< double, ALLOC< double > > >(counting_vect));

      // launch the threads
      // here we use openMP for launching the threads because, experimentally,
      // it seems to provide results that are twice as fast as the results
      // with the std::thread
      for (std::size_t i = std::size_t(0); i < nb_ranges; i += nb_threads) {
#  pragma omp parallel num_threads(int(nb_threads))
        {
          // get the number of the thread
          const std::size_t this_thread = getThreadNumber();
          if (this_thread + i < nb_ranges) {
            DBRowGeneratorParser< ALLOC >& parser = __parsers[this_thread].data;
            parser.setRange(__thread_ranges[this_thread + i].first,
                            __thread_ranges[this_thread + i].second);
            std::vector< double, ALLOC< double > >& countings =
               thread_countings[this_thread].data;

            // parse the database
            try {
              while (parser.hasRows()) {
                // get the observed rows
                const DBRow< DBTranslatedValue >& row = parser.row();

                // fill the counts for the current row
                std::size_t offset = std::size_t(0);
                for (std::size_t i = std::size_t(0); i < ids_size; ++i) {
                  offset +=
                     row[cols_offsets[i].first].discr_val * cols_offsets[i].second;
                }

                countings[offset] += row.weight();
              }
            } catch (NotFound&) {}   // this exception is raised by the row filter
                                     // if the row generators create no output row
                                     // from the last rows of the database
          }
        }
      }


      // add the counts to counting_vect
      for (std::size_t k = std::size_t(0); k < nb_threads; ++k) {
        const auto& thread_counting = thread_countings[k].data;
        for (std::size_t r = std::size_t(0); r < counting_vect_size; ++r) {
          counting_vect[r] += thread_counting[r];
        }
      }

      // save the final results
      __last_DB_ids = ids;
      __last_DB_countings = std::move(counting_vect);

      return __last_DB_countings;
    }


    /// the method used by threads to produce countings by parsing the database
    template < template < typename > class ALLOC >
    void RecordCounter< ALLOC >::__threadedCount(
       const std::size_t              begin,
       const std::size_t              end,
       DBRowGeneratorParser< ALLOC >& parser,
       const std::vector< std::pair< std::size_t, std::size_t >,
                          ALLOC< std::pair< std::size_t, std::size_t > > >&
                                               cols_offsets,
       std::vector< double, ALLOC< double > >& countings) {
      parser.setRange(begin, end);

      try {
        const std::size_t nb_columns = cols_offsets.size();
        while (parser.hasRows()) {
          // get the observed filtered rows
          const DBRow< DBTranslatedValue >& row = parser.row();

          // fill the counts for the current row
          std::size_t offset = std::size_t(0);
          for (std::size_t i = std::size_t(0); i < nb_columns; ++i) {
            offset +=
               row[cols_offsets[i].first].discr_val * cols_offsets[i].second;
          }

          countings[offset] += row.weight();
        }
      } catch (NotFound&) {}   // this exception is raised by the row filter if the
                               // row generators create no output row from the last
                               // rows of the database
    }


    /// checks that the ranges passed in argument are ok or raise an exception
    template < template < typename > class ALLOC >
    template < template < typename > class XALLOC >
    void RecordCounter< ALLOC >::__checkRanges(
       const std::vector< std::pair< std::size_t, std::size_t >,
                          XALLOC< std::pair< std::size_t, std::size_t > > >&
          new_ranges) const {
      const std::size_t dbsize = __parsers[0].data.database().nbRows();
      std::vector< std::pair< std::size_t, std::size_t >,
                   ALLOC< std::pair< std::size_t, std::size_t > > >
         incorrect_ranges;
      for (const auto& range : new_ranges) {
        if ((range.first >= range.second) || (range.second > dbsize)) {
          incorrect_ranges.push_back(range);
        }
      }
      if (!incorrect_ranges.empty()) {
        std::stringstream str;
        str << "It is impossible to set the ranges because the following one";
        if (incorrect_ranges.size() > 1)
          str << "s are incorrect: ";
        else
          str << " is incorrect: ";
        bool deja = false;
        for (const auto& range : incorrect_ranges) {
          if (deja)
            str << ", ";
          else
            deja = true;
          str << '[' << range.first << ';' << range.second << ')';
        }

        GUM_ERROR(OutOfBounds, str.str());
      }
    }


    /// sets the ranges within which each thread should perform its computations
    template < template < typename > class ALLOC >
    void RecordCounter< ALLOC >::__dispatchRangesToThreads() {
      __thread_ranges.clear();

      // ensure that __ranges contains the ranges asked by the user
      bool add_range = false;
      if (__ranges.empty()) {
        const auto& database = __parsers[0].data.database();
        __ranges.push_back(std::pair< std::size_t, std::size_t >(
           std::size_t(0), database.nbRows()));
        add_range = true;
      }

      // dispatch the ranges
      for (const auto& range : __ranges) {
        if (range.second > range.first) {
          const std::size_t range_size = range.second - range.first;
          std::size_t       nb_threads = range_size / __min_nb_rows_per_thread;
          if (nb_threads < 1)
            nb_threads = 1;
          else if (nb_threads > __max_nb_threads)
            nb_threads = __max_nb_threads;
          std::size_t nb_rows_par_thread = range_size / nb_threads;
          std::size_t rest_rows = range_size - nb_rows_par_thread * nb_threads;

          std::size_t begin_index = range.first;
          for (std::size_t i = std::size_t(0); i < nb_threads; ++i) {
            std::size_t end_index = begin_index + nb_rows_par_thread;
            if (rest_rows != std::size_t(0)) {
              ++end_index;
              --rest_rows;
            }
            __thread_ranges.push_back(
               std::pair< std::size_t, std::size_t >(begin_index, end_index));
            begin_index = end_index;
          }
        }
      }
      if (add_range) __ranges.clear();

      // sort ranges by decreasing range size, so that if the number of
      // ranges exceeds the number of threads allowed, we start a first round of
      // threads with the highest range, then another round with lower ranges,
      // and so on until all the ranges have been processed
      std::sort(__thread_ranges.begin(),
                __thread_ranges.end(),
                [](const std::pair< std::size_t, std::size_t >& a,
                   const std::pair< std::size_t, std::size_t >& b) -> bool {
                  return (a.second - a.first) > (b.second - b.first);
                });
    }


    /// sets new ranges to perform the countings
    template < template < typename > class ALLOC >
    template < template < typename > class XALLOC >
    void RecordCounter< ALLOC >::setRanges(
       const std::vector< std::pair< std::size_t, std::size_t >,
                          XALLOC< std::pair< std::size_t, std::size_t > > >&
          new_ranges) {
      // first, we check that all ranges are within the database's bounds
      __checkRanges(new_ranges);

      // since the ranges are OK, save them and clear the counting caches
      const std::size_t new_size = new_ranges.size();
      std::vector< std::pair< std::size_t, std::size_t >,
                   ALLOC< std::pair< std::size_t, std::size_t > > >
         ranges(new_size);
      for (std::size_t i = std::size_t(0); i < new_size; ++i) {
        ranges[i].first = new_ranges[i].first;
        ranges[i].second = new_ranges[i].second;
      }

      clear();
      __ranges = std::move(ranges);

      // dispatch the ranges to the threads
      __dispatchRangesToThreads();
    }


    /// reset the ranges to the one range corresponding to the whole database
    template < template < typename > class ALLOC >
    void RecordCounter< ALLOC >::clearRanges() {
      if (__ranges.empty()) return;
      clear();
      __ranges.clear();
      __dispatchRangesToThreads();
    }


    /// returns the current ranges
    template < template < typename > class ALLOC >
    INLINE const std::vector< std::pair< std::size_t, std::size_t >,
                              ALLOC< std::pair< std::size_t, std::size_t > > >&
                 RecordCounter< ALLOC >::ranges() const {
      return __ranges;
    }


    /// assign a new Bayes net to all the counter's generators depending on a BN
    template < template < typename > class ALLOC >
    template < typename GUM_SCALAR >
    INLINE void
       RecordCounter< ALLOC >::setBayesNet(const BayesNet< GUM_SCALAR >& new_bn) {
      // remove the caches
      clear();

      // assign the new BN
      for (auto& xparser : __parsers) {
        xparser.data.setBayesNet(new_bn);
      }
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
