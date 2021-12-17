/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Class to include at least once this header
 *
 * @author Christophe GONZALES(@AMU) & Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <agrum/tools/graphicalModels/inference/scheduler/scheduler.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/tools/graphicalModels/inference/scheduler/scheduler_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  /// default constructor
  Scheduler::Scheduler(Size   max_nb_threads,
                       double max_memory) :
      _max_memory(max_memory * 1048576.0) {   // 1048576 = 1024 * 1024 = 1Mb
    // set the number of threads
    if (max_nb_threads != Size(0)) _max_nb_threads = max_nb_threads;
    if (_max_memory <= 0.0) _max_memory = 0.0;

    // for debugging purposes
    GUM_CONSTRUCTOR(Scheduler);
  }


  /// copy constructor
  Scheduler::Scheduler(const Scheduler& from) :
      _max_nb_threads(from._max_nb_threads), _max_memory(from._max_memory) {
    // for debugging purposes
    GUM_CONS_CPY(Scheduler);
  }


  /// move constructor
  Scheduler::Scheduler(Scheduler&& from) :
      _max_nb_threads(from._max_nb_threads), _max_memory(from._max_memory) {
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
      _max_nb_threads = from._max_nb_threads;
      _max_memory     = from._max_memory;
    }

    return *this;
  }


  /// move operator
  Scheduler& Scheduler::operator=(Scheduler&& from) {
    if (this != &from) {
      _max_nb_threads = from._max_nb_threads;
      _max_memory     = from._max_memory;
    }

    return *this;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
