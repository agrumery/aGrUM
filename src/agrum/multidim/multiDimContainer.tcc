/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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

#include <agrum/config.h>

#include <agrum/multidim/multiDimContainer.h>

namespace gum {

  // Default constructor

  template <typename GUM_SCALAR>
  INLINE MultiDimContainer<GUM_SCALAR>::MultiDimContainer()
      : MultiDimAdressable() {
    GUM_CONSTRUCTOR(MultiDimContainer);
  }

  // Copy constructor

  template <typename GUM_SCALAR>
  INLINE MultiDimContainer<GUM_SCALAR>::MultiDimContainer(
      const MultiDimContainer<GUM_SCALAR> &src)
      : MultiDimAdressable(src) {
    GUM_CONS_CPY(MultiDimContainer);
  }

  // destructor

  template <typename GUM_SCALAR>
  INLINE MultiDimContainer<GUM_SCALAR>::~MultiDimContainer() {
    GUM_DESTRUCTOR(MultiDimContainer);
  }

  // an [] operator using a Instantiation as argument

  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR MultiDimContainer<GUM_SCALAR>::
  operator[](const Instantiation &i) const {
    return get(i);
  }

  // an [] operator using a Instantiation as argument

  template <typename GUM_SCALAR>
  INLINE void MultiDimContainer<GUM_SCALAR>::set(const Instantiation &i,
                                                 const GUM_SCALAR &value) const {
    _get(i) = value;
  }

  // an [] operator using a Instantiation as argument

  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR
      MultiDimContainer<GUM_SCALAR>::get(const Instantiation &i) const {
    return _get(i);
  }

  // display the content of an array

  template <typename GUM_SCALAR>
  const std::string MultiDimContainer<GUM_SCALAR>::toString() const {
    // we create a new instantiation and iterate over it to display the whole
    // content of the array
    std::stringstream ss;
    Instantiation myinst(const_cast<MultiDimContainer *>(this));

    bool first = true;

    for (myinst.setFirst(); !myinst.end(); ++myinst) {
      if (!first) {
        ss << " /";
      }

      first = false;

      ss << myinst << " :: " << get(myinst);
    }

    return ss.str();
  }

  // Test if this potential is equal to p.

  template <typename GUM_SCALAR>
  bool MultiDimContainer<GUM_SCALAR>::
  operator==(const MultiDimContainer<GUM_SCALAR> &p) const {
    if ((nbrDim() == p.nbrDim()) && (domainSize() == p.domainSize())) {
      typedef Sequence<const DiscreteVariable *>::const_iterator_safe var_iterator;

      for (var_iterator iter = variablesSequence().beginSafe();
           iter != variablesSequence().endSafe(); ++iter) {
        if (!p.variablesSequence().exists(*iter)) {
          return false;
        }
      }

      Instantiation i(*this);

      AlmostDifferent<GUM_SCALAR> cmp;

      for (i.setFirst(); !i.end(); ++i) {
        if (cmp(get(i), p.get(i))) {
          return false;
        }
      }

      return true;
    } else {
      return false;
    }
  }

  // Test if this potential is different of p.

  template <typename GUM_SCALAR>
  INLINE bool MultiDimContainer<GUM_SCALAR>::
  operator!=(const MultiDimContainer<GUM_SCALAR> &p) const {
    return !operator==(p);
  }

  // automation fill with vector.

  template <typename GUM_SCALAR>
  void
  MultiDimContainer<GUM_SCALAR>::fillWith(const std::vector<GUM_SCALAR> &v) const {
    if (domainSize() != v.size()) {
      GUM_ERROR(SizeError, "Sizes does not match in fillWith");
    }

    Size cpt = 0;

    Instantiation i(*this);

    for (i.setFirst(); !i.end(); ++i, ++cpt)
      set(i, v[cpt]);
  }

  // copyFrom

  template <typename GUM_SCALAR>
  void
  MultiDimContainer<GUM_SCALAR>::copyFrom(const MultiDimContainer<GUM_SCALAR> &src,
                                          Instantiation *p_i) const {
    if (src.domainSize() != domainSize()) {
      GUM_ERROR(OperationNotAllowed, "Domain size do not fit");
    }

    Instantiation i_dest(*this);
    Instantiation i_src(src);

    if (p_i == (Instantiation *)0) {
      for (i_dest.setFirst(), i_src.setFirst(); !i_dest.end(); ++i_dest, ++i_src) {
        set(i_dest, src[i_src]);
      }
    } else {
      for (i_dest.setFirst(), i_src.setFirst(); !i_dest.end();
           i_dest.incIn(*p_i), ++i_src) {
        set(i_dest, src[i_src]);
      }
    }
  }

  // copy

  template <typename GUM_SCALAR>
  void
  MultiDimContainer<GUM_SCALAR>::copy(const MultiDimContainer<GUM_SCALAR> &src) {
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

  // display the content of an array

  template <typename GUM_SCALAR>
  std::ostream &operator<<(std::ostream &out,
                           const MultiDimContainer<GUM_SCALAR> &array) {
    out << array.toString();
    return out;
  }

} /* namespace gum */
