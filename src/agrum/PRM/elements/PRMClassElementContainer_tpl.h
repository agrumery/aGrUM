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
 * @brief Inline implementation of gum::PRMClassElementContainer
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/PRM/elements/PRMClassElement.h>
#include <agrum/PRM/elements/PRMClassElementContainer.h>

namespace gum {

  namespace prm {
    template < typename GUM_SCALAR >
    void PRMClassElementContainer< GUM_SCALAR >::copyIOFlags_(
       const PRMClassElementContainer< GUM_SCALAR >& c) {
      for (const auto& flag: c._IOFlags_)
        setIOFlag_(get(flag.first), flag.second);
    }

    template < typename GUM_SCALAR >
    INLINE
       PRMClassElementContainer< GUM_SCALAR >::PRMClassElementContainer(const std::string& name) :
        PRMObject(name) {
      GUM_CONSTRUCTOR(PRMClassElementContainer);
    }

    template < typename GUM_SCALAR >
    INLINE PRMClassElementContainer< GUM_SCALAR >::~PRMClassElementContainer() {
      GUM_DESTRUCTOR(PRMClassElementContainer);
    }

    template < typename GUM_SCALAR >
    INLINE PRMClassElementContainer< GUM_SCALAR >&
       PRMClassElementContainer< GUM_SCALAR >::operator=(
          const PRMClassElementContainer< GUM_SCALAR >& source) {
      GUM_ERROR(FatalError, "illegal call to ClassElementContainer copy operator")
    }

    template < typename GUM_SCALAR >
    INLINE PRMClassElementContainer< GUM_SCALAR >::PRMClassElementContainer(
       const PRMClassElementContainer< GUM_SCALAR >& source) :
        PRMObject(source) {
      GUM_CONS_CPY(PRMClassElementContainer);
      GUM_ERROR(FatalError, "illegal call to ClassElementContainer copy constructor")
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMClassElementContainer< GUM_SCALAR >::isInputNode(
       const PRMClassElement< GUM_SCALAR >& elt) const {
      try {
        return getIOFlag_(elt).first;
      } catch (NotFound const&) { return false; }
    }

    template < typename GUM_SCALAR >
    INLINE void PRMClassElementContainer< GUM_SCALAR >::setInputNode(
       const PRMClassElement< GUM_SCALAR >& elt,
       bool                                 b) {
      if (!exists(elt.safeName())) {
        GUM_ERROR(NotFound, ": <" + elt.safeName() + "> is not in <" + name() + ">")
      } else if (PRMClassElement< GUM_SCALAR >::isAttribute(elt)
                 || PRMClassElement< GUM_SCALAR >::isAggregate(elt)) {
        try {
          getIOFlag_(elt).first = b;
        } catch (NotFound const&) { setIOFlag_(elt, std::make_pair(b, false)); }
      } else {
        GUM_ERROR(WrongClassElement, "given id is not an PRMAttribute or an PRMAggregate")
      }
    }

    template < typename GUM_SCALAR >
    INLINE void PRMClassElementContainer< GUM_SCALAR >::setOutputNode(
       const PRMClassElement< GUM_SCALAR >& elt,
       bool                                 b) {
      if (!exists(elt.safeName())) {
        GUM_ERROR(NotFound, "<" + elt.safeName() + "> is not in <" + name() + ">")
      } else if (PRMClassElement< GUM_SCALAR >::isAttribute(elt)
                 || PRMClassElement< GUM_SCALAR >::isAggregate(elt)) {
        try {
          getIOFlag_(elt).second = b;
        } catch (NotFound const&) { setIOFlag_(elt, std::make_pair(false, b)); }

        if (b) { updateDescendants_(elt); }
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
        return !(getIOFlag_(elt).first || getIOFlag_(elt).second);
      } catch (NotFound const&) { return true; }
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMClassElementContainer< GUM_SCALAR >::isSuperTypeOf(
       const PRMClassElementContainer< GUM_SCALAR >& cec) const {
      return cec.isSubTypeOf(*this);
    }

    template < typename GUM_SCALAR >
    INLINE std::pair< bool, bool >& PRMClassElementContainer< GUM_SCALAR >::getIOFlag_(
       const PRMClassElement< GUM_SCALAR >& elt) {
      try {
        return _IOFlags_[elt.safeName()];
      } catch (NotFound const&) {
        GUM_ERROR(NotFound, "this ClassElement<GUM_SCALAR> does not have any IO flags")
      }
    }

    template < typename GUM_SCALAR >
    INLINE const std::pair< bool, bool >& PRMClassElementContainer< GUM_SCALAR >::getIOFlag_(
       const PRMClassElement< GUM_SCALAR >& elt) const {
      try {
        return _IOFlags_[elt.safeName()];
      } catch (NotFound const&) {
        GUM_ERROR(NotFound, "this ClassElement<GUM_SCALAR> does not have any IO flags")
      }
    }

    template < typename GUM_SCALAR >
    INLINE void
       PRMClassElementContainer< GUM_SCALAR >::setIOFlag_(const PRMClassElement< GUM_SCALAR >& elt,
                                                          const std::pair< bool, bool >& flags) {
      try {
        _IOFlags_[elt.safeName()] = flags;
      } catch (NotFound const&) { _IOFlags_.insert(elt.safeName(), flags); }
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMClassElementContainer< GUM_SCALAR >::exists(NodeId id) const {
      return containerDag().exists(id);
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMClassElementContainer< GUM_SCALAR >::exists(const std::string& name) const {
      try {
        get(name);
        return true;
      } catch (NotFound const&) { return false; }
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMClassElementContainer< GUM_SCALAR >::belongsTo(
       const PRMClassElement< GUM_SCALAR >& elt) const {
      try {
        return &elt == &(get(elt.safeName()));
      } catch (NotFound const&) { return false; }
    }

    template < typename GUM_SCALAR >
    INLINE const DAG& PRMClassElementContainer< GUM_SCALAR >::containerDag() const {
      return dag_();
    }

  } /* namespace prm */
} /* namespace gum */

template < typename GUM_SCALAR >
std::ostream& operator<<(std::ostream&                                           output,
                         const gum::prm::PRMClassElementContainer< GUM_SCALAR >& container) {
  std::string tab = "  ";
  output << "digraph \"" << container.name() << "\" {" << std::endl;

  for (const auto node: container.containerDag().nodes()) {
    if (container.containerDag().children(node).size() > 0) {
      for (const auto chi: container.containerDag().children(node)) {
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
