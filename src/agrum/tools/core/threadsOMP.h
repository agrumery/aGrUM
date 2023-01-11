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
 * @brief Wrappers for OpenMP.
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_THREADS_OPEMMP_H
#define GUM_THREADS_OPEMMP_H

#ifdef _OPENMP
#  include <omp.h>
#endif

#include <thread>
#include <vector>
#include <utility>

#include <agrum/agrum.h>

namespace gum {

  namespace threadsOMP {

    /**
     * @brief Is OMP active ?
     * @ingroup basicstruct_group
     * @return \c True if OMP has been set at compilation, \c False otherwise.
     */
    bool hasOMPSupport();

    /**
     * @brief Returns the maximum number of threads that should be used at any time.
     * @ingroup basicstruct_group
     *
     * By default, it should be the number of CPU cores available. Note that it
     * is preferable to use function getNumberOfThreads(), which corresponds to the
     * max number of threads the user wants to launch in parallel regions (this
     * is by default equal to getMaxNumberOfThreads(), but the user may wish to
     * change it for some reason).
     *
     * @return Returns the maximum number of threads at any time.
     */
    unsigned int getMaxNumberOfThreads();

    /**
     * @brief returns the max number of threads used by default when entering the
     * next parallel region
     *
     * This is the number of threads launched in parallel regions. By default, this
     * number is equal to getMaxNumberOfThreads() but if the user has
     * changed it using method setNumberOfThreads, then this number is what the
     * user required.
     *
     * @return the number of threads used by default the next time we enter into
     * a parallel region
     */
    unsigned int getNumberOfThreads();

    /**
     * @brief Get the number of logical processors.
     * @ingroup basicstruct_group
     * @return The number of logical processors.
     */
    unsigned int getNumberOfLogicalProcessors();

    /**
     * @brief Set the number of threads to be used when entering the next
     * parallel region.
     * @ingroup basicstruct_group
     *
     * To avoid spare cycles (less then 100% CPU occupied), you may want to use more
     * threads than logical processors (x2 could be a good all-around value).
     * @param number The number of threads to be used in the next parallel region.
     */
    void setNumberOfThreads(unsigned int number);

    /**
     * @brief Get the calling thread id.
     * @ingroup basicstruct_group
     *
     * Call this from a parallel region.
     * @return The calling thread id.
     */
    unsigned int getThreadNumber();

    /**
     * @brief Get the current number of running threads.
     * @ingroup basicstruct_group
     *
     * Call this from a parallel region.
     * @return The current number of running threads.
     */
    unsigned int getNumberOfRunningThreads();

    /**
     * @brief Set nested parallelism (false bu default).
     * @ingroup basicstruct_group
     *
     * Nested parallelism, i.e. parallel activity within another parallel
     * activity : threads creating more threads.
     *
     * Off by default.
     *
     * @param value \c True if nested parallelism should be activated, \c False
     * otherwise.
     */
    void setNestedParallelism(bool value);

    /**
     * @brief Get nested parallelism status.
     * @ingroup basicstruct_group
     * @return \c True if nested parallelism is enabled, \c False otherwise.
     */
    bool getNestedParallelism();

    /**
     * @brief Set the dynamic threads number (false by default).
     * @ingroup basicstruct_group
     *
     * Automatically adjust the number of running threads within a parallel
     * region.
     *
     * Desactivated by default.
     *
     * @param value \c True if dynamic thread number should be used, \c False
     * otherwise.
     */
    void setDynamicThreadsNumber(bool value);

    /**
     * @brief Get the dynamic thread number adjustment status.
     * @ingroup basicstruct_group
     * @ingroup basicstruct_group
     * @return \c True if dynamic adjustment is enabled, \c False otherwise.
     */
    bool getDynamicThreadsNumber();


  } /* namespace threadsOMP */

}   // namespace gum

// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/tools/core/threadsOMP_inl.h>
#endif /* GUM_NO_INLINE */

#endif   // GUM_THREADS_OPENMP_H
