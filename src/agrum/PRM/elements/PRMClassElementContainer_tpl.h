
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
 * @brief Inline implementation of gum::PRMClassElementContainer
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <agrum/PRM/elements/PRMClassElement.h>
#include <agrum/PRM/elements/PRMClassElementContainer.h>

namespace gum {

  namespace prm {
    template < typename GUM_SCALAR >
    void PRMClassElementContainer< GUM_SCALAR >::_copyIOFlags(
       const PRMClassElementContainer< GUM_SCALAR >& c) {
      for (const auto& flag : c.__IOFlags)
        _setIOFlag(get(flag.first), flag.second);
    }

    template < typename GUM_SCALAR >
    INLINE PRMClassElementContainer< GUM_SCALAR >::PRMClassElementContainer(
       const std::string& name) :
        PRMObject(name) {
      GUM_CONSTRUCTOR(PRMClassElementContainer);
    }

    template < typename GUM_SCALAR >
    INLINE PRMClassElementContainer< GUM_SCALAR >::~PRMClassElementContainer() {
      GUM_DESTRUCTOR(PRMClassElementContainer);
    }

    template < typename GUM_SCALAR >
    INLINE PRMClassElementContainer< GUM_SCALAR >&
           PRMClassElementContainer< GUM_SCALAR >::
           operator=(const PRMClassElementContainer< GUM_SCALAR >& source) {
      GUM_ERROR(FatalError, "illegal call to ClassElementContainer copy operator");
    }

    template < typename GUM_SCALAR >
    INLINE PRMClassElementContainer< GUM_SCALAR >::PRMClassElementContainer(
       const PRMClassElementContainer< GUM_SCALAR >& source) :
        PRMObject(source) {
      GUM_CONS_CPY(PRMClassElementContainer);
      GUM_ERROR(FatalError,
                "illegal call to ClassElementContainer copy constructor");
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMClassElementContainer< GUM_SCALAR >::isInputNode(
       const PRMClassElement< GUM_SCALAR >& elt) const {
      try {
        return _getIOFlag(elt).first;
      } catch (NotFound&) { return false; }
    }

    template < typename GUM_SCALAR >
    INLINE void PRMClassElementContainer< GUM_SCALAR >::setInputNode(
       const PRMClassElement< GUM_SCALAR >& elt, bool b) {
      if (!exists(elt.safeName())) {
        GUM_ERROR(NotFound,
                  ": <" + elt.safeName() + "> is not in <" + name() + ">");
      } else if (PRMClassElement< GUM_SCALAR >::isAttribute(elt)
                 || PRMClassElement< GUM_SCALAR >::isAggregate(elt)) {
        try {
          _getIOFlag(elt).first = b;
        } catch (NotFound&) { _setIOFlag(elt, std::make_pair(b, false)); }
      } else {
        GUM_ERROR(WrongClassElement,
                  "given id is not an PRMAttribute or an PRMAggregate");
      }
    }

    template < typename GUM_SCALAR >
    INLINE void PRMClassElementContainer< GUM_SCALAR >::setOutputNode(
       const PRMClassElement< GUM_SCALAR >& elt, bool b) {
      if (!exists(elt.safeName())) {
        GUM_ERROR(NotFound, "<" + elt.safeName() + "> is not in <" + name() + ">");
      } else if (PRMClassElement< GUM_SCALAR >::isAttribute(elt)
                 || PRMClassElement< GUM_SCALAR >::isAggregate(elt)) {
        try {
          _getIOFlag(elt).second = b;
        } catch (NotFound&) { _setIOFlag(elt, std::make_pair(false, b)); }

        if (b) { _updateDescendants(elt); }
      } else {
        GUM_ERROR(WrongClassElement,
                  "given ClassElement<GUM_SCALAR> is not an "
                  "PRMAttribute or an PRMAggregate");
      }
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMClassElementContainer< GUM_SCALAR >::isInnerNode(
       const PRMClassElement< GUM_SCALAR >& elt) const {
      try {
        return !(_getIOFlag(elt).first || _getIOFlag(elt).second);
      } catch (NotFound&) { return true; }
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMClassElementContainer< GUM_SCALAR >::isSuperTypeOf(
       const PRMClassElementContainer< GUM_SCALAR >& cec) const {
      return cec.isSubTypeOf(*this);
    }

    template < typename GUM_SCALAR >
    INLINE std::pair< bool, bool >&
           PRMClassElementContainer< GUM_SCALAR >::_getIOFlag(
          const PRMClassElement< GUM_SCALAR >& elt) {
      try {
        return __IOFlags[elt.safeName()];
      } catch (NotFound&) {
        GUM_ERROR(NotFound,
                  "this ClassElement<GUM_SCALAR> does not have any IO flags");
      }
    }

    template < typename GUM_SCALAR >
    INLINE const std::pair< bool, bool >&
                 PRMClassElementContainer< GUM_SCALAR >::_getIOFlag(
          const PRMClassElement< GUM_SCALAR >& elt) const {
      try {
        return __IOFlags[elt.safeName()];
      } catch (NotFound&) {
        GUM_ERROR(NotFound,
                  "this ClassElement<GUM_SCALAR> does not have any IO flags");
      }
    }

    template < typename GUM_SCALAR >
    INLINE void PRMClassElementContainer< GUM_SCALAR >::_setIOFlag(
       const PRMClassElement< GUM_SCALAR >& elt,
       const std::pair< bool, bool >&       flags) {
      try {
        __IOFlags[elt.safeName()] = flags;
      } catch (NotFound&) { __IOFlags.insert(elt.safeName(), flags); }
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMClassElementContainer< GUM_SCALAR >::exists(NodeId id) const {
      return containerDag().exists(id);
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMClassElementContainer< GUM_SCALAR >::exists(
       const std::string& name) const {
      try {
        get(name);
        return true;
      } catch (NotFound&) { return false; }
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMClassElementContainer< GUM_SCALAR >::belongsTo(
       const PRMClassElement< GUM_SCALAR >& elt) const {
      try {
        return &elt == &(get(elt.safeName()));
      } catch (NotFound&) { return false; }
    }

    template < typename GUM_SCALAR >
    INLINE const DAG&
                 PRMClassElementContainer< GUM_SCALAR >::containerDag() const {
      return _dag();
    }

  } /* namespace prm */
} /* namespace gum */

template < typename GUM_SCALAR >
std::ostream&
   operator<<(std::ostream&                                           output,
              const gum::prm::PRMClassElementContainer< GUM_SCALAR >& container) {
  std::string tab = "  ";
  output << "digraph \"" << container.name() << "\" {" << std::endl;

  for (const auto node : container.containerDag().nodes()) {
    if (container.containerDag().children(node).size() > 0) {
      for (const auto chi : container.containerDag().children(node)) {
        output << tab << "\"" << container.get(node).name() << "\" -> "
               << "\"" << container.get(chi).name() << "\";" << std::endl;
      }
    } else if (container.containerDag().parents(node).size() == 0) {
      output << tab << "\"" << container.get(node).name() << "\";" << std::endl;
    }
  }

  output << "}" << std::endl;
  return output;
}
