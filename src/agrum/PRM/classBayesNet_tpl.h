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
 * @brief Inline implementation of ClassBayesNet.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(@LIP6)
 */
#include <agrum/PRM/classBayesNet.h>   // to ease IDE parser
#include <agrum/tools/core/inline.h>

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    void ClassBayesNet< GUM_SCALAR >::init__(const PRMClass< GUM_SCALAR >& c) {
      for (const auto node: c.containerDag().nodes()) {
        try {
          // Adding the attribute
          if (PRMClassElement< GUM_SCALAR >::isAttribute(c.get(node))
              || PRMClassElement< GUM_SCALAR >::isAggregate(c.get(node))) {
            const PRMClassElement< GUM_SCALAR >& elt = c.get(node);
            this->dag_.addNodeWithId(elt.id());
            this->varNodeMap__.insert(&(elt.type().variable()), &elt);
          }
        } catch (NotFound&) {
          // Not an attribute
        }
      }

      for (const auto& arc: c.containerDag().arcs()) {
        try {
          this->dag_.addArc(arc.tail(), arc.head());
        } catch (InvalidNode&) {
          // Not added means not an attribute
        }
      }
    }

    template < typename GUM_SCALAR >
    INLINE ClassBayesNet< GUM_SCALAR >::ClassBayesNet(
       const PRMClass< GUM_SCALAR >& c) :
        IBayesNet< GUM_SCALAR >(),
        class__(&c) {
      GUM_CONSTRUCTOR(ClassBayesNet);
      init__(c);
    }

    template < typename GUM_SCALAR >
    INLINE ClassBayesNet< GUM_SCALAR >::ClassBayesNet(
       const ClassBayesNet< GUM_SCALAR >& from) :
        IBayesNet< GUM_SCALAR >(from),
        class__(from.class__) {
      GUM_CONS_CPY(ClassBayesNet);
    }

    template < typename GUM_SCALAR >
    INLINE ClassBayesNet< GUM_SCALAR >::~ClassBayesNet() {
      GUM_DESTRUCTOR(ClassBayesNet);
    }

    template < typename GUM_SCALAR >
    INLINE ClassBayesNet< GUM_SCALAR >& ClassBayesNet< GUM_SCALAR >::operator=(
       const ClassBayesNet< GUM_SCALAR >& from) {
      if (this != &from) {
        IBayesNet< GUM_SCALAR >::operator=(from);

        class__ = from.class__;
      }

      return *this;
    }

    template < typename GUM_SCALAR >
    INLINE const Potential< GUM_SCALAR >&
                 ClassBayesNet< GUM_SCALAR >::cpt(NodeId varId) const {
      return get__(varId).cpf();
    }

    template < typename GUM_SCALAR >
    INLINE const VariableNodeMap&
                 ClassBayesNet< GUM_SCALAR >::variableNodeMap() const {
      GUM_ERROR(FatalError, "Sorry no VarMap in a ClassBayesNet.");
    }

    template < typename GUM_SCALAR >
    INLINE const DiscreteVariable&
                 ClassBayesNet< GUM_SCALAR >::variable(NodeId id) const {
      return get__(id).type().variable();
    }

    template < typename GUM_SCALAR >
    INLINE NodeId
       ClassBayesNet< GUM_SCALAR >::nodeId(const DiscreteVariable& var) const {
      return varNodeMap__[&var]->id();
    }

    template < typename GUM_SCALAR >
    INLINE NodeId
       ClassBayesNet< GUM_SCALAR >::idFromName(const std::string& name) const {
      return get__(name).id();
    }

    template < typename GUM_SCALAR >
    INLINE const DiscreteVariable& ClassBayesNet< GUM_SCALAR >::variableFromName(
       const std::string& name) const {
      return get__(name).type().variable();
    }

    template < typename GUM_SCALAR >
    INLINE const PRMClassElement< GUM_SCALAR >&
                 ClassBayesNet< GUM_SCALAR >::get__(NodeId id) const {
      if (this->dag_.exists(id)) {
        return class__->get(id);
      } else {
        GUM_ERROR(NotFound, "no element found with that id.");
      }
    }

    template < typename GUM_SCALAR >
    INLINE const PRMClassElement< GUM_SCALAR >&
                 ClassBayesNet< GUM_SCALAR >::get__(const std::string& name) const {
      try {
        return class__->get(name);
      } catch (NotFound&) {
        GUM_ERROR(NotFound, "no element found with that id.");
      }
    }

    template < typename GUM_SCALAR >
    INLINE const NodeProperty< Size >&
                 ClassBayesNet< GUM_SCALAR >::modalities() const {
      if (modalities__.empty()) {
        for (const auto node: this->nodes()) {
          modalities__.insert(node, (Size)variable(node).domainSize());
        }
      }

      return modalities__;
    }

    template < typename GUM_SCALAR >
    INLINE std::string ClassBayesNet< GUM_SCALAR >::toDot() const {
      std::string       tab = "  ";
      std::stringstream output;
      output << "digraph \"";
      output << class__->name() << "\" {" << std::endl;

      for (const auto node: this->nodes()) {
        if (this->children(node).size() > 0)
          for (const auto chi: this->children(node)) {
            output << tab << "\"" << variable(node).name() << "\" -> ";
            output << "\"" << variable(chi).name() << "\";" << std::endl;
          }
        else if (this->parents(node).size() == 0) {
          output << tab << "\"" << variable(node).name() << "\";" << std::endl;
        }
      }

      output << "}" << std::endl;
      return output.str();
    }

  } /* namespace prm */
} /* namespace gum */
