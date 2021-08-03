/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Outlined implementation VariableNodeMap
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/tools/graphicalModels/variableNodeMap.h>
#  include <iostream>
#  include <sstream>

#  ifdef GUM_NO_INLINE
#    include <agrum/tools/graphicalModels/variableNodeMap_inl.h>
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
