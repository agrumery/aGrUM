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

#include <agrum/agrum.h>
#include <agrum/tools/core/AVLTree.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  // end/rend iterator for all the types of data. This is mainly what stroustrup suggests
  // in his C++ programming language, third edition, page 854
  static constexpr const AVLTreeIterator< int > _static_AVLTree_end_rend_;
  static constexpr const void* _get_AVLTree_end_rend_() { return &_static_AVLTree_end_rend_; }
  constexpr const void* const  _AVLTree_end_rend_ = _get_AVLTree_end_rend_();

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
