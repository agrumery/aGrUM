
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
 * @brief  Interface-like class encapsulating basic functionalities for both a
 *IBayesNet and IMarkovNet
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN (@LIP6)
 */

#include <agrum/tools/core/math/math.h>
#include <agrum/tools/graphicalModels/graphicalModel.h>

namespace gum {

  INLINE
  const std::string& GraphicalModel::property(const std::string& name) const {
    try {
      return __properties()[name];
    } catch (NotFound&) {
      std::string msg = "The following property does not exists: ";
      GUM_ERROR(NotFound, msg + name);
    }
  }

  INLINE
  HashTable< std::string, std::string >& GraphicalModel::__properties() const {
    if (__propertiesMap == nullptr) {
      __propertiesMap = new HashTable< std::string, std::string >();
    }

    return *__propertiesMap;
  }

  INLINE
  const std::string&
     GraphicalModel::propertyWithDefault(const std::string& name,
                                         const std::string& byDefault) const {
    try {
      return __properties()[name];
    } catch (NotFound&) { return byDefault; }
  }

  INLINE
  void GraphicalModel::setProperty(const std::string& name,
                                   const std::string& value) {
    try {
      __properties()[name] = value;
    } catch (NotFound&) { __properties().insert(name, value); }
  }


  INLINE
  double GraphicalModel::log10DomainSize() const {
    double dSize = 0.0;

    for (const auto node: nodes()) {
      dSize += std::log10(variable(node).domainSize());
    }

    return dSize;
  }

  INLINE
  Instantiation GraphicalModel::completeInstantiation() const {
    Instantiation I;

    for (const auto node: nodes())
      I << variable(node);

    return I;
  }

  INLINE
  bool GraphicalModel::empty() const { return size() == 0; }
} /* namespace gum */
