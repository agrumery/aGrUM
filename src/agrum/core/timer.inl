/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
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
    std::chrono::duration<double, std::milli> ms;
    ;
    if ( _sleeping )
      ms = _pause - _start;
    else
      ms = std::chrono::high_resolution_clock::now() - _start;
    return ms.count() / 1000.0;
  }

  INLINE
  double Timer::pause() {
    if ( !_sleeping ) {
      _pause = std::chrono::high_resolution_clock::now();
      _sleeping = true;
    }

    return step();
  }

  INLINE
  double Timer::resume() {
    if ( _sleeping ) {
      _start += std::chrono::high_resolution_clock::now() - _pause;
      _sleeping = false;
    }

    return step();
  }
} /* namespace gum */
