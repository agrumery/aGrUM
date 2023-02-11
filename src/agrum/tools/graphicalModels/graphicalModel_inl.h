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
 * @brief  Interface-like class encapsulating basic functionalities for both a
 *IBayesNet and IMarkovRandomField
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <algorithm>
#include <iterator>

#include <agrum/tools/core/math/math_utils.h>
#include <agrum/tools/graphicalModels/graphicalModel.h>

namespace gum {

  INLINE
  const std::string& GraphicalModel::property(const std::string& name) const {
    try {
      return _properties_()[name];
    } catch (NotFound const&) {
      std::string msg = "The following property does not exists: ";
      GUM_ERROR(NotFound, msg + name)
    }
  }

  INLINE
  const HashTable< std::string, std::string >& GraphicalModel::_properties_() const {
    return _propertiesMap_;
  }

  INLINE
  const std::string& GraphicalModel::propertyWithDefault(const std::string& name,
                                                         const std::string& byDefault) const {
    return (_propertiesMap_.exists(name)) ? _propertiesMap_[name] : byDefault;
  }

  INLINE
  void GraphicalModel::setProperty(const std::string& name, const std::string& value) {
    if (_propertiesMap_.exists(name)) _propertiesMap_[name] = value;
    else _propertiesMap_.insert(name, value);
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

  INLINE
  std::vector< std::string > GraphicalModel::names(const std::vector< NodeId >& ids) const {
    std::vector< std::string > res;
    const VariableNodeMap&     v = variableNodeMap();
    std::transform(ids.cbegin(), ids.cend(), std::back_inserter(res), [v](NodeId n) {
      return v[n].name();
    });
    return res;
  }

  INLINE
  std::vector< std::string > GraphicalModel::names(const NodeSet& ids) const {
    const VariableNodeMap&     v = variableNodeMap();
    std::vector< std::string > res;
    for (auto n: ids) {
      res.push_back(v.name(n));
    }
    return res;
  }

  INLINE
  std::vector< NodeId > GraphicalModel::ids(const std::vector< std::string >& names) const {
    std::vector< NodeId >  res;
    const VariableNodeMap& v = variableNodeMap();
    std::transform(names.cbegin(),
                   names.cend(),
                   std::back_inserter(res),
                   [v](const std::string& n) { return v.idFromName(n); });
    return res;
  }
} /* namespace gum */
