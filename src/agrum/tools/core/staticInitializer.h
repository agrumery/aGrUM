/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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

/**
 * @file
 * @brief Data types to enable creating static variables at compile time
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 *
 */

#ifndef GUM_STATIC_INITIALIZER_H
#define GUM_STATIC_INITIALIZER_H

namespace gum {

  // A dummy enum that, when passed as argument to a consteval constructor, will
  // mean that we want this constructor to be used to initialize a static variable
  // at compile time.
  // Basically, the idea is the following:
  // 1/ in the .cpp file, create the "constinit const" static variables you need
  //    using a constructor to which StaticInitializer::CONSTINIT is passed in
  //    argument (see file tools/core/AVLTree.cpp)
  // 2/ in the .h file, declare "extern constinit cont" these static variables.
  //    If these correspond to iterators of templates, you may need to declare
  //    pointers to these variables as "inline constexpr void* const" and use
  //    these pointer rather than the static variables themselves (this avoids
  //    having a lot of strict-aliasing rule warnings). See the very end of
  //    file AVLTree.h for an example.
  enum class StaticInitializer : char { CONSTINIT, CONSTEXPR };

}   // namespace gum

#endif   // GUM_STATIC_INITIALIZER_H
