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
        const PRMClassElementContainer< GUM_SCALAR >& source) : PRMObject(source) {
      GUM_CONS_CPY(PRMClassElementContainer);
      GUM_ERROR(FatalError, "illegal call to ClassElementContainer copy constructor")
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMClassElementContainer< GUM_SCALAR >::isInputNode(
        const PRMClassElement< GUM_SCALAR >& elt) const {
      const auto* p = _IOFlags_.tryGet(elt.safeName());
      if (p == nullptr) return false;
      return p->first;
    }

    template < typename GUM_SCALAR >
    INLINE void PRMClassElementContainer< GUM_SCALAR >::setInputNode(
        const PRMClassElement< GUM_SCALAR >& elt,
        bool                                 b) {
      if (!exists(elt.safeName())) {
        GUM_ERROR(NotFound, ": <" + elt.safeName() + "> is not in <" + name() + ">")
      } else if (PRMClassElement< GUM_SCALAR >::isAttribute(elt)
                 || PRMClassElement< GUM_SCALAR >::isAggregate(elt)) {
        if (auto* p = _IOFlags_.tryGet(elt.safeName()); p != nullptr) {
          p->first = b;
        } else { setIOFlag_(elt, std::make_pair(b, false)); }
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
        if (auto* p = _IOFlags_.tryGet(elt.safeName()); p != nullptr) {
          p->second = b;
        } else { setIOFlag_(elt, std::make_pair(false, b)); }

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
      const auto* p = _IOFlags_.tryGet(elt.safeName());
      if (p == nullptr) return true;
      return !(p->first || p->second);
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMClassElementContainer< GUM_SCALAR >::isSuperTypeOf(
        const PRMClassElementContainer< GUM_SCALAR >& cec) const {
      return cec.isSubTypeOf(*this);
    }

    template < typename GUM_SCALAR >
    INLINE std::pair< bool, bool >& PRMClassElementContainer< GUM_SCALAR >::getIOFlag_(
        const PRMClassElement< GUM_SCALAR >& elt) {
      auto* p = _IOFlags_.tryGet(elt.safeName());
      if (p == nullptr)
        GUM_ERROR(NotFound, "this ClassElement<GUM_SCALAR> does not have any IO flags")
      return *p;
    }

    template < typename GUM_SCALAR >
    INLINE const std::pair< bool, bool >& PRMClassElementContainer< GUM_SCALAR >::getIOFlag_(
        const PRMClassElement< GUM_SCALAR >& elt) const {
      const auto* p = _IOFlags_.tryGet(elt.safeName());
      if (p == nullptr)
        GUM_ERROR(NotFound, "this ClassElement<GUM_SCALAR> does not have any IO flags")
      return *p;
    }

    template < typename GUM_SCALAR >
    INLINE void
        PRMClassElementContainer< GUM_SCALAR >::setIOFlag_(const PRMClassElement< GUM_SCALAR >& elt,
                                                           const std::pair< bool, bool >& flags) {
      if (auto* p = _IOFlags_.tryGet(elt.safeName()); p != nullptr) {
        *p = flags;
      } else { _IOFlags_.insert(elt.safeName(), flags); }
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
