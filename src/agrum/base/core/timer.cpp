/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Class used to compute response times for benchmark purposes.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Jean-Philippe Dubus
 */

#include <chrono>
#include <cmath>
#include <sstream>

#include <agrum/base/core/timer.h>

#ifdef GUM_NO_INLINE
#  include <agrum/base/core/timer_inl.h>
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

  Timer::~Timer() {
    GUM_DESTRUCTOR(Timer);
    ;
  }

  Timer& Timer::operator=(const Timer& from) {
    GUM_OP_CPY(Timer);
    start_    = from.start_;
    pause_    = from.pause_;
    sleeping_ = from.sleeping_;
    return *this;
  }

  std::string Timer::toString() const {
    auto d     = step();
    auto sec   = fmod(d, 60);
    d          = (d - sec) / 60;
    auto minut = fmod(d, 60);
    d          = (d - minut) / 60;

    std::stringstream s;
    s << std::setfill('0') << std::setw(2) << int(d) << ":";
    s << std::setfill('0') << std::setw(2) << int(minut) << ":";
    s << ((sec < 10) ? "0" : "") << sec;

    return s.str();
  }
} /* namespace gum */
