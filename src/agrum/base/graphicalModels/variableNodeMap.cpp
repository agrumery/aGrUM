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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
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
 * @brief Outlined implementation VariableNodeMap
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <iostream>
#  include <sstream>

#  include <agrum/base/graphicalModels/variableNodeMap.h>

#  ifdef GUM_NO_INLINE
#    include <agrum/base/graphicalModels/variableNodeMap_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  // Default constructor.
  VariableNodeMap::VariableNodeMap() {
    GUM_CONSTRUCTOR(VariableNodeMap);
    ;
  }

  // Copy constructor.
  VariableNodeMap::VariableNodeMap(const VariableNodeMap& source) {
    GUM_CONS_CPY(VariableNodeMap);

    _copy_(source);
  }

  // Destructor
  VariableNodeMap::~VariableNodeMap() {
    GUM_DESTRUCTOR(VariableNodeMap);

    clear();
  }

  // Copy operator.
  VariableNodeMap& VariableNodeMap::operator=(const VariableNodeMap& source) {
    clear();
    _copy_(source);

    return *this;
  }

  void VariableNodeMap::clear() {
    for (auto iter = _nodes2vars_.begin(); iter != _nodes2vars_.end(); ++iter)
      delete iter.second();

    _nodes2vars_.clear();
    _names2nodes_.clear();
  }

  /// friendly displays the content of the VariableNodeMap
  std::string VariableNodeMap::toString() const {
    std::stringstream stream;

    stream << "list of associations:" << std::endl;
    stream << _nodes2vars_ << std::endl << std::endl;
    stream << "list of variable names:" << std::endl;
    stream << _names2nodes_ << std::endl;

    return stream.str();
  }

  /// do the copy
  void VariableNodeMap::_copy_(const VariableNodeMap& source) {
    for (auto iter = source._nodes2vars_.begin(); iter != source._nodes2vars_.end(); ++iter)
      _nodes2vars_.insert(iter.first(), iter.second()->clone());

    // copy factory is used inside insert

    _names2nodes_ = source._names2nodes_;
  }

  /// for friendly displaying the content of clique graphs
  std::ostream& operator<<(std::ostream& stream, const VariableNodeMap& v) {
    stream << v.toString();
    return stream;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
