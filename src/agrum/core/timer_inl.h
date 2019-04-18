
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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

#include <agrum/core/timer.h>

namespace gum {


  INLINE
  void Timer::reset() {
    _sleeping = false;
    _start = std::chrono::high_resolution_clock::now();
    _pause = std::chrono::high_resolution_clock::now();

    // do _start = clock(); while ( _start == k );// to be sure to start at the
    // beginning of a tick
  }

  INLINE
  double Timer::step() const {
    std::chrono::duration< double, std::milli > ms;
    ;
    if (_sleeping)
      ms = _pause - _start;
    else
      ms = std::chrono::high_resolution_clock::now() - _start;
    return ms.count() / 1000.0;
  }

  INLINE
  double Timer::pause() {
    if (!_sleeping) {
      _pause = std::chrono::high_resolution_clock::now();
      _sleeping = true;
    }

    return step();
  }

  INLINE
  double Timer::resume() {
    if (_sleeping) {
      _start += std::chrono::high_resolution_clock::now() - _pause;
      _sleeping = false;
    }

    return step();
  }
} /* namespace gum */
