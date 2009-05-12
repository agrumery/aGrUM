/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *   test $Id: $                                                           *
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
#include <agrum/core/timer.h>
#include <agrum/core/debug.h>
#include <agrum/core/inline.h>


#ifdef GUM_NO_INLINE
#include <agrum/core/timer.inl>
#endif // GUM_NO_INLINE

namespace gum {

  Timer::Timer() {
    GUM_CONSTRUCTOR( Timer );
    reset();
  }

  Timer::Timer( const Timer& from ) : _start (from._start) {
    GUM_CONS_CPY( Timer );
  }

  Timer::~Timer() {
    GUM_DESTRUCTOR( Timer );
  }

  Timer& Timer::operator= (const Timer& from) {
    GUM_OP_CPY ( Timer );
    _start = from._start;
    return *this;
  }

  void Timer::reset() {
    struct timeb time_val;
    ftime( &time_val );
    _start = _convert( &time_val );
  }

  double Timer::step() const {
    struct timeb time_val;
    ftime( &time_val );
    double result = _convert( &time_val ) - _start;
    return result;
  }


} /* namespace gum */

