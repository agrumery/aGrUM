/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
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
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
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
#include <algorithm>
#include <exception>
#include <functional>
#include <thread>
#include <tuple>
#include <vector>

#include <agrum/base/core/threads/threadExecutorSTL.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {

  namespace threadsSTL {

    /// executes a function using several threads
    template < typename FUNCTION, typename... ARGS >
    void ThreadExecutor::execute(std::size_t nb_threads, FUNCTION exec_func, ARGS&&... func_args) {
      if (nb_threads <= 1) {
        exec_func(0, 1, std::forward< ARGS >(func_args)...);
      } else {
        // indicate that we start a new threadExecutor
        ++nbRunningThreadsExecutors_;

        // here, we shall create the threads, but also one std::exception_ptr
        // for each thread. This will allow us to catch the exception raised
        // by the threads
        std::vector< std::thread > threads;
        threads.reserve(nb_threads);
        std::vector< std::exception_ptr > func_exceptions(nb_threads, nullptr);

        // create a lambda that will execute exec_func while catching its exceptions
        auto real_exec_func = [&exec_func, nb_threads](std::size_t         this_thread,
                                                       std::exception_ptr& exc,
                                                       ARGS&... args) -> void {
          try {
            exec_func(this_thread, nb_threads, std::forward< ARGS >(args)...);
          } catch (...) { exc = std::current_exception(); }
        };

        // launch the threads
        for (std::size_t i = std::size_t(0); i < nb_threads; ++i) {
          threads.push_back(
              std::thread(real_exec_func, i, std::ref(func_exceptions[i]), std::ref(func_args)...));
          //                                     std::ref(std::forward< ARGS >(func_args))...));
        }

        // wait for the threads to complete their executions
        std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));

        // now, we have completed the execution of the ThreadExecutor
        --nbRunningThreadsExecutors_;

        // now, check if one exception has been raised
        for (const auto& exc: func_exceptions) {
          if (exc != nullptr) { std::rethrow_exception(exc); }
        }
      }
    }

    /// executes in parallel a function and undoes it if execptions are raised
    template < typename FUNC1, typename FUNC2, typename... ARGS >
    void ThreadExecutor::executeOrUndo(std::size_t nb_threads,
                                       FUNC1       exec_func,
                                       FUNC2       undo_func,
                                       ARGS&&... func_args) {
      if (nb_threads <= 1) {
        try {
          exec_func(0, 1, std::forward< ARGS >(func_args)...);
        } catch (...) {
          undo_func(0, 1, std::forward< ARGS >(func_args)...);
          throw;
        }
      } else {
        // indicate that we start a new threadExecutor
        ++nbRunningThreadsExecutors_;

        // here, we shall create the threads, but also one std::exception_ptr
        // for each thread. This will allow us to catch the exception raised
        // by the threads
        std::vector< std::thread > threads;
        threads.reserve(nb_threads);
        std::vector< std::exception_ptr > func_exceptions(nb_threads, nullptr);

        // create a lambda that will execute exec_func while catching its exceptions
        auto real_exec_func = [&exec_func, nb_threads](std::size_t         this_thread,
                                                       std::exception_ptr& exc,
                                                       ARGS&... args) -> void {
          try {
            exec_func(this_thread, nb_threads, std::forward< ARGS >(args)...);
          } catch (...) { exc = std::current_exception(); }
        };


        // launch the threads
        for (std::size_t i = std::size_t(0); i < nb_threads; ++i) {
          threads.push_back(
              std::thread(real_exec_func, i, std::ref(func_exceptions[i]), std::ref(func_args)...));
          //                                     std::ref(std::forward< ARGS >(func_args))...));
        }

        // wait for the threads to complete their executions
        std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));

        // now, check if one exception has been raised
        bool exception_raised = false;
        for (const auto& exc: func_exceptions) {
          if (exc != nullptr) {
            exception_raised = true;
            break;
          }
        }


        if (exception_raised) {
          // create a lambda that will execute undo_func while catching
          // its exceptions
          auto real_undo_func = [&undo_func, nb_threads](std::size_t         this_thread,
                                                         std::exception_ptr& exc,
                                                         ARGS&... args) -> void {
            try {
              undo_func(this_thread, nb_threads, std::forward< ARGS >(args)...);
            } catch (...) { exc = std::current_exception(); }
          };

          // launch the repair threads
          threads.clear();
          std::vector< std::exception_ptr > undo_func_exceptions(nb_threads, nullptr);
          for (std::size_t i = std::size_t(0); i < nb_threads; ++i) {
            // we just need to repair the threads that did not raise exceptions
            if (func_exceptions[i] == nullptr)
              threads.push_back(std::thread(real_undo_func,
                                            i,
                                            std::ref(undo_func_exceptions[i]),
                                            std::ref(func_args)...));
            // std::ref(std::forward< ARGS >(func_args))...));
          }

          // wait for the threads to complete their executions
          std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));

          // now, we have completed the execution of the ThreadExecutor
          --nbRunningThreadsExecutors_;

          // rethrow the exception
          for (const auto& exc: func_exceptions) {
            if (exc != nullptr) { std::rethrow_exception(exc); }
          }
        } else {
          // now, we have completed the execution of the ThreadExecutor
          --nbRunningThreadsExecutors_;
        }
      }
    }


  } /* namespace threadsSTL */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
