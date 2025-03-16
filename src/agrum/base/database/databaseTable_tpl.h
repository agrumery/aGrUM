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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/





/** @file
 * @brief The implementation of tabular databases stored in memory (RAM)
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/base/database/databaseTable.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    // a method to process the rows of the database in multithreading
    template < typename Functor1, typename Functor2 >
    void DatabaseTable::_threadProcessDatabase_(Functor1& exec_func, Functor2& undo_func) {
      // compute the number of threads to execute the code
      const std::size_t nb_threads = this->nbProcessingThreads_();

      // if there is just one thread, let it process all the rows
      if (nb_threads == 1) {
        const std::size_t db_size = this->nbRows();
        exec_func(std::size_t(0), db_size, 0);
        return;
      }

      // if there are multiple threads, compute the ranges of rows they should process
      const std::vector< std::pair< std::size_t, std::size_t > > ranges
          = this->rangesProcessingThreads_(nb_threads);

      // here, we shall create the threads, but also one std::exception_ptr
      // for each thread. This will allow us to catch the exception raised
      // by the threads
      std::vector< std::thread > threads;
      threads.reserve(nb_threads);
      std::vector< std::exception_ptr > func_exceptions(nb_threads, nullptr);

      // create a lambda that will execute exec_func while catching its exceptions
      auto real_exec_func = [&exec_func](std::size_t         begin,
                                         std::size_t         end,
                                         std::size_t         index,
                                         std::exception_ptr& exc) -> void {
        try {
          exec_func(begin, end, index);
        } catch (...) { exc = std::current_exception(); }
      };

      // launch the threads
      for (std::size_t i = std::size_t(0); i < nb_threads; ++i) {
        threads.push_back(std::thread(std::ref(real_exec_func),
                                      ranges[i].first,
                                      ranges[i].second,
                                      i,
                                      std::ref(func_exceptions[i])));
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
        auto real_undo_func = [&undo_func](std::size_t         begin,
                                           std::size_t         end,
                                           std::size_t         index,
                                           std::exception_ptr& exc) -> void {
          try {
            undo_func(begin, end, index);
          } catch (...) { exc = std::current_exception(); }
        };

        // launch the repair threads
        threads.clear();
        std::vector< std::exception_ptr > undo_func_exceptions(nb_threads, nullptr);
        for (std::size_t i = std::size_t(0); i < nb_threads; ++i) {
          // we just need to repair the threads that did not raise exceptions
          if (func_exceptions[i] == nullptr)
            threads.push_back(std::thread(std::ref(real_undo_func),
                                          ranges[i].first,
                                          ranges[i].second,
                                          i,
                                          std::ref(undo_func_exceptions[i])));
        }

        // wait for the threads to complete their executions
        std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));

        // rethrow the exception
        for (const auto& exc: func_exceptions) {
          if (exc != nullptr) { std::rethrow_exception(exc); }
        }
      }
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
