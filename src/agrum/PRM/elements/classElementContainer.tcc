/**************************************************************************
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
 * @brief Inline implementation of gum::ClassElementContainer
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <agrum/PRM/elements/classElementContainer.h>
#include <agrum/PRM/elements/classElement.h>

namespace gum {

  namespace prm {
    template <typename GUM_SCALAR>
    void ClassElementContainer<GUM_SCALAR>::_copyIOFlags(
        const ClassElementContainer<GUM_SCALAR> &c) {
      for (const auto &flag : c.__IOFlags)
        _setIOFlag(get(flag.first), flag.second);
    }

    template <typename GUM_SCALAR>
    INLINE
    ClassElementContainer<GUM_SCALAR>::ClassElementContainer(const std::string &name)
        : PRMObject(name) {
      GUM_CONSTRUCTOR(ClassElementContainer);
    }

    template <typename GUM_SCALAR>
    INLINE ClassElementContainer<GUM_SCALAR>::~ClassElementContainer() {
      GUM_DESTRUCTOR(ClassElementContainer);
    }

    template <typename GUM_SCALAR>
    INLINE ClassElementContainer<GUM_SCALAR> &ClassElementContainer<GUM_SCALAR>::
    operator=(const ClassElementContainer<GUM_SCALAR> &source) {
      GUM_ERROR(FatalError, "illegal call to ClassElementContainer copy operator");
    }

    template <typename GUM_SCALAR>
    INLINE ClassElementContainer<GUM_SCALAR>::ClassElementContainer(
        const ClassElementContainer<GUM_SCALAR> &source)
        : PRMObject(source) {
      GUM_CONS_CPY(ClassElementContainer);
      GUM_ERROR(FatalError,
                "illegal call to ClassElementContainer copy constructor");
    }

    // template<typename GUM_SCALAR> INLINE
    // Property<std::pair<bool, bool>*>::onNodes&
    // ClassElementContainer<GUM_SCALAR>::_IOFlags() { return _IOFlags; }
    //
    // template<typename GUM_SCALAR> INLINE
    // const Property<std::pair<bool, bool>*>::onNodes&
    // ClassElementContainer<GUM_SCALAR>::_IOFlags() const { return _IOFlags; }
    //
    template <typename GUM_SCALAR>
    INLINE bool ClassElementContainer<GUM_SCALAR>::isInputNode(
        const ClassElement<GUM_SCALAR> &elt) const {
      try {
        return _getIOFlag(elt).first;
      } catch (NotFound &) {
        return false;
      }
    }

    template <typename GUM_SCALAR>
    INLINE void ClassElementContainer<GUM_SCALAR>::setInputNode(
        const ClassElement<GUM_SCALAR> &elt, bool b) {
      if (not exists(elt.safeName())) {
        GUM_ERROR(NotFound, ": <" + elt.safeName() + "> is not in <" + name() + ">");
      } else if (ClassElement<GUM_SCALAR>::isAttribute(elt) or
                 ClassElement<GUM_SCALAR>::isAggregate(elt)) {
        try {
          _getIOFlag(elt).second = b;
        } catch (NotFound &) {
          _setIOFlag(elt, std::make_pair(b, false));
        }
      } else {
        GUM_ERROR(WrongClassElement, "given id is not an Attribute or an Aggregate");
      }
    }

    template <typename GUM_SCALAR>
    INLINE bool ClassElementContainer<GUM_SCALAR>::isOutputNode(
        const ClassElement<GUM_SCALAR> &elt) const {
      try {
        return _getIOFlag(elt).second;
      } catch (NotFound &) {
        return false;
      }
    }

    template <typename GUM_SCALAR>
    INLINE void ClassElementContainer<GUM_SCALAR>::setOutputNode(
        const ClassElement<GUM_SCALAR> &elt, bool b) {
      if (not exists(elt.safeName())) {
        GUM_ERROR(NotFound, "<" + elt.safeName() + "> is not in <" + name() + ">");
      } else if (ClassElement<GUM_SCALAR>::isAttribute(elt) or
                 ClassElement<GUM_SCALAR>::isAggregate(elt)) {
        try {
          _getIOFlag(elt).second = b;
        } catch (NotFound &) {
          _setIOFlag(elt, std::make_pair(false, b));
        }

        if (b) {
          _updateDescendants(elt);
        }
      } else {
        GUM_ERROR(
            WrongClassElement,
            "given ClassElement<GUM_SCALAR> is not an Attribute or an Aggregate");
      }
    }

    template <typename GUM_SCALAR>
    INLINE bool ClassElementContainer<GUM_SCALAR>::isInnerNode(
        const ClassElement<GUM_SCALAR> &elt) const {
      try {
        return not(_getIOFlag(elt).first or _getIOFlag(elt).second);
      } catch (NotFound &) {
        return true;
      }
    }

    template <typename GUM_SCALAR>
    INLINE bool ClassElementContainer<GUM_SCALAR>::isSuperTypeOf(
        const ClassElementContainer<GUM_SCALAR> &cec) const {
      return cec.isSubTypeOf(*this);
    }

    template <typename GUM_SCALAR>
    INLINE std::pair<bool, bool> &ClassElementContainer<GUM_SCALAR>::_getIOFlag(
        const ClassElement<GUM_SCALAR> &elt) {
      try {
        return __IOFlags[elt.safeName()];
      } catch (NotFound &) {
        GUM_ERROR(NotFound,
                  "this ClassElement<GUM_SCALAR> does not have any IO flags");
      }
    }

    template <typename GUM_SCALAR>
    INLINE const std::pair<bool, bool> &
    ClassElementContainer<GUM_SCALAR>::_getIOFlag(
        const ClassElement<GUM_SCALAR> &elt) const {
      try {
        return __IOFlags[elt.safeName()];
      } catch (NotFound &) {
        GUM_ERROR(NotFound,
                  "this ClassElement<GUM_SCALAR> does not have any IO flags");
      }
    }

    template <typename GUM_SCALAR>
    INLINE void ClassElementContainer<GUM_SCALAR>::_setIOFlag(
        const ClassElement<GUM_SCALAR> &elt, const std::pair<bool, bool> &flags) {
      try {
        __IOFlags[elt.safeName()] = flags;
      } catch (NotFound &) {
        __IOFlags.insert(elt.safeName(), flags);
      }
    }

    template <typename GUM_SCALAR>
    INLINE bool ClassElementContainer<GUM_SCALAR>::exists(NodeId id) const {
      return dag().exists(id);
    }

    template <typename GUM_SCALAR>
    INLINE bool
    ClassElementContainer<GUM_SCALAR>::exists(const std::string &name) const {
      try {
        get(name);
        return true;
      } catch (NotFound &) {
        return false;
      }
    }

    template <typename GUM_SCALAR>
    INLINE bool ClassElementContainer<GUM_SCALAR>::belongsTo(
        const ClassElement<GUM_SCALAR> &elt) const {
      try {
        return &elt == &(get(elt.safeName()));
      } catch (NotFound &) {
        return false;
      }
    }

    template <typename GUM_SCALAR>
    INLINE const DAG &ClassElementContainer<GUM_SCALAR>::dag() const {
      return _dag();
    }

  } /* namespace prm */
} /* namespace gum */

template <typename GUM_SCALAR>
std::ostream &
operator<<(std::ostream &output,
           const gum::prm::ClassElementContainer<GUM_SCALAR> &container) {
  std::string tab = "  ";
  output << "digraph \"" << container.name() << "\" {" << std::endl;

  for (const auto node : container.dag().nodes()) {
    if (container.dag().children(node).size() > 0) {
      for (const auto chi : container.dag().children(node)) {
        output << tab << "\"" << container.get(node).name() << "\" -> "
               << "\"" << container.get(chi).name() << "\";" << std::endl;
      }
    } else if (container.dag().parents(node).size() == 0) {
      output << tab << "\"" << container.get(node).name() << "\";" << std::endl;
    }
  }

  output << "}" << std::endl;
  return output;
}
