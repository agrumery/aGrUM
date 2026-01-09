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
 * @brief Implementation of the MultiDimContainer class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <algorithm>

#include <agrum/agrum.h>

namespace gum {

  template < typename GUM_ELEMENT >
  INLINE
      MultiDimContainer< GUM_ELEMENT >::MultiDimContainer(MultiDimContainer< GUM_ELEMENT >&& from) :
      MultiDimAdressable(std::forward< MultiDimAdressable >(from)) {
    GUM_CONS_MOV(MultiDimContainer);
  }

  // Default constructor
  template < typename GUM_ELEMENT >
  INLINE MultiDimContainer< GUM_ELEMENT >::MultiDimContainer() : MultiDimAdressable() {
    GUM_CONSTRUCTOR(MultiDimContainer);
  }

  // Copy constructor
  template < typename GUM_ELEMENT >
  INLINE MultiDimContainer< GUM_ELEMENT >::MultiDimContainer(
      const MultiDimContainer< GUM_ELEMENT >& src) : MultiDimAdressable(src) {
    GUM_CONS_CPY(MultiDimContainer);
  }

  template < typename GUM_ELEMENT >
  INLINE MultiDimContainer< GUM_ELEMENT >&
         MultiDimContainer< GUM_ELEMENT >::operator=(const MultiDimContainer< GUM_ELEMENT >& from) {
    MultiDimAdressable::operator=(from);
    return *this;
  }

  template < typename GUM_ELEMENT >
  INLINE MultiDimContainer< GUM_ELEMENT >&
         MultiDimContainer< GUM_ELEMENT >::operator=(MultiDimContainer< GUM_ELEMENT >&& from) {
    GUM_OP_MOV(MultiDimContainer);
    MultiDimAdressable::operator=(std::forward< MultiDimAdressable >(from));
    return *this;
  }

  // destructor

  template < typename GUM_ELEMENT >
  INLINE MultiDimContainer< GUM_ELEMENT >::~MultiDimContainer() {
    GUM_DESTRUCTOR(MultiDimContainer);
  }

  // an [] operator using a Instantiation as argument

  template < typename GUM_ELEMENT >
  INLINE GUM_ELEMENT MultiDimContainer< GUM_ELEMENT >::operator[](const Instantiation& i) const {
    return get(i);
  }

  // an [] operator using a Instantiation as argument

  template < typename GUM_ELEMENT >
  INLINE void MultiDimContainer< GUM_ELEMENT >::set(const Instantiation& i,
                                                    const GUM_ELEMENT&   value) const {
    get_(i) = value;
  }

  // an [] operator using a Instantiation as argument

  template < typename GUM_ELEMENT >
  INLINE GUM_ELEMENT MultiDimContainer< GUM_ELEMENT >::get(const Instantiation& i) const {
    return get_(i);
  }

  // display the content of an array

  template < typename GUM_ELEMENT >
  std::string MultiDimContainer< GUM_ELEMENT >::toString() const {
    // we create a new instantiation and iterate over it to display the whole
    // content of the array
    if (this->nbrDim() == 0) { return "[]"; }

    std::stringstream ss;
    Instantiation     inst(const_cast< MultiDimContainer* >(this));

    bool first = true;

    for (inst.setFirst(); !inst.end(); ++inst) {
      if (!first) { ss << " /"; }
      first = false;

      ss << inst << " :: " << get(inst);
    }

    return ss.str();
  }

  // Test if this tensor is equal to p.

  template < typename GUM_ELEMENT >
  bool MultiDimContainer< GUM_ELEMENT >::operator==(
      const MultiDimContainer< GUM_ELEMENT >& p) const {
    if ((nbrDim() == p.nbrDim()) && (domainSize() == p.domainSize())) {
      if (nbrDim() == 0) return true;

      for (auto iter = variablesSequence().beginSafe(); iter != variablesSequence().endSafe();
           ++iter) {
        if (!p.variablesSequence().exists(*iter)) { return false; }
      }
    } else {
      return false;
    }

    Instantiation                  i(*this);
    AlmostDifferent< GUM_ELEMENT > cmp;
    for (i.setFirst(); !i.end(); ++i) {
      if (cmp(get(i), p.get(i))) { return false; }
    }

    return true;
  }

  // Test if this tensor is different of p.

  template < typename GUM_ELEMENT >
  INLINE bool MultiDimContainer< GUM_ELEMENT >::operator!=(
      const MultiDimContainer< GUM_ELEMENT >& p) const {
    return !operator==(p);
  }

  // automation fill with vector.
  template < typename GUM_ELEMENT >
  void MultiDimContainer< GUM_ELEMENT >::populate(const std::vector< GUM_ELEMENT >& v) const {
    if (domainSize() != v.size()) {
      GUM_ERROR(SizeError, "Sizes do not match : " << domainSize() << "!=" << v.size())
    }

    Size cpt = 0;

    Instantiation i(*this);

    for (i.setFirst(); !i.end(); ++i, ++cpt)
      set(i, v[cpt]);
  }

  template < typename GUM_ELEMENT >
  void MultiDimContainer< GUM_ELEMENT >::populate(std::initializer_list< GUM_ELEMENT > l) const {
    if (domainSize() != l.size()) {
      GUM_ERROR(SizeError, "Sizes do not match : " << domainSize() << "!=" << l.size())
    }

    Instantiation i(*this);
    // insert all the elements
    for (const auto& elt: l) {
      set(i, elt);
      ++i;
    }
  }

  template < typename GUM_ELEMENT >
  void MultiDimContainer< GUM_ELEMENT >::apply(std::function< GUM_ELEMENT(GUM_ELEMENT) > f) const {
    Instantiation i(*this);
    for (i.setFirst(); !i.end(); ++i) {
      set(i, f(get(i)));
    }
  }

  template < typename GUM_ELEMENT >
  GUM_ELEMENT MultiDimContainer< GUM_ELEMENT >::reduce(
      std::function< GUM_ELEMENT(GUM_ELEMENT, GUM_ELEMENT) > f,
      GUM_ELEMENT                                            base) const {
    GUM_ELEMENT   tmp = base;
    Instantiation i(*this);
    for (i.setFirst(); !i.end(); ++i) {
      tmp = f(tmp, get(i));
    }
    return tmp;
  }

  template < typename GUM_ELEMENT >
  void MultiDimContainer< GUM_ELEMENT >::copyFrom(const MultiDimContainer< GUM_ELEMENT >& src,
                                                  Instantiation* p_i) const {
    if (src.domainSize() != domainSize()) {
      GUM_ERROR(OperationNotAllowed,
                "Domain sizes do not fit : " << src.domainSize() << "!=" << domainSize());
    }

    if (p_i == nullptr) {   // if null, we just follow the same order
      Instantiation i(src);
      for (i.setFirst(); !i.end(); ++i) {
        set(i, src[i]);
      }
    } else {
      Instantiation i_dest(*this);
      Instantiation i_src(src);
      for (i_dest.setFirst(), i_src.setFirst(); !i_dest.end(); i_dest.incIn(*p_i), ++i_src) {
        set(i_dest, src[i_src]);
      }
    }
  }

  template < typename GUM_ELEMENT >
  void MultiDimContainer< GUM_ELEMENT >::extractFrom(const MultiDimContainer< GUM_ELEMENT >& src,
                                                     const Instantiation& imask) {
    this->beginMultipleChanges();

    Size nbr = this->nbrDim();
    for (Idx i = 0; i < nbr; i++) {
      this->erase(this->variable(0));
    }

    for (Idx i = 0; i < src.nbrDim(); i++) {
      if (!imask.contains(src.variable(i))) this->add(src.variable(i));
    }

    this->endMultipleChanges();

    if (this->nbrDim() == 0) { GUM_ERROR(FatalError, "Empty tensor") }

    Instantiation inst(src);
    inst.setVals(imask);
    for (inst.setFirstOut(imask); !inst.end(); inst.incOut(imask))
      set(inst, src[inst]);
  }

  template < typename GUM_ELEMENT >
  void MultiDimContainer< GUM_ELEMENT >::copyFrom(
      const MultiDimContainer< GUM_ELEMENT >& src) const {
    if (src.domainSize() != domainSize()) {
      GUM_ERROR(OperationNotAllowed,
                "Domain sizes do not fit : " << src.domainSize() << "!=" << domainSize());
    }

    Instantiation i_dest(*this);
    Instantiation i_src(src);

    for (i_dest.setFirst(), i_src.setFirst(); !i_dest.end(); ++i_dest, ++i_src) {
      set(i_dest, src[i_src]);
    }
  }

  // copy

  template < typename GUM_ELEMENT >
  void MultiDimContainer< GUM_ELEMENT >::copy(const MultiDimContainer< GUM_ELEMENT >& src) {
    this->beginMultipleChanges();

    Size nbr = this->nbrDim();

    for (Idx i = 0; i < nbr; i++) {
      this->erase(this->variable(0));
    }

    for (Idx i = 0; i < src.nbrDim(); i++) {
      this->add(src.variable(i));
    }

    this->endMultipleChanges();
    this->copyFrom(src);
  }

  template < typename GUM_ELEMENT >
  INLINE MultiDimAdressable& MultiDimContainer< GUM_ELEMENT >::getMasterRef() {
    return static_cast< MultiDimAdressable& >(*content());
  }

  template < typename GUM_ELEMENT >
  INLINE const MultiDimAdressable& MultiDimContainer< GUM_ELEMENT >::getMasterRef() const {
    return static_cast< const MultiDimAdressable& >(*content());
  }

  // display the content of an array

  template < typename GUM_ELEMENT >
  std::ostream& operator<<(std::ostream& out, const MultiDimContainer< GUM_ELEMENT >& array) {
    out << array.toString();
    return out;
  }

} /* namespace gum */
