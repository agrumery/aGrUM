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


/**
 * @file
 * @brief A class to execute several threads by exploiting std::thread
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_THREAD_EXECUTOR_STL_H
#define GUM_THREAD_EXECUTOR_STL_H

#include <agrum/agrum.h>

#include <agrum/base/core/threads/threadExecutorBase.h>
#include <agrum/base/core/threads/threadsSTL.h>

namespace gum {

  namespace threadsSTL {

    /** @class threadsSTL::ThreadExecutor
     * @brief The class enables to launch std::threads to execute callables in
     * parallel.
     * @headerfile threadExecutorSTL.h <agrum/base/core/threads/threadExecutorSTL.h>
     *
     * The threadsSTL::ThreadExecutor class is intended to easily parallelize
     * algorithms by providing a simple interface to launch the threads. The
     * class also catches exceptions raised by the threads, so that those do
     * not terminate prematurely the program. It can then rethrow one such
     * exception in a single-threaded manner, so that it can be caught by the
     * main thread.
     *
     * @warning This class is not intended to be used explicitly by the user.
     * It is preferable to use class threads::ThreadExecutor, which will point to
     * the threadsSTL::ThreadExecutor class if the threads infrastructure to
     * use is STL.
     *
     * @par Usage example:
     * @code
     * @endcode
     */
    struct ThreadExecutor: private ThreadExecutorBase {
      /// indicates how many threadExecutors are currently running
      using ThreadExecutorBase::nbRunningThreadsExecutors;

      /// executes a function using several threads
      template < typename FUNCTION, typename... ARGS >
      static void execute(std::size_t nb_threads, FUNCTION exec_func, ARGS&&... func_args);


      /// executes in parallel a function and undoes it if execptions are raised
      template < typename FUNC1, typename FUNC2, typename... ARGS >
      static void executeOrUndo(std::size_t nb_threads,
                                FUNC1       exec_func,
                                FUNC2       undo_func,
                                ARGS&&... func_args);
    };

  } /* namespace threadsSTL */

} /* namespace gum */

/// always include the templated implementations
#include <agrum/base/core/threads/threadExecutorSTL_tpl.h>

#endif /* GUM_THREAD_EXECUTOR_STL_H */
