/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/PRM/instanceBayesNet.h>   // to ease IDE parser

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    void InstanceBayesNet< GUM_SCALAR >::_init_(const PRMInstance< GUM_SCALAR >& i) {
      for (const auto node: i.type().containerDag().nodes()) {
        try {
          // Adding the attribute
          const PRMAttribute< GUM_SCALAR >& attr = i.get(node);
          this->dag_.addNodeWithId(attr.id());
          _varNodeMap_.insert(&(attr.type().variable()), &attr);
        } catch (NotFound const&) {
          // Not an attribute
        }
      }

      for (const auto& arc: i.type().containerDag().arcs()) {
        try {
          this->dag_.addArc(arc.tail(), arc.head());
        } catch (InvalidNode const&) {
          // Not added means not an attribute
        }
      }
    }

    template < typename GUM_SCALAR >
    INLINE InstanceBayesNet< GUM_SCALAR >::InstanceBayesNet(const PRMInstance< GUM_SCALAR >& i) :
        IBayesNet< GUM_SCALAR >(), _inst_(&i) {
      GUM_CONSTRUCTOR(InstanceBayesNet);
      _init_(i);
    }

    template < typename GUM_SCALAR >
    INLINE InstanceBayesNet< GUM_SCALAR >::InstanceBayesNet(const InstanceBayesNet& from) :
        IBayesNet< GUM_SCALAR >(from), _varNodeMap_(from._varNodeMap_), _inst_(from._inst_) {
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

        _varNodeMap_ = from._varNodeMap_;
      }

      return *this;
    }

    template < typename GUM_SCALAR >
    INLINE const Potential< GUM_SCALAR >& InstanceBayesNet< GUM_SCALAR >::cpt(NodeId varId) const {
      return _get_(varId).cpf();
    }

    template < typename GUM_SCALAR >
    INLINE const VariableNodeMap& InstanceBayesNet< GUM_SCALAR >::variableNodeMap() const {
      GUM_ERROR(NotFound, "no VariableNodeMap in an InstanceBayesNet")
    }

    template < typename GUM_SCALAR >
    INLINE const DiscreteVariable& InstanceBayesNet< GUM_SCALAR >::variable(NodeId id) const {
      return _get_(id).type().variable();
    }

    template < typename GUM_SCALAR >
    INLINE NodeId InstanceBayesNet< GUM_SCALAR >::nodeId(const DiscreteVariable& var) const {
      return _varNodeMap_[&var]->id();
    }

    template < typename GUM_SCALAR >
    INLINE NodeId InstanceBayesNet< GUM_SCALAR >::idFromName(const std::string& name) const {
      return _get_(name).id();
    }

    template < typename GUM_SCALAR >
    INLINE const DiscreteVariable&
       InstanceBayesNet< GUM_SCALAR >::variableFromName(const std::string& name) const {
      return _get_(name).type().variable();
    }

    template < typename GUM_SCALAR >
    INLINE const PRMClassElement< GUM_SCALAR >&
                 InstanceBayesNet< GUM_SCALAR >::_get_(NodeId id) const {
      return _inst_->get(id);
    }

    template < typename GUM_SCALAR >
    INLINE const PRMClassElement< GUM_SCALAR >&
                 InstanceBayesNet< GUM_SCALAR >::_get_(const std::string& name) const {
      try {
        return _inst_->get(name);
      } catch (NotFound const&) { GUM_ERROR(NotFound, "no element found with that name") }
    }

    template < typename GUM_SCALAR >
    INLINE const NodeProperty< Size >& InstanceBayesNet< GUM_SCALAR >::modalities() const {
      if (_modalities_.empty()) {
        for (const auto node: this->nodes()) {
          _modalities_.insert(node, variable(node).domainSize());
        }
      }

      return _modalities_;
    }

    template < typename GUM_SCALAR >
    INLINE std::string InstanceBayesNet< GUM_SCALAR >::toDot() const {
      std::string       tab = "  ";
      std::stringstream output;
      output << "digraph \"";
      output << _inst_->name() << "\" {" << std::endl;

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
