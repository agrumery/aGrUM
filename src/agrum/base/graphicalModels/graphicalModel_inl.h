/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


/**
 * @file
 * @brief  Interface-like class encapsulating basic functionalities for both a
 *IBayesNet and IMarkovRandomField
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <algorithm>
#include <iterator>

#include <agrum/base/graphicalModels/graphicalModel.h>

#include <agrum/base/core/math/math_utils.h>

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
  std::vector< std::string > GraphicalModel::properties() const {
    std::vector< std::string > prop;
    for (const auto& [p, v]: _propertiesMap_)
      prop.push_back(p);
    return prop;
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

  INLINE
  VariableSet GraphicalModel::variables(const std::vector< std::string >& l) const {
    VariableSet            s;
    const VariableNodeMap& v = variableNodeMap();
    for (const auto& name: l) {
      s.insert(&v.variableFromName(name));
    }
    return s;
  }

  INLINE
  VariableSet GraphicalModel::variables(const NodeSet& l) const {
    VariableSet            s;
    const VariableNodeMap& v = variableNodeMap();
    for (const auto& node: l) {
      s.insert(&v.get(node));
    }
    return s;
  }

} /* namespace gum */
