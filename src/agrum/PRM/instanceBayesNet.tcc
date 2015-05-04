/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
 * @brief Inline implementation of InstanceBayesNet.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <agrum/PRM/instanceBayesNet.h> // to ease IDE parser

namespace gum {
  namespace prm {

    template <typename GUM_SCALAR>
    void InstanceBayesNet<GUM_SCALAR>::__init(const Instance<GUM_SCALAR> &i) {
      for (const auto node : i.type().dag().nodes()) {
        try {
          // Adding the attribute
          const Attribute<GUM_SCALAR> &attr = i.get(node);
          this->_dag.addNode(attr.id());
          __varNodeMap.insert(&(attr.type().variable()), &attr);
        } catch (NotFound &) {
          // Not an attribute
        }
      }

      for (const auto &arc : i.type().dag().arcs()) {
        try {
          this->_dag.addArc(arc.tail(), arc.head());
        } catch (InvalidNode &) {
          // Not added means not an attribute
        }
      }
    }

    template <typename GUM_SCALAR>
    INLINE
    InstanceBayesNet<GUM_SCALAR>::InstanceBayesNet(const Instance<GUM_SCALAR> &i)
        : IBayesNet<GUM_SCALAR>(), __inst(&i) {
      GUM_CONSTRUCTOR(InstanceBayesNet);
      __init(i);
    }

    template <typename GUM_SCALAR>
    INLINE
    InstanceBayesNet<GUM_SCALAR>::InstanceBayesNet(const InstanceBayesNet &from)
        : IBayesNet<GUM_SCALAR>(from), __varNodeMap(from.__varNodeMap),
          __inst(from.__inst) {
      GUM_CONS_CPY(InstanceBayesNet);
    }

    template <typename GUM_SCALAR>
    INLINE InstanceBayesNet<GUM_SCALAR>::~InstanceBayesNet() {
      GUM_DESTRUCTOR(InstanceBayesNet);
    }

    template <typename GUM_SCALAR>
    INLINE InstanceBayesNet<GUM_SCALAR> &InstanceBayesNet<GUM_SCALAR>::
    operator=(const InstanceBayesNet &from) {
      if (this != &from) {
        IBayesNet<GUM_SCALAR>::operator=(from);

        __varNodeMap = from.__varNodeMap;
      }

      return *this;
    }

    template <typename GUM_SCALAR>
    INLINE const Potential<GUM_SCALAR> &
    InstanceBayesNet<GUM_SCALAR>::cpt(NodeId varId) const {
      return __get(varId).cpf();
    }

    template <typename GUM_SCALAR>
    INLINE const VariableNodeMap &
    InstanceBayesNet<GUM_SCALAR>::variableNodeMap() const {
      GUM_ERROR(NotFound, "no VariableNodeMap in an InstanceBayesNet");
    }

    template <typename GUM_SCALAR>
    INLINE const DiscreteVariable &
    InstanceBayesNet<GUM_SCALAR>::variable(NodeId id) const {
      return __get(id).type().variable();
    }

    template <typename GUM_SCALAR>
    INLINE NodeId
        InstanceBayesNet<GUM_SCALAR>::nodeId(const DiscreteVariable &var) const {
      return __varNodeMap[&var]->id();
    }

    template <typename GUM_SCALAR>
    INLINE NodeId
        InstanceBayesNet<GUM_SCALAR>::idFromName(const std::string &name) const {
      return __get(name).id();
    }

    template <typename GUM_SCALAR>
    INLINE const DiscreteVariable &
    InstanceBayesNet<GUM_SCALAR>::variableFromName(const std::string &name) const {
      return __get(name).type().variable();
    }

    template <typename GUM_SCALAR>
    INLINE const ClassElement<GUM_SCALAR> &
    InstanceBayesNet<GUM_SCALAR>::__get(NodeId id) const {
      return __inst->get(id);
    }

    template <typename GUM_SCALAR>
    INLINE const ClassElement<GUM_SCALAR> &
    InstanceBayesNet<GUM_SCALAR>::__get(const std::string &name) const {
      try {
        return __inst->get(name);
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "no element found with that name");
      }
    }

    template <typename GUM_SCALAR>
    INLINE const NodeProperty<Size> &
    InstanceBayesNet<GUM_SCALAR>::modalities() const {
      if (__modalities.empty()) {
        for (const auto node : this->nodes()) {
          __modalities.insert(node, variable(node).domainSize());
        }
      }

      return __modalities;
    }

    template <typename GUM_SCALAR>
    INLINE std::string InstanceBayesNet<GUM_SCALAR>::toDot() const {
      std::string tab = "  ";
      std::stringstream output;
      output << "digraph \"";
      output << __inst->name() << "\" {" << std::endl;

      for (const auto node : this->nodes()) {
        if (this->dag().children(node).size() > 0) {
          const NodeSet &children = this->dag().children(node);

          for (const auto chi : children) {
            output << tab << "\"" << variable(node).name() << "\" -> ";
            output << "\"" << variable(chi).name() << "\";" << std::endl;
          }
        } else if (this->dag().parents(node).size() == 0) {
          output << tab << "\"" << variable(node).name() << "\";" << std::endl;
        }
      }

      output << "}" << std::endl;
      return output.str();
    }

  } /* namespace prm */
} /* namespace gum */
