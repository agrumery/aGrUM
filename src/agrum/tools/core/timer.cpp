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
 * @brief Class used to compute response times for benchmark purposes.
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) & Jean-Philippe Dubus
 */

#include <agrum/tools/core/timer.h>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/core/timer_inl.h>
#endif   // GUM_NO_INLINE

namespace gum {

  Timer::Timer() {
    GUM_CONSTRUCTOR(Timer);
    reset();
  }

  Timer::Timer(const Timer& from) :
      start_(from.start_), pause_(from.pause_), sleeping_(from.sleeping_) {
    GUM_CONS_CPY(Timer);
  }

  Timer::~Timer() { GUM_DESTRUCTOR(Timer); }

  Timer& Timer::operator=(const Timer& from) {
    GUM_OP_CPY(Timer);
    start_ = from.start_;
    pause_ = from.pause_;
    sleeping_ = from.sleeping_;
    return *this;
  }

} /* namespace gum */
