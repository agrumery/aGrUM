/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
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
 * @brief The base class used by all thread executors
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef AGRUM_THREADEXECUTOR_BASE_H
#define AGRUM_THREADEXECUTOR_BASE_H

#include <atomic>

#include <agrum/agrum.h>

namespace gum {

  /**
   * @class ThreadExecutorBase
   * @brief The base class used by all thread executors
   * @headerfile threadExecutorBase.h <agrum/tools/core/threadExecutorBase.h>
   */
  class ThreadExecutorBase
  /**
   * @brief Set the max number of threads to be used.
   * @ingroup basicstruct_group
   *
   * To avoid spare cycles (less then 100% CPU occupied), use more threads than
   * logical processors (x2 is a good all-around value).
   * @param number The number of threads to be used.
   */
  {
    public:
    /// indicates how many threadExecutors are currently running
    static int nbRunningThreadsExecutors();

    protected:
    /// he number of currently running ThreadExecutors
    static std::atomic< int > nbRunningThreadsExecutors_;
  };

} /* namespace gum */


// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/tools/core/threadExecutorBase_inl.h>
#endif /* GUM_NO_INLINE */


#endif   // AGRUM_THREADEXECUTOR_BASE_H
