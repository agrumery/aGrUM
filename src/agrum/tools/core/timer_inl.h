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


// to ease IDE parser
#include <chrono>

#include <agrum/tools/core/timer.h>

namespace gum {


  INLINE
  void Timer::reset() {
    sleeping_ = false;
    start_    = std::chrono::high_resolution_clock::now();
    pause_    = std::chrono::high_resolution_clock::now();

    // do start_ = clock(); while ( start_ == k );// to be sure to start at the
    // beginning of a tick
  }

  INLINE
  std::chrono::duration< double, std::milli > Timer::step_() const {
    std::chrono::duration< double, std::milli > ms;
    if (sleeping_) ms = pause_ - start_;
    else ms = std::chrono::high_resolution_clock::now() - start_;

    return ms;
  }

  INLINE
  double Timer::step() const { return step_().count() / 1000.0; }

  INLINE
  double Timer::pause() {
    if (!sleeping_) {
      pause_    = std::chrono::high_resolution_clock::now();
      sleeping_ = true;
    }

    return step();
  }

  INLINE
  double Timer::resume() {
    if (sleeping_) {
      start_ += std::chrono::high_resolution_clock::now() - pause_;
      sleeping_ = false;
    }

    return step();
  }
} /* namespace gum */
