/**
 *
 *   Copyright (c) 2005-2022 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief AVL binary search trees.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/tools/core/AVLTree.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  // The global static variable representing end and rend for all the AVL trees.
  // The constinit ensures it is created at compile time. Here, we use the
  // constinit keyword rather than constexpr because, in debugging mode, the
  // iterators' destructors cannot be declared as constepxr.
  constinit const AVLTreeIterator< int, std::less< int > >
     _static_AVLTree_end_(StaticInitializer::CONSTINIT);
  constinit const AVLTreeReverseIterator< int, std::less< int > >
     _static_AVLTree_rend_(StaticInitializer::CONSTINIT);
  constinit const AVLTreeIteratorSafe< int, std::less< int > >
     _static_AVLTree_end_safe_(StaticInitializer::CONSTINIT);
  constinit const AVLTreeReverseIteratorSafe< int, std::less< int > >
     _static_AVLTree_rend_safe_(StaticInitializer::CONSTINIT);

}   // namespace gum

#endif   // DOXYGEN_SHOULD_SKIP_THIS
