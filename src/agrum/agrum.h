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

#include <agrum/tools/core/debug.h>
#include <agrum/tools/core/exceptions.h>
#include <agrum/tools/core/inline.h>
#include <agrum/tools/core/types.h>
#include <agrum/tools/core/utils_misc.h>

#endif   // GUM_AGRUM_H
