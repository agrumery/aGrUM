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
   * @headerfile threadExecutorBase.h <agrum/base/core/threads/threadExecutorBase.h>
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
#  include <agrum/base/core/threads/threadExecutorBase_inl.h>
#endif   /* GUM_NO_INLINE */


#endif   // AGRUM_THREADEXECUTOR_BASE_H
