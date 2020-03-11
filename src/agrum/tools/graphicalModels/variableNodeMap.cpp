
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN (@LIP6)
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
  VariableNodeMap::VariableNodeMap() { GUM_CONSTRUCTOR(VariableNodeMap); }

  // Copy constructor.
  VariableNodeMap::VariableNodeMap(const VariableNodeMap& source) {
    GUM_CONS_CPY(VariableNodeMap);

    __copy(source);
  }

  // Destructor
  VariableNodeMap::~VariableNodeMap() {
    GUM_DESTRUCTOR(VariableNodeMap);

    clear();
  }

  // Copy operator.
  VariableNodeMap& VariableNodeMap::operator=(const VariableNodeMap& source) {
    clear();
    __copy(source);

    return *this;
  }

  void VariableNodeMap::clear() {
    for (auto iter = __nodes2vars.begin(); iter != __nodes2vars.end(); ++iter)
      delete iter.second();

    __nodes2vars.clear();
    __names2nodes.clear();
  }

  /// friendly displays the content of the VariableNodeMap
  const std::string VariableNodeMap::toString() const {
    std::stringstream stream;

    stream << "list of associations:" << std::endl;
    stream << __nodes2vars << std::endl << std::endl;
    stream << "list of variable names:" << std::endl;
    stream << __names2nodes << std::endl;

    return stream.str();
  }

  /// do the copy
  void VariableNodeMap::__copy(const VariableNodeMap& source) {
    for (auto iter = source.__nodes2vars.begin();
         iter != source.__nodes2vars.end();
         ++iter)
      __nodes2vars.insert(iter.first(), iter.second()->clone());

    // copy factory is used inside insert

    __names2nodes = source.__names2nodes;
  }

  /// for friendly displaying the content of clique graphs
  std::ostream& operator<<(std::ostream& stream, const VariableNodeMap& v) {
    stream << v.toString();
    return stream;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
