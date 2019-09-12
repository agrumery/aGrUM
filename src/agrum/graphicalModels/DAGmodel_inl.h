
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief  Interface-like class encapsulating basic functionalities for a
 *IBayesNet.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <agrum/core/math/math.h>
#include <agrum/graphicalModels/DAGmodel.h>

namespace gum {

  INLINE
  const std::string& DAGmodel::property(const std::string& name) const {
    try {
      return __properties()[name];
    } catch (NotFound&) {
      std::string msg = "The following property does not exists: ";
      GUM_ERROR(NotFound, msg + name);
    }
  }

  INLINE
  const std::string&
     DAGmodel::propertyWithDefault(const std::string& name,
                                   const std::string& byDefault) const {
    try {
      return __properties()[name];
    } catch (NotFound&) { return byDefault; }
  }

  INLINE
  void DAGmodel::setProperty(const std::string& name, const std::string& value) {
    try {
      __properties()[name] = value;
    } catch (NotFound&) { __properties().insert(name, value); }
  }

  INLINE
  const DAG& DAGmodel::dag() const { return _dag; }

  INLINE
  HashTable< std::string, std::string >& DAGmodel::__properties() const {
    if (__propertiesMap == nullptr) {
      __propertiesMap = new HashTable< std::string, std::string >();
    }

    return *__propertiesMap;
  }

  INLINE
  double DAGmodel::log10DomainSize() const {
    double dSize = 0.0;

    for (const auto node : nodes()) {
      dSize += std::log10(variable(node).domainSize());
    }

    return dSize;
  }

  INLINE
  Instantiation DAGmodel::completeInstantiation() const {
    Instantiation I;

    for (const auto node : dag())
      I << variable(node);

    return I;
  }

  INLINE
  Size DAGmodel::size() const { return dag().size(); }

  INLINE
  bool DAGmodel::empty() const { return size() == 0; }

  INLINE
  Size DAGmodel::sizeArcs() const { return _dag.sizeArcs(); }

  INLINE const ArcSet& DAGmodel::arcs() const { return _dag.arcs(); }

  INLINE const NodeSet& DAGmodel::parents(const NodeId id) const {
    return _dag.parents(id);
  }
  INLINE const NodeSet& DAGmodel::parents(const std::string& name) const {
    return parents(idFromName(name));
  }

  INLINE const NodeSet& DAGmodel::children(const NodeId id) const {
    return _dag.children(id);
  }
  INLINE const NodeSet& DAGmodel::children(const std::string& name) const {
    return _dag.children(idFromName(name));
  }

  INLINE const NodeGraphPart& DAGmodel::nodes() const {
    return (NodeGraphPart&)_dag;
  }
} /* namespace gum */
