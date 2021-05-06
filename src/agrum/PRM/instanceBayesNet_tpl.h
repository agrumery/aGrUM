/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Inline implementation of InstanceBayesNet.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <agrum/PRM/instanceBayesNet.h>   // to ease IDE parser

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    void
       InstanceBayesNet< GUM_SCALAR >::init__(const PRMInstance< GUM_SCALAR >& i) {
      for (const auto node: i.type().containerDag().nodes()) {
        try {
          // Adding the attribute
          const PRMAttribute< GUM_SCALAR >& attr = i.get(node);
          this->dag_.addNodeWithId(attr.id());
          varNodeMap__.insert(&(attr.type().variable()), &attr);
        } catch (NotFound&) {
          // Not an attribute
        }
      }

      for (const auto& arc: i.type().containerDag().arcs()) {
        try {
          this->dag_.addArc(arc.tail(), arc.head());
        } catch (InvalidNode&) {
          // Not added means not an attribute
        }
      }
    }

    template < typename GUM_SCALAR >
    INLINE InstanceBayesNet< GUM_SCALAR >::InstanceBayesNet(
       const PRMInstance< GUM_SCALAR >& i) :
        IBayesNet< GUM_SCALAR >(),
        inst__(&i) {
      GUM_CONSTRUCTOR(InstanceBayesNet);
      init__(i);
    }

    template < typename GUM_SCALAR >
    INLINE InstanceBayesNet< GUM_SCALAR >::InstanceBayesNet(
       const InstanceBayesNet& from) :
        IBayesNet< GUM_SCALAR >(from),
        varNodeMap__(from.varNodeMap__), inst__(from.inst__) {
      GUM_CONS_CPY(InstanceBayesNet);
    }

    template < typename GUM_SCALAR >
    INLINE InstanceBayesNet< GUM_SCALAR >::~InstanceBayesNet() {
      GUM_DESTRUCTOR(InstanceBayesNet);
    }

    template < typename GUM_SCALAR >
    INLINE InstanceBayesNet< GUM_SCALAR >&
       InstanceBayesNet< GUM_SCALAR >::operator=(const InstanceBayesNet& from) {
      if (this != &from) {
        IBayesNet< GUM_SCALAR >::operator=(from);

        varNodeMap__ = from.varNodeMap__;
      }

      return *this;
    }

    template < typename GUM_SCALAR >
    INLINE const Potential< GUM_SCALAR >&
                 InstanceBayesNet< GUM_SCALAR >::cpt(NodeId varId) const {
      return get__(varId).cpf();
    }

    template < typename GUM_SCALAR >
    INLINE const VariableNodeMap&
                 InstanceBayesNet< GUM_SCALAR >::variableNodeMap() const {
      GUM_ERROR(NotFound, "no VariableNodeMap in an InstanceBayesNet")
    }

    template < typename GUM_SCALAR >
    INLINE const DiscreteVariable&
                 InstanceBayesNet< GUM_SCALAR >::variable(NodeId id) const {
      return get__(id).type().variable();
    }

    template < typename GUM_SCALAR >
    INLINE NodeId
       InstanceBayesNet< GUM_SCALAR >::nodeId(const DiscreteVariable& var) const {
      return varNodeMap__[&var]->id();
    }

    template < typename GUM_SCALAR >
    INLINE NodeId
       InstanceBayesNet< GUM_SCALAR >::idFromName(const std::string& name) const {
      return get__(name).id();
    }

    template < typename GUM_SCALAR >
    INLINE const DiscreteVariable&
                 InstanceBayesNet< GUM_SCALAR >::variableFromName(
          const std::string& name) const {
      return get__(name).type().variable();
    }

    template < typename GUM_SCALAR >
    INLINE const PRMClassElement< GUM_SCALAR >&
                 InstanceBayesNet< GUM_SCALAR >::get__(NodeId id) const {
      return inst__->get(id);
    }

    template < typename GUM_SCALAR >
    INLINE const PRMClassElement< GUM_SCALAR >&
       InstanceBayesNet< GUM_SCALAR >::get__(const std::string& name) const {
      try {
        return inst__->get(name);
      } catch (NotFound&) {
        GUM_ERROR(NotFound, "no element found with that name")
      }
    }

    template < typename GUM_SCALAR >
    INLINE const NodeProperty< Size >&
                 InstanceBayesNet< GUM_SCALAR >::modalities() const {
      if (modalities__.empty()) {
        for (const auto node: this->nodes()) {
          modalities__.insert(node, variable(node).domainSize());
        }
      }

      return modalities__;
    }

    template < typename GUM_SCALAR >
    INLINE std::string InstanceBayesNet< GUM_SCALAR >::toDot() const {
      std::string       tab = "  ";
      std::stringstream output;
      output << "digraph \"";
      output << inst__->name() << "\" {" << std::endl;

      for (const auto node: this->nodes()) {
        if (this->children(node).size() > 0) {
          const NodeSet& children = this->children(node);

          for (const auto chi: children) {
            output << tab << "\"" << variable(node).name() << "\" -> ";
            output << "\"" << variable(chi).name() << "\";" << std::endl;
          }
        } else if (this->parents(node).size() == 0) {
          output << tab << "\"" << variable(node).name() << "\";" << std::endl;
        }
      }

      output << "}" << std::endl;
      return output.str();
    }

  } /* namespace prm */
} /* namespace gum */
