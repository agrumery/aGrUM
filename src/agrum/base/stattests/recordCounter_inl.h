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
#pragma once


#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/base/stattests/recordCounter.h>

namespace gum {

  namespace learning {

    /// returns the minimum of rows that each thread should process
    INLINE std::size_t RecordCounter::minNbRowsPerThread() const {
      return _min_nb_rows_per_thread_;
    }

    /// returns the mapping from ids to column positions in the database
    INLINE const Bijection< NodeId, std::size_t >& RecordCounter::nodeId2Columns() const {
      return _nodeId2columns_;
    }

    /// returns the database on which we perform the counts
    INLINE const DatabaseTable& RecordCounter::database() const {
      return _parsers_[0].data.database();
    }

    /// returns the counts for a given set of nodes
    INLINE const std::vector< double >& RecordCounter::counts(const IdCondSet& ids,
                                                              const bool check_discrete_vars) {
      // if the idset is empty, return an empty vector
      if (ids.empty()) {
        _last_nonDB_ids_.clear();
        _last_nonDB_counting_.clear();
        return _last_nonDB_counting_;
      }

      // check whether we can extract the vector we wish to return from
      // some already computed counting vector
      if (_last_nonDB_ids_.contains(ids))
        return _extractFromCountings_(ids, _last_nonDB_ids_, _last_nonDB_counting_);
      else if (_last_DB_ids_.contains(ids))
        return _extractFromCountings_(ids, _last_DB_ids_, _last_DB_counting_);
      else {
        if (check_discrete_vars) _checkDiscreteVariables_(ids);
        return _countFromDatabase_(ids);
      }
    }

    /// returns the current ranges
    INLINE const std::vector< std::pair< std::size_t, std::size_t > >&
                 RecordCounter::ranges() const {
      return _ranges_;
    }

    /// sets the number max of threads that can be used
    INLINE void RecordCounter::setNumberOfThreads(Size nb) {
      ThreadNumberManager::setNumberOfThreads(nb);

      // redispatch the ranges to the threads
      _dispatchRangesToThreads_();
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
