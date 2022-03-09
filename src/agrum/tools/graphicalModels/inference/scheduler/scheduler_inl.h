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


/** @file
 * @brief the common interface of all the schedulers
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>

namespace gum {

  /// sets an upper bound on the memory consumption admissible
  INLINE void Scheduler::setMaxMemory(double megabytes) {
    _max_memory = megabytes * 1048576.0;   // 1048576 = 1024 * 1024 = 1Mb
    if (_max_memory < 0.0) _max_memory = 0.0;
  }


  /// returns the Max memory (in megabytes) available for executing schedules
  INLINE double Scheduler::maxMemory() const {
    return _max_memory / 1048576.0;   // 1048576 = 1024 * 1024 = 1Mb
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
