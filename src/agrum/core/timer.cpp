/***************************************************************************
*   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
*   {prenom.nom}_at_lip6.fr                                               *
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
#define GUM_TRACE_ON
#include <agrum/config.h>

#include <agrum/core/timer.h>

#ifdef GUM_NO_INLINE
#include <agrum/core/timer.inl>
#endif // GUM_NO_INLINE

namespace gum {

  Timer::Timer() {
    GUM_CONSTRUCTOR(Timer);
    reset();
  }

  Timer::Timer(const Timer &from)
      : _start(from._start), _pause(from._pause), _sleeping(from._sleeping) {
    GUM_CONS_CPY(Timer);
  }

  Timer::~Timer() { GUM_DESTRUCTOR(Timer); }

  Timer &Timer::operator=(const Timer &from) {
    GUM_OP_CPY(Timer);
    _start = from._start;
    _pause = from._pause;
    _sleeping = from._sleeping;
    return *this;
  }
} /* namespace gum */
