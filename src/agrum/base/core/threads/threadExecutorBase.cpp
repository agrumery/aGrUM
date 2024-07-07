/**
 *
 *   Copyright (c) 2005-2024  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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

#include <agrum/base/core/threads/threadExecutorBase.h>

// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#  include <agrum/base/core/threads/threadExecutorBase_inl.h>
#endif /* GUM_NO_INLINE */


namespace gum {

  // the number of currently running ThreadExecutors
  std::atomic< int > ThreadExecutorBase::nbRunningThreadsExecutors_{0};

}   // namespace gum
