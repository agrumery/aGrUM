/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Contains useful methods for random stuff.
 *
 * @author Vincent RENAUDINEAU and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <chrono>

#include <agrum/agrum.h>

#include <agrum/base/core/utils_misc.h>
#include <agrum/base/core/utils_random.h>

#ifdef GUM_NO_INLINE
#  include <agrum/base/core/utils_random_inl.h>
#endif   // GUM_NO_INLINE

namespace gum {
  namespace {
    // the seed given to the last call to initRandom (0 if none), the clock being resolved
    unsigned int _currentSeed_ = 0;
  }   // namespace

  namespace _rand_namespace_ {
    // not inline: an inline function would give one generator per shared module
    std::mt19937& generator() {
      static std::mt19937 Generator_;
      return Generator_;
    }
  }   // namespace _rand_namespace_

  void initRandom(unsigned int seed) {
    if (seed == 0) seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
    _currentSeed_ = seed;
    std::seed_seq seq{seed + 1, seed + 2, seed + 3, seed + 4, seed + 5};
    _rand_namespace_::generator().seed(seq);
  }

  unsigned int randomGeneratorSeed() { return _currentSeed_; }

} /* namespace gum */
