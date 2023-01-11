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
 * @brief A class to execute several threads by exploiting openMP
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <agrum/tools/core/threadExecutorOMP.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {

  namespace threadsOMP {

    /// executes a function using several threads
    template < typename FUNCTION, typename... ARGS >
    void ThreadExecutor::execute(std::size_t nb_threads, FUNCTION exec_func, ARGS&&... func_args) {
#  ifndef _OPENMP
      // without openMP we only have one thread available
      exec_func(0, 1, std::forward< ARGS >(func_args)...);
#  else
      if (nb_threads <= 1) {
        exec_func(0, 1, std::forward< ARGS >(func_args)...);
      } else {
        // indicate that we start a new threadExecutor
        ++nbRunningThreadsExecutors_;

        // here, we shall create one std::exception_ptr for each openMP thread
        // that will be created. This will allow us to catch the exception raised
        // by the threads
        std::vector< std::exception_ptr > func_exceptions(nb_threads, nullptr);

        // launch the threads and wait for their completion
#    pragma omp parallel num_threads(int(nb_threads))
        {
          // get the number of the thread
          const std::size_t this_thread = omp_get_thread_num();

          try {
            exec_func(this_thread, nb_threads, std::forward< ARGS >(func_args)...);
          } catch (...) { func_exceptions[this_thread] = std::current_exception(); }
        }

        // now, we have completed the execution of the ThreadExecutor
        --nbRunningThreadsExecutors_;

        // now, check if one exception has been raised
        for (const auto& exc: func_exceptions) {
          if (exc != nullptr) { std::rethrow_exception(exc); }
        }
      }
#  endif   // _OPENMP
    }


    /// executes in parallel a function and undoes it if execptions are raised
    template < typename FUNC1, typename FUNC2, typename... ARGS >
    void ThreadExecutor::executeOrUndo(std::size_t nb_threads,
                                       FUNC1       exec_func,
                                       FUNC2       undo_func,
                                       ARGS&&... func_args) {
#  ifndef _OPENMP
      // without openMP we only have one thread available
      try {
        exec_func(0, 1, std::forward< ARGS >(func_args)...);
      } catch (...) {
        undo_func(0, 1, std::forward< ARGS >(func_args)...);
        throw;
      }
#  else
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

        // here, we shall create one std::exception_ptr for each thread openMP
        // that will be created. This will allow us to catch the exception raised
        // by the threads
        std::vector< std::exception_ptr > func_exceptions(nb_threads, nullptr);

        // launch the threads and waith for their completion
#    pragma omp parallel num_threads(int(nb_threads))
        {
          // get the number of the thread
          const std::size_t this_thread = getThreadNumber();

          try {
            exec_func(this_thread, nb_threads, std::forward< ARGS >(func_args)...);
          } catch (...) { func_exceptions[this_thread] = std::current_exception(); }
        }

        // now, check if one exception has been raised
        bool exception_raised = false;
        for (const auto& exc: func_exceptions) {
          if (exc != nullptr) {
            exception_raised = true;
            break;
          }
        }

        if (exception_raised) {
          // create the exceptions to catch during the repair threads executions
          std::vector< std::exception_ptr > undo_func_exceptions(nb_threads, nullptr);

          // launch the repair threads
#    pragma omp parallel num_threads(int(nb_threads))
          {
            // get the number of the thread
            const std::size_t this_thread = getThreadNumber();

            try {
              undo_func(this_thread, nb_threads, std::forward< ARGS >(func_args)...);
            } catch (...) { undo_func_exceptions[this_thread] = std::current_exception(); }
          }

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
#  endif   // _OPENMP
    }

  } /* namespace threadsOMP */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
