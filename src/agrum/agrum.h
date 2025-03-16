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





#ifndef GUM_AGRUM_H
#define GUM_AGRUM_H

/// gum is the global namespace for all aGrUM entities
namespace gum {

  /// Internal namespace for aGrUM signaler/listener components
  namespace __sig__ {}

  /// Internal namespace for aGrUM debugging tools
  namespace __debug__ {}

  /// Aggregators are functional description of CPTs
  namespace aggregator {}

  /// Internal namespace for complexity tools (quite empty for now)
  namespace complexity {}

  /// namespace for all credal networks entities
  namespace credal {

    /// namespace for constraint-based description of credal sets
    namespace lp {}
  }   // namespace credal

  /// namespace for all probabilistic relational models entities
  namespace prm {}
}   // namespace gum

#include <agrum/config.h>

#include <agrum/base/core/debug.h>
#include <agrum/base/core/exceptions.h>
#include <agrum/base/core/inline.h>
#include <agrum/base/core/types.h>

#include <agrum/base/core/utils_misc.h>

#endif   // GUM_AGRUM_H
