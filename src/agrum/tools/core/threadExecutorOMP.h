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

/**
 * @file
 * @brief A class to execute several threads by exploiting openMP
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_THREAD_EXECUTOR_OMP_H
#define GUM_THREAD_EXECUTOR_OMP_H

#include <vector>
#include <exception>
#include <algorithm>
#include <functional>

#include <agrum/agrum.h>
#include <agrum/core/threadsOMP.h>

namespace gum {

  namespace threadsOMP {

    /** @class threadsOMP::ThreadExecutorImpl
     * @brief The class enables to uses openMP to execute callables in
     * parallel.
     *
     * The threadsOMP::ThreadExecutorImpl class is intended to easily
     * parallelize algorithms by providing a simple interface to launch the
     * threads. The class also catches exceptions raised by the threads, so that
     * those do not terminate prematurely the program. It can then rethrow one
     * such exception in a single-threaded manner, so that it can be caught by
     * the main thread.
     *
     * @warning This class is not intended to be used explicitly by the user.
     * It is preferrable to use class ThreadExecutor, which will point to the
     * threadsOMP::ThreadExecutorImpl class if the threads infrastructure to
     * use is openMP.
     *
     * @par Usage example:
     * @code
     * @endcode
     */
    struct ThreadExecutorImpl {

      /// executes a function using several threads
      template <typename FUNCTION, typename... ARGS>
      static void execute ( std::size_t nb_threads,
                            FUNCTION    exec_func,
                            ARGS&&...   func_args );
      

      /// executes in parallel a function and undoes it if exceptions are raised
      template <typename FUNC1, typename FUNC2, typename... ARGS>
      static void executeOrUndo ( std::size_t nb_threads,
                                  FUNC1       exec_func,
                                  FUNC2       undo_func,
                                  ARGS&&...   func_args );
    };

  } /* namespace threadsOMP */
    
} /* namespace gum */


/// always include the templated implementations
#include <agrum/core/threadExecutorOMP_tpl.h>

#endif /* GUM_THREAD_EXECUTOR_OMP_H */
