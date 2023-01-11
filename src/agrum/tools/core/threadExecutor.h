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
 * @brief The class to use to execute a function by several threads
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_THREAD_EXECUTOR_H
#define GUM_THREAD_EXECUTOR_H

#include <agrum/agrum.h>
#include <agrum/tools/core/threadExecutorSTL.h>
#include <agrum/tools/core/threadExecutorOMP.h>

namespace gum {

  // use either the OMP or the STL thread executor, depending on the --threads
  // aGrUM's compilation option
  // @TODO substitute the lines below by
  // using ThreadExecutor = GUM_THREADS::ThreadExecutor;
  // when swig will support it
#ifdef GUM_THREADS_USE_OMP
  using ThreadExecutor = threadsOMP::ThreadExecutor;
#else
  using ThreadExecutor = threadsSTL::ThreadExecutor;
#endif

} /* namespace gum */

#endif /* GUM_THREAD_EXECUTOR_H */
