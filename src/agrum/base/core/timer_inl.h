/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
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
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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


// to ease IDE parser
#include <chrono>

#include <agrum/base/core/timer.h>

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

  INLINE
  std::ostream& operator<<(std::ostream& os, const Timer& t) { return os << t.toString(); };
} /* namespace gum */
