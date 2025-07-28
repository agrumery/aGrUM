/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief The class that computes counts of observations from the database.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/base/stattests/recordCounter.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/base/stattests/recordCounter_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    RecordCounter::RecordCounter(const DBRowGeneratorParser&                                 parser,
                                 const std::vector< std::pair< std::size_t, std::size_t > >& ranges,
                                 const Bijection< NodeId, std::size_t >& nodeId2columns) :
        _nodeId2columns_(nodeId2columns) {
      // check that the columns in nodeId2columns do belong to the database
      const std::size_t db_nb_cols = parser.database().nbVariables();
      for (auto iter = nodeId2columns.cbegin(); iter != nodeId2columns.cend(); ++iter) {
        if (iter.second() >= db_nb_cols) {
          GUM_ERROR(OutOfBounds,
                    "the mapping between ids and database columns "
                        << "is incorrect because Column " << iter.second()
                        << " does not belong to the database.");
        }
      }

      // create the parsers. There should always be at least one parser
      const auto max_nb_threads = ThreadNumberManager::getNumberOfThreads();
      _parsers_.reserve(max_nb_threads);
      for (std::size_t i = std::size_t(0); i < max_nb_threads; ++i)
        _parsers_.push_back(parser);

      // check that the ranges are within the bounds of the database and
      // save them
      _checkRanges_(ranges);
      _ranges_.reserve(ranges.size());
      for (const auto& range: ranges)
        _ranges_.push_back(range);

      // dispatch the ranges for the threads
      _dispatchRangesToThreads_();

      GUM_CONSTRUCTOR(RecordCounter);
    }

    /// default constructor
    RecordCounter::RecordCounter(const DBRowGeneratorParser&             parser,
                                 const Bijection< NodeId, std::size_t >& nodeId2columns) :
        RecordCounter(parser,
                      std::vector< std::pair< std::size_t, std::size_t > >(),
                      nodeId2columns) {}

    /// copy constructor
    RecordCounter::RecordCounter(const RecordCounter& from) :
        ThreadNumberManager(from), _parsers_(from._parsers_), _ranges_(from._ranges_),
        _thread_ranges_(from._thread_ranges_), _nodeId2columns_(from._nodeId2columns_),
        _last_DB_counting_(from._last_DB_counting_), _last_DB_ids_(from._last_DB_ids_),
        _last_nonDB_counting_(from._last_nonDB_counting_), _last_nonDB_ids_(from._last_nonDB_ids_),
        _min_nb_rows_per_thread_(from._min_nb_rows_per_thread_) {
      GUM_CONS_CPY(RecordCounter);
    }

    /// move constructor
    RecordCounter::RecordCounter(RecordCounter&& from) :
        ThreadNumberManager(std::move(from)), _parsers_(std::move(from._parsers_)),
        _ranges_(std::move(from._ranges_)), _thread_ranges_(std::move(from._thread_ranges_)),
        _nodeId2columns_(std::move(from._nodeId2columns_)),
        _last_DB_counting_(std::move(from._last_DB_counting_)),
        _last_DB_ids_(std::move(from._last_DB_ids_)),
        _last_nonDB_counting_(std::move(from._last_nonDB_counting_)),
        _last_nonDB_ids_(std::move(from._last_nonDB_ids_)),
        _min_nb_rows_per_thread_(from._min_nb_rows_per_thread_) {
      GUM_CONS_MOV(RecordCounter);
    }

    /// virtual copy constructor
    RecordCounter* RecordCounter::clone() const { return new RecordCounter(*this); }

    /// destructor
    RecordCounter::~RecordCounter() { GUM_DESTRUCTOR(RecordCounter); }

    /// copy operator
    RecordCounter& RecordCounter::operator=(const RecordCounter& from) {
      if (this != &from) {
        ThreadNumberManager::operator=(from);
        _parsers_                = from._parsers_;
        _ranges_                 = from._ranges_;
        _thread_ranges_          = from._thread_ranges_;
        _nodeId2columns_         = from._nodeId2columns_;
        _last_DB_counting_       = from._last_DB_counting_;
        _last_DB_ids_            = from._last_DB_ids_;
        _last_nonDB_counting_    = from._last_nonDB_counting_;
        _last_nonDB_ids_         = from._last_nonDB_ids_;
        _min_nb_rows_per_thread_ = from._min_nb_rows_per_thread_;
      }
      return *this;
    }

    /// move operator
    RecordCounter& RecordCounter::operator=(RecordCounter&& from) {
      if (this != &from) {
        ThreadNumberManager::operator=(std::move(from));
        _parsers_                = std::move(from._parsers_);
        _ranges_                 = std::move(from._ranges_);
        _thread_ranges_          = std::move(from._thread_ranges_);
        _nodeId2columns_         = std::move(from._nodeId2columns_);
        _last_DB_counting_       = std::move(from._last_DB_counting_);
        _last_DB_ids_            = std::move(from._last_DB_ids_);
        _last_nonDB_counting_    = std::move(from._last_nonDB_counting_);
        _last_nonDB_ids_         = std::move(from._last_nonDB_ids_);
        _min_nb_rows_per_thread_ = from._min_nb_rows_per_thread_;
      }
      return *this;
    }

    /// clears all the last database-parsed counts from memory
    void RecordCounter::clear() {
      _last_DB_counting_.clear();
      _last_DB_ids_.clear();
      _last_nonDB_counting_.clear();
      _last_nonDB_ids_.clear();
    }

    // changes the number min of rows a thread should process in a
    // multithreading context
    void RecordCounter::setMinNbRowsPerThread(const std::size_t nb) const {
      if (nb == std::size_t(0)) _min_nb_rows_per_thread_ = std::size_t(1);
      else _min_nb_rows_per_thread_ = nb;
    }

    /// compute and raise the exception when some variables are continuous
    void RecordCounter::_raiseCheckException_(const std::vector< std::string >& bad_vars) const {
      // generate the exception
      std::stringstream msg;
      msg << "Counts cannot be performed on continuous variables. ";
      msg << "Unfortunately the following variable";
      if (bad_vars.size() == 1) msg << " is continuous: " << bad_vars[0];
      else {
        msg << "s are continuous: ";
        bool deja = false;
        for (const auto& name: bad_vars) {
          if (deja) msg << ", ";
          else deja = true;
          msg << name;
        }
      }
      GUM_ERROR(TypeError, msg.str())
    }

    /// check that all the variables in an idset are discrete
    void RecordCounter::_checkDiscreteVariables_(const IdCondSet& ids) const {
      const std::size_t    size     = ids.size();
      const DatabaseTable& database = _parsers_[0].data.database();

      if (_nodeId2columns_.empty()) {
        // check all the ids
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          if (database.variable(i).varType() == VarType::CONTINUOUS) {
            // here, var i does not correspond to a discrete variable.
            // we check whether there are other non discrete variables, so that
            // we can generate an exception mentioning all these variables
            std::vector< std::string > bad_vars{database.variable(i).name()};
            for (++i; i < size; ++i) {
              if (database.variable(i).varType() == VarType::CONTINUOUS)
                bad_vars.push_back(database.variable(i).name());
            }
            _raiseCheckException_(bad_vars);
          }
        }
      } else {
        // check all the ids
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          // get the position of the variable in the database
          std::size_t pos = _nodeId2columns_.second(ids[i]);

          if (database.variable(pos).varType() == VarType::CONTINUOUS) {
            // here, id does not correspond to a discrete variable.
            // we check whether there are other non discrete variables, so that
            // we can generate an exception mentioning all these variables
            std::vector< std::string > bad_vars{database.variable(pos).name()};
            for (++i; i < size; ++i) {
              pos = _nodeId2columns_.second(ids[i]);
              if (database.variable(pos).varType() == VarType::CONTINUOUS)
                bad_vars.push_back(database.variable(pos).name());
            }
            _raiseCheckException_(bad_vars);
          }
        }
      }
    }

    // returns a mapping from the nodes ids to the columns of the database
    // for a given sequence of ids
    HashTable< NodeId, std::size_t >
        RecordCounter::_getNodeIds2Columns_(const IdCondSet& ids) const {
      HashTable< NodeId, std::size_t > res(ids.size());
      if (_nodeId2columns_.empty()) {
        for (const auto id: ids) {
          res.insert(id, std::size_t(id));
        }
      } else {
        for (const auto id: ids) {
          res.insert(id, _nodeId2columns_.second(id));
        }
      }
      return res;
    }

    /// extracts some new counts from previously computed ones
    std::vector< double >&
        RecordCounter::_extractFromCountings_(const IdCondSet&             subset_ids,
                                              const IdCondSet&             superset_ids,
                                              const std::vector< double >& superset_vect) {
      // get a mapping between the node Ids and their columns in the database.
      // This should be stored into _nodeId2columns_, except if the latter is
      // empty, in which case there is an identity mapping
      const auto nodeId2columns = _getNodeIds2Columns_(superset_ids);

      // we first determine the size of the output vector, the domain of
      // each of its variables and their offsets in the output vector
      const auto& database         = _parsers_[0].data.database();
      std::size_t result_vect_size = std::size_t(1);
      for (const auto id: subset_ids) {
        result_vect_size *= database.domainSize(nodeId2columns[id]);
      }

      // we create the output vector
      std::vector< double > result_vect(result_vect_size, 0.0);

      // check if the subset_ids is the beginning of the sequence of superset_ids
      // if this is the case, then we can outer loop over the variables not in
      // subset_ids and, for each iteration of this loop add a vector of size
      // result_size to result_vect
      bool              subset_begin    = true;
      const std::size_t subset_ids_size = std::size_t(subset_ids.size());
      for (std::size_t i = 0; i < subset_ids_size; ++i) {
        if (superset_ids.pos(subset_ids[i]) != i) {
          subset_begin = false;
          break;
        }
      }

      if (subset_begin) {
        const std::size_t superset_vect_size = superset_vect.size();
        std::size_t       i                  = std::size_t(0);
        while (i < superset_vect_size) {
          for (std::size_t j = std::size_t(0); j < result_vect_size; ++j, ++i) {
            result_vect[j] += superset_vect[i];
          }
        }

        // save the subset_ids and the result vector
        try {
          _last_nonDB_ids_      = subset_ids;
          _last_nonDB_counting_ = std::move(result_vect);
          return _last_nonDB_counting_;
        } catch (...) {
          _last_nonDB_ids_.clear();
          _last_nonDB_counting_.clear();
          throw;
        }
      }


      // check if subset_ids is the end of the sequence of superset_ids.
      // In this case, as above, there are two simple loops to perform the
      // counts
      bool              subset_end        = true;
      const std::size_t superset_ids_size = std::size_t(superset_ids.size());
      for (std::size_t i = 0; i < subset_ids_size; ++i) {
        if (superset_ids.pos(subset_ids[i]) != i + superset_ids_size - subset_ids_size) {
          subset_end = false;
          break;
        }
      }

      if (subset_end) {
        // determine the size of the vector corresponding to the variables
        // not belonging to subset_ids
        std::size_t vect_not_subset_size = std::size_t(1);
        for (std::size_t i = std::size_t(0); i < superset_ids_size - subset_ids_size; ++i)
          vect_not_subset_size *= database.domainSize(nodeId2columns[superset_ids[i]]);

        // perform the two loops
        std::size_t i = std::size_t(0);
        for (std::size_t j = std::size_t(0); j < result_vect_size; ++j) {
          for (std::size_t k = std::size_t(0); k < vect_not_subset_size; ++k, ++i) {
            result_vect[j] += superset_vect[i];
          }
        }

        // save the subset_ids and the result vector
        try {
          _last_nonDB_ids_      = subset_ids;
          _last_nonDB_counting_ = std::move(result_vect);
          return _last_nonDB_counting_;
        } catch (...) {
          _last_nonDB_ids_.clear();
          _last_nonDB_counting_.clear();
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
        std::size_t                tmp_before_incr    = std::size_t(1);
        std::vector< std::size_t > superset_order(subset_ids_size);

        for (std::size_t h = std::size_t(0), j = std::size_t(0); j < subset_ids_size; ++h) {
          if (subset_ids.exists(superset_ids[h])) {
            before_incr[j]                                  = tmp_before_incr - 1;
            superset_order[subset_ids.pos(superset_ids[h])] = j;
            tmp_before_incr                                 = 1;
            ++j;
          } else {
            tmp_before_incr *= database.domainSize(nodeId2columns[superset_ids[h]]);
          }
        }

        // compute the offsets in the order of the superset_ids
        for (std::size_t i = 0; i < subset_ids.size(); ++i) {
          const std::size_t domain_size = database.domainSize(nodeId2columns[subset_ids[i]]);
          const std::size_t j           = superset_order[i];
          result_domain[j]              = domain_size;
          result_offset[j]              = result_domain_size;
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
      std::size_t       the_result_offset  = std::size_t(0);
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
        _last_nonDB_ids_      = subset_ids;
        _last_nonDB_counting_ = std::move(result_vect);
        return _last_nonDB_counting_;
      } catch (...) {
        _last_nonDB_ids_.clear();
        _last_nonDB_counting_.clear();
        throw;
      }
    }

    /// parse the database to produce new counts
    std::vector< double >& RecordCounter::_countFromDatabase_(const IdCondSet& ids) {
      // if the ids vector is empty or the database is empty, return an
      // empty vector
      const auto& database = _parsers_[0].data.database();
      if (ids.empty() || database.empty() || _thread_ranges_.empty()) {
        _last_nonDB_counting_.clear();
        _last_nonDB_ids_.clear();
        return _last_nonDB_counting_;
      }

      // we translate the ids into their corresponding columns in the
      // DatabaseTable
      const auto nodeId2columns = _getNodeIds2Columns_(ids);

      // we first determine the size of the counting vector, the domain of
      // each of its variables and their offsets in the output vector
      const std::size_t                                    ids_size           = ids.size();
      std::size_t                                          counting_vect_size = std::size_t(1);
      std::vector< std::size_t >                           domain_sizes(ids_size);
      std::vector< std::pair< std::size_t, std::size_t > > cols_offsets(ids_size);
      {
        std::size_t i = std::size_t(0);
        for (const auto id: ids) {
          const std::size_t domain_size = database.domainSize(nodeId2columns[id]);
          domain_sizes[i]               = domain_size;
          cols_offsets[i].first         = nodeId2columns[id];
          cols_offsets[i].second        = counting_vect_size;
          counting_vect_size *= domain_size;
          ++i;
        }
      }

      // we sort the columns and offsets by increasing column index. This
      // may speed up threaded counts by improving the cacheline hits
      std::sort(
          cols_offsets.begin(),
          cols_offsets.end(),
          [](const std::pair< std::size_t, std::size_t >& a,
             const std::pair< std::size_t, std::size_t >& b) -> bool { return a.first < b.first; });

      // create parsers if needed
      const std::size_t nb_ranges      = _thread_ranges_.size();
      const auto        max_nb_threads = ThreadNumberManager::getNumberOfThreads();
      const std::size_t nb_threads     = nb_ranges <= max_nb_threads ? nb_ranges : max_nb_threads;
      while (_parsers_.size() < nb_threads) {
        ThreadData< DBRowGeneratorParser > new_parser(_parsers_[0]);
        _parsers_.push_back(std::move(new_parser));
      }

      // set the columns of interest for each parser. This specifies to the
      // parser which columns are used for the counts. This is important
      // for parsers like the EM parser that complete unobserved variables.
      std::vector< std::size_t > cols_of_interest(ids_size);
      for (std::size_t i = std::size_t(0); i < ids_size; ++i) {
        cols_of_interest[i] = cols_offsets[i].first;
      }
      for (auto& parser: _parsers_) {
        parser.data.setColumnsOfInterest(cols_of_interest);
      }

      // allocate all the counting vectors, including that which will add
      // all the results provided by the threads. We initialize once and
      // for all these vectors with zeroes
      std::vector< double >                              counting_vect(counting_vect_size, 0.0);
      std::vector< ThreadData< std::vector< double > > > thread_countings(
          nb_threads,
          ThreadData< std::vector< double > >(counting_vect));

      // here, we create a lambda that will be executed by all the threads
      // to perform the counts in a parallel manner
      auto threadedCount = [this, nb_ranges, ids_size, &thread_countings, cols_offsets](
                               const std::size_t this_thread,
                               const std::size_t nb_threads,
                               const std::size_t nb_loop) -> void {
        if (this_thread + nb_loop < nb_ranges) {
          // get the database parser and the contingency table to fill
          DBRowGeneratorParser& parser = this->_parsers_[this_thread].data;
          parser.setRange(this->_thread_ranges_[this_thread + nb_loop].first,
                          this->_thread_ranges_[this_thread + nb_loop].second);
          std::vector< double >& counts = thread_countings[this_thread].data;

          // parse the database
          try {
            while (parser.hasRows()) {
              // get the observed rows
              const DBRow< DBTranslatedValue >& row = parser.row();

              // fill the counts for the current row
              std::size_t offset = std::size_t(0);
              for (std::size_t i = std::size_t(0); i < ids_size; ++i) {
                offset += row[cols_offsets[i].first].discr_val * cols_offsets[i].second;
              }

              counts[offset] += row.weight();
            }
          } catch (NotFound const&) {}   // this exception is raised by the row filter
                                         // if the row generators create no output row
                                         // from the last rows of the database
        }
      };


      // launch the threads
      for (std::size_t i = std::size_t(0); i < nb_ranges; i += nb_threads) {
        ThreadExecutor::execute(nb_threads, threadedCount, i);
      }


      // add the counts to counting_vect
      for (std::size_t k = std::size_t(0); k < nb_threads; ++k) {
        const auto& thread_counting = thread_countings[k].data;
        for (std::size_t r = std::size_t(0); r < counting_vect_size; ++r) {
          counting_vect[r] += thread_counting[r];
        }
      }

      // save the final results
      _last_DB_ids_      = ids;
      _last_DB_counting_ = std::move(counting_vect);

      return _last_DB_counting_;
    }

    /// checks that the ranges passed in argument are ok or raise an exception
    void RecordCounter::_checkRanges_(
        const std::vector< std::pair< std::size_t, std::size_t > >& new_ranges) const {
      const std::size_t dbsize = _parsers_[0].data.database().nbRows();
      std::vector< std::pair< std::size_t, std::size_t > > incorrect_ranges;
      for (const auto& range: new_ranges) {
        if ((range.first >= range.second) || (range.second > dbsize)) {
          incorrect_ranges.push_back(range);
        }
      }
      if (!incorrect_ranges.empty()) {
        std::stringstream str;
        str << "It is impossible to set the ranges because the following one";
        if (incorrect_ranges.size() > 1) str << "s are incorrect: ";
        else str << " is incorrect: ";
        bool deja = false;
        for (const auto& range: incorrect_ranges) {
          if (deja) str << ", ";
          else deja = true;
          str << '[' << range.first << ';' << range.second << ')';
        }

        GUM_ERROR(OutOfBounds, str.str())
      }
    }

    /// sets the ranges within which each thread should perform its computations
    void RecordCounter::_dispatchRangesToThreads_() {
      _thread_ranges_.clear();

      // ensure that  _ranges_ contains the ranges asked by the user
      bool add_range = false;
      if (_ranges_.empty()) {
        const auto& database = _parsers_[0].data.database();
        _ranges_.push_back(
            std::pair< std::size_t, std::size_t >(std::size_t(0), database.nbRows()));
        add_range = true;
      }

      // dispatch the ranges
      const auto max_nb_threads = ThreadNumberManager::getNumberOfThreads();
      for (const auto& range: _ranges_) {
        if (range.second > range.first) {
          const std::size_t range_size = range.second - range.first;
          std::size_t       nb_threads = range_size / _min_nb_rows_per_thread_;
          if (nb_threads < 1) nb_threads = 1;
          else if (nb_threads > max_nb_threads) nb_threads = max_nb_threads;
          std::size_t nb_rows_par_thread = range_size / nb_threads;
          std::size_t rest_rows          = range_size - nb_rows_par_thread * nb_threads;

          std::size_t begin_index = range.first;
          for (std::size_t i = std::size_t(0); i < nb_threads; ++i) {
            std::size_t end_index = begin_index + nb_rows_par_thread;
            if (rest_rows != std::size_t(0)) {
              ++end_index;
              --rest_rows;
            }
            _thread_ranges_.push_back(
                std::pair< std::size_t, std::size_t >(begin_index, end_index));
            begin_index = end_index;
          }
        }
      }
      if (add_range) _ranges_.clear();

      // sort ranges by decreasing range size, so that if the number of
      // ranges exceeds the number of threads allowed, we start a first round of
      // threads with the highest range, then another round with lower ranges,
      // and so on until all the ranges have been processed
      std::sort(_thread_ranges_.begin(),
                _thread_ranges_.end(),
                [](const std::pair< std::size_t, std::size_t >& a,
                   const std::pair< std::size_t, std::size_t >& b) -> bool {
                  return (a.second - a.first) > (b.second - b.first);
                });
    }

    /// sets new ranges to perform the counts
    void RecordCounter::setRanges(
        const std::vector< std::pair< std::size_t, std::size_t > >& new_ranges) {
      // first, we check that all ranges are within the database's bounds
      _checkRanges_(new_ranges);

      // since the ranges are OK, save them and clear the counting caches
      const std::size_t                                    new_size = new_ranges.size();
      std::vector< std::pair< std::size_t, std::size_t > > ranges(new_size);
      for (std::size_t i = std::size_t(0); i < new_size; ++i) {
        ranges[i].first  = new_ranges[i].first;
        ranges[i].second = new_ranges[i].second;
      }

      clear();
      _ranges_ = std::move(ranges);

      // dispatch the ranges to the threads
      _dispatchRangesToThreads_();
    }

    /// reset the ranges to the one range corresponding to the whole database
    void RecordCounter::clearRanges() {
      if (_ranges_.empty()) return;
      clear();
      _ranges_.clear();
      _dispatchRangesToThreads_();
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
