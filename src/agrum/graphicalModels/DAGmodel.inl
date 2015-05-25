/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 * @file
 * @brief  Interface-like class encapsulating basic functionalities for a IBayesNet.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <math.h>

#include <agrum/graphicalModels/DAGmodel.h>

namespace gum {

  INLINE
  const std::string &DAGmodel::property(const std::string &name) const {
    try {
      return __properties()[name];
    } catch (NotFound &) {
      std::string msg = "The following property does not exists: ";
      GUM_ERROR(NotFound, msg + name);
    }
  }

  INLINE
  const std::string &
  DAGmodel::propertyWithDefault(const std::string &name,
                                const std::string &byDefault) const {
    try {
      return __properties()[name];
    } catch (NotFound &) {
      return byDefault;
    }
  }

  INLINE
  void DAGmodel::setProperty(const std::string &name, const std::string &value) {
    try {
      __properties()[name] = value;
    } catch (NotFound &) {
      __properties().insert(name, value);
    }
  }

  INLINE
  const DAG &DAGmodel::dag() const { return _dag; }

  INLINE
  const DAG::NodeIterator DAGmodel::beginNodes() const { // deprecated
    return dag().begin();
  }

  INLINE
  const DAG::NodeIterator DAGmodel::endNodes() const { // deprecated
    return dag().end();
  }

  INLINE
  const DAG::ArcIterator DAGmodel::beginArcs() const { // deprecated
    return dag().arcs().begin();
  }

  INLINE
  const DAG::ArcIterator &DAGmodel::endArcs() const { // deprecated
    return dag().arcs().end();
  }

  INLINE
  HashTable<std::string, std::string> &DAGmodel::__properties() const {
    if (__propertiesMap == nullptr) {
      __propertiesMap = new HashTable<std::string, std::string>();
    }

    return *__propertiesMap;
  }

  INLINE
  double DAGmodel::log10DomainSize(void) const {
    double dSize = 0.0;

    for (const auto node : nodes()) {
      dSize += std::log10(variable(node).domainSize());
    }

    return dSize;
  }

  INLINE
  void DAGmodel::completeInstantiation(Instantiation &I) const {
    I.clear();

    for (const auto node : dag())
      I << variable(node);
  }

  INLINE
  Size DAGmodel::size() const { return dag().size(); }

  INLINE
  bool DAGmodel::empty() const { return size() == 0; }

  INLINE
  Size DAGmodel::sizeArcs() const { return _dag.sizeArcs(); }

  INLINE
  Size DAGmodel::nbrArcs() const { return _dag.sizeArcs(); }

  INLINE const ArcSet &DAGmodel::arcs() const { return _dag.arcs(); }

  INLINE const NodeGraphPart &DAGmodel::nodes() const {
    return (NodeGraphPart &)_dag;
  }
} /* namespace gum */
