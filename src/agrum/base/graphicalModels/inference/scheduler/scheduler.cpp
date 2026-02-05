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
 * @brief the common interface of all the schedulers
 *
 * @author Christophe GONZALES(@AMU) & Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <agrum/base/graphicalModels/inference/scheduler/scheduler.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/base/graphicalModels/inference/scheduler/scheduler_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  /// default constructor
  Scheduler::Scheduler(Size nb_threads, double max_memory) :
      ThreadNumberManager(nb_threads),
      _max_memory(max_memory * 1048576.0) {   // 1048576 = 1024 * 1024 = 1Mb
    if (_max_memory <= 0.0) _max_memory = 0.0;

    // for debugging purposes
    GUM_CONSTRUCTOR(Scheduler);
  }

  /// copy constructor
  Scheduler::Scheduler(const Scheduler& from) :
      ThreadNumberManager(from), _max_memory(from._max_memory) {
    // for debugging purposes
    GUM_CONS_CPY(Scheduler);
  }

  /// move constructor
  Scheduler::Scheduler(Scheduler&& from) :
      ThreadNumberManager(std::move(from)), _max_memory(from._max_memory) {
    // for debugging purposes
    GUM_CONS_MOV(Scheduler);
  }

  /// destructor
  Scheduler::~Scheduler() {
    // for debugging purposes
    GUM_DESTRUCTOR(Scheduler);
  }

  /// copy operator
  Scheduler& Scheduler::operator=(const Scheduler& from) {
    if (this != &from) {
      ThreadNumberManager::operator=(from);
      _max_memory = from._max_memory;
    }

    return *this;
  }

  /// move operator
  Scheduler& Scheduler::operator=(Scheduler&& from) {
    if (this != &from) {
      ThreadNumberManager::operator=(std::move(from));
      _max_memory = from._max_memory;
    }

    return *this;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
