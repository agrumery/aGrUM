/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) et Christophe GONZALES(_at_AMU)
 * (_at_AMU) info_at_agrum_dot_org
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
 * @brief Implementation of the MultiDimContainer class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <agrum/agrum.h>
#include <algorithm>


namespace gum {

  template < typename GUM_SCALAR >
  INLINE
     MultiDimContainer< GUM_SCALAR >::MultiDimContainer(MultiDimContainer< GUM_SCALAR >&& from) :
      MultiDimAdressable(std::forward< MultiDimAdressable >(from)) {
    GUM_CONS_MOV(MultiDimContainer);
  }

  // Default constructor
  template < typename GUM_SCALAR >
  INLINE MultiDimContainer< GUM_SCALAR >::MultiDimContainer() : MultiDimAdressable() {
    GUM_CONSTRUCTOR(MultiDimContainer);
  }

  // Copy constructor
  template < typename GUM_SCALAR >
  INLINE MultiDimContainer< GUM_SCALAR >::MultiDimContainer(
     const MultiDimContainer< GUM_SCALAR >& src) :
      MultiDimAdressable(src) {
    GUM_CONS_CPY(MultiDimContainer);
  }
  template < typename GUM_SCALAR >
  INLINE MultiDimContainer< GUM_SCALAR >&
     MultiDimContainer< GUM_SCALAR >::operator=(const MultiDimContainer< GUM_SCALAR >& from) {
    MultiDimAdressable::operator=(from);
    return *this;
  }
  template < typename GUM_SCALAR >
  INLINE MultiDimContainer< GUM_SCALAR >&
     MultiDimContainer< GUM_SCALAR >::operator=(MultiDimContainer< GUM_SCALAR >&& from) {
    GUM_OP_MOV(MultiDimContainer);
    MultiDimAdressable::operator=(std::forward< MultiDimAdressable >(from));
    return *this;
  }

  // destructor

  template < typename GUM_SCALAR >
  INLINE MultiDimContainer< GUM_SCALAR >::~MultiDimContainer() {
    GUM_DESTRUCTOR(MultiDimContainer);
  }

  // an [] operator using a Instantiation as argument

  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR MultiDimContainer< GUM_SCALAR >::operator[](const Instantiation& i) const {
    return get(i);
  }

  // an [] operator using a Instantiation as argument

  template < typename GUM_SCALAR >
  INLINE void MultiDimContainer< GUM_SCALAR >::set(const Instantiation& i,
                                                   const GUM_SCALAR&    value) const {
    get_(i) = value;
  }

  // an [] operator using a Instantiation as argument

  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR MultiDimContainer< GUM_SCALAR >::get(const Instantiation& i) const {
    return get_(i);
  }

  // display the content of an array

  template < typename GUM_SCALAR >
  std::string MultiDimContainer< GUM_SCALAR >::toString() const {
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

  // Test if this potential is equal to p.

  template < typename GUM_SCALAR >
  bool MultiDimContainer< GUM_SCALAR >::operator==(const MultiDimContainer< GUM_SCALAR >& p) const {
    if ((nbrDim() == p.nbrDim()) && (domainSize() == p.domainSize())) {
      if (nbrDim() == 0) return true;

      for (auto iter = variablesSequence().beginSafe(); iter != variablesSequence().endSafe();
           ++iter) {
        if (!p.variablesSequence().exists(*iter)) { return false; }
      }
    } else {
      return false;
    }

    Instantiation                 i(*this);
    AlmostDifferent< GUM_SCALAR > cmp;
    for (i.setFirst(); !i.end(); ++i) {
      if (cmp(get(i), p.get(i))) { return false; }
    }

    return true;
  }

  // Test if this potential is different of p.

  template < typename GUM_SCALAR >
  INLINE bool
     MultiDimContainer< GUM_SCALAR >::operator!=(const MultiDimContainer< GUM_SCALAR >& p) const {
    return !operator==(p);
  }

  // automation fill with vector.
  template < typename GUM_SCALAR >
  void MultiDimContainer< GUM_SCALAR >::populate(const std::vector< GUM_SCALAR >& v) const {
    if (domainSize() != v.size()) {
      GUM_ERROR(SizeError, "Sizes do not match : " << domainSize() << "!=" << v.size())
    }

    Size cpt = 0;

    Instantiation i(*this);

    for (i.setFirst(); !i.end(); ++i, ++cpt)
      set(i, v[cpt]);
  }

  template < typename GUM_SCALAR >
  void MultiDimContainer< GUM_SCALAR >::populate(std::initializer_list< GUM_SCALAR > l) const {
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

  template < typename GUM_SCALAR >
  void MultiDimContainer< GUM_SCALAR >::apply(std::function< GUM_SCALAR(GUM_SCALAR) > f) const {
    Instantiation i(*this);
    for (i.setFirst(); !i.end(); ++i) {
      set(i, f(get(i)));
    }
  }

  template < typename GUM_SCALAR >
  GUM_SCALAR
     MultiDimContainer< GUM_SCALAR >::reduce(std::function< GUM_SCALAR(GUM_SCALAR, GUM_SCALAR) > f,
                                             GUM_SCALAR base) const {
    GUM_SCALAR    tmp = base;
    Instantiation i(*this);
    for (i.setFirst(); !i.end(); ++i) {
      tmp = f(tmp, get(i));
    }
    return tmp;
  }


  template < typename GUM_SCALAR >
  void MultiDimContainer< GUM_SCALAR >::copyFrom(const MultiDimContainer< GUM_SCALAR >& src,
                                                 Instantiation*                         p_i) const {
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

  template < typename GUM_SCALAR >
  void MultiDimContainer< GUM_SCALAR >::extractFrom(const MultiDimContainer< GUM_SCALAR >& src,
                                                    const Instantiation&                   imask) {
    this->beginMultipleChanges();

    Size nbr = this->nbrDim();
    for (Idx i = 0; i < nbr; i++) {
      this->erase(this->variable(0));
    }

    for (Idx i = 0; i < src.nbrDim(); i++) {
      if (!imask.contains(src.variable(i))) this->add(src.variable(i));
    }

    this->endMultipleChanges();

    if (this->nbrDim() == 0) { GUM_ERROR(FatalError, "Empty potential") }

    Instantiation inst(src);
    inst.setVals(imask);
    for (inst.setFirstOut(imask); !inst.end(); inst.incOut(imask))
      set(inst, src[inst]);
  }

  template < typename GUM_SCALAR >
  void MultiDimContainer< GUM_SCALAR >::copyFrom(const MultiDimContainer< GUM_SCALAR >& src) const {
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

  template < typename GUM_SCALAR >
  void MultiDimContainer< GUM_SCALAR >::copy(const MultiDimContainer< GUM_SCALAR >& src) {
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

  template < typename GUM_SCALAR >
  INLINE MultiDimAdressable& MultiDimContainer< GUM_SCALAR >::getMasterRef() {
    return static_cast< MultiDimAdressable& >(*content());
  }

  template < typename GUM_SCALAR >
  INLINE const MultiDimAdressable& MultiDimContainer< GUM_SCALAR >::getMasterRef() const {
    return static_cast< const MultiDimAdressable& >(*content());
  }

  // display the content of an array

  template < typename GUM_SCALAR >
  std::ostream& operator<<(std::ostream& out, const MultiDimContainer< GUM_SCALAR >& array) {
    out << array.toString();
    return out;
  }

} /* namespace gum */
