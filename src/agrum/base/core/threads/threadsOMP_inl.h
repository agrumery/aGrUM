/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
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


/**
 * @file
 * @brief OpenMP wrappers implementation.
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN
 */

// to ease automatic parsers
#include <agrum/agrum.h>

#include <agrum/base/core/threads/threadsOMP.h>

namespace gum {

  namespace threadsOMP {

    INLINE bool hasOMPSupport() {
#ifdef _OPENMP
      return true;
#else
      return false;
#endif
    }

    INLINE unsigned int getMaxNumberOfThreads() {
#ifdef _OPENMP
      return omp_get_max_threads();
#else
      return 1;
#endif
    }

    INLINE unsigned int getNumberOfLogicalProcessors() {
#ifdef _OPENMP
      return omp_get_num_procs();
#else
      return std::thread::hardware_concurrency();
#endif
    }

    INLINE unsigned int getThreadNumber() {
#ifdef _OPENMP
      return omp_get_thread_num();
#else
      return 0;
#endif
    }

    INLINE unsigned int getNumberOfRunningThreads() {
#ifdef _OPENMP
      return omp_get_num_threads();
#else
      return 1;
#endif
    }

    INLINE void setNestedParallelism(bool value) {
#ifdef _OPENMP
      omp_set_nested(((value == true) ? 1 : 0));
#else
      GUM_ERROR(OperationNotAllowed,
                "openMP was not enabled at compilation (and you "
                "asked for nested parallelism !)");
#endif
    }

    INLINE bool getNestedParallelism() {
#ifdef _OPENMP
      return ((omp_get_nested() == 0) ? false : true);
#else
      return false;
#endif
    }

    INLINE void setDynamicThreadsNumber(bool value) {
#ifdef _OPENMP
      omp_set_dynamic(((value == true) ? 1 : 0));
#else
      GUM_ERROR(OperationNotAllowed,
                "openMP was not enabled at compilation (and you "
                "asked for dynamic adjustment of the number of "
                "threads !)");
#endif
    }

    INLINE bool getDynamicThreadsNumber() {
#ifdef _OPENMP
      return ((omp_get_dynamic() == 0) ? false : true);
#else
      return false;
#endif
    }

  } /* namespace threadsOMP */

}   // namespace gum
