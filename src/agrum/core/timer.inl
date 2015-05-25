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
#include <ctime>
#include <agrum/core/timer.h>

#include <sys/time.h>

static long get_clock() {
  struct timeval tv;
  gettimeofday(&tv, nullptr);
  return (tv.tv_sec * 1000000) + tv.tv_usec;
}

namespace gum {
  INLINE
  void Timer::reset() {
    _pause = 0;
    _sleeping = false;
    _start = get_clock();

    // do _start = clock(); while ( _start == k );// to be sure to start at the
    // beginning of a tick
  }

  INLINE
  double Timer::step() const {
    if (_sleeping)
      return double(_pause - _start) / 1000000.0;
    else
      return double(get_clock() - _start) / 1000000.0;
  }

  INLINE
  double Timer::pause() {
    if (!_sleeping) {
      _pause = get_clock();
      _sleeping = true;
    }

    return step();
  }

  INLINE
  double Timer::resume() {
    if (_sleeping) {
      _start += get_clock() - _pause;
      _sleeping = false;
    }

    return step();
  }
} /* namespace gum */
