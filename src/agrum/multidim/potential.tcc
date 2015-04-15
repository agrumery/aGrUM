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

// to ease IDE parser
#include <agrum/multidim/potential.h>

namespace gum {

  /// Default constructor: creates an empty null dimensional matrix
  /**
   * choose a MultiDimArray<> as decorated implementation  */

  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>::Potential()
      : MultiDimDecorator<GUM_SCALAR>(new MultiDimArray<GUM_SCALAR>()) {
    // for debugging purposes
    GUM_CONSTRUCTOR(Potential);
  }

  /// Default constructor: creates an empty null dimensional matrix

  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>::Potential(MultiDimImplementation<GUM_SCALAR> *aContent)
      : MultiDimDecorator<GUM_SCALAR>(aContent) {
    // for debugging purposes
    GUM_CONSTRUCTOR(Potential);
  }

  /// copy constructor
  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>::Potential(const Potential<GUM_SCALAR> &src)
      : Potential<GUM_SCALAR>(static_cast<MultiDimImplementation<GUM_SCALAR> *>(
                                  src.content()->newFactory()),
                              *(src.content())) {
    // for debugging purposes
    GUM_CONS_CPY(Potential);
  }

  // complex copy constructor : we choose the implementation
  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>::Potential(MultiDimImplementation<GUM_SCALAR> *aContent,
                                   const MultiDimContainer<GUM_SCALAR> &src)
      : MultiDimDecorator<GUM_SCALAR>(aContent) {
    // for debugging purposes
    GUM_CONSTRUCTOR(Potential);

    if (!src.empty()) {
      this->beginMultipleChanges();

      for (Idx i = 0; i < src.variablesSequence().size(); i++) {
        this->add(*(src.variablesSequence()[i]));
      }

      this->endMultipleChanges();

      MultiDimDecorator<GUM_SCALAR>::content()->copyFrom(
          dynamic_cast<const MultiDimContainer<GUM_SCALAR> &>(src.getMasterRef()));
    }
  }

  // operator =
  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR> &Potential<GUM_SCALAR>::
  operator=(const Potential<GUM_SCALAR> &src) {
    MultiDimDecorator<GUM_SCALAR>::content()->copy(
        dynamic_cast<const MultiDimContainer<GUM_SCALAR> &>(src.getMasterRef()));
    return *this;
  }

  /// destructor

  template <typename GUM_SCALAR> Potential<GUM_SCALAR>::~Potential() {
    // for debugging purposes
    GUM_DESTRUCTOR(Potential);
  }

  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR> &
  Potential<GUM_SCALAR>::marginalize(const Potential &p) const {
    const Sequence<const DiscreteVariable *> &seq = this->variablesSequence();
    Set<const DiscreteVariable *> delvars;

    if (p.empty()) {
      GUM_ERROR(OperationNotAllowed, "Impossible to marginalize");
    }

    for (Sequence<const DiscreteVariable *>::iterator_safe iter = seq.beginSafe();
         iter != seq.endSafe(); ++iter) {
      if (!p.contains(**iter)) {
        GUM_ERROR(OperationNotAllowed, "Impossible to marginalize");
      }
    }

    this->_swapContent(
        projectSum(p.getMasterRef(), p.variablesSequence().diffSet(seq)));

    // a const method should return a const ref BUT WE NEED t return a non const ref
    return const_cast<Potential<GUM_SCALAR> &>(*this);
  }

  template <typename GUM_SCALAR>
  INLINE Potential<GUM_SCALAR> &
  Potential<GUM_SCALAR>::multiplicateBy(const Potential &p1) {
    if (p1.empty()) {
      GUM_ERROR(OperationNotAllowed,
                "Multiplicate by empty potential is not allowed");
    }

    if (this->empty()) {
      *this = p1;
    } else {
      Potential<GUM_SCALAR> tmp;
      tmp._multiplicate(*this, p1);
      MultiDimImplementation<GUM_SCALAR> *p = this->_content;
      this->_content = tmp._content;
      tmp._content = p;
    }

    return (*this);
  }

  template <typename GUM_SCALAR>
  INLINE void Potential<GUM_SCALAR>::multiplicate(const Potential &p1,
                                                  const Potential &p2) {
    if (p1.empty()) {
      *this = p2;
      return;
    }

    if (p2.empty()) {
      *this = p1;
      return;
    }

    _multiplicate(p1, p2);
  }

  template <typename GUM_SCALAR>
  void Potential<GUM_SCALAR>::_multiplicate(const Potential &p1,
                                            const Potential &p2) {
    this->beginMultipleChanges();
    // remove vars in this : WARNING -- THIS IS A COPY OF SEQ !!!!
    const Sequence<const DiscreteVariable *> seq0 = this->variablesSequence();

    for (Sequence<const DiscreteVariable *>::iterator_safe iter = seq0.beginSafe();
         iter != seq0.endSafe(); ++iter) {
      this->erase(**iter);
    }

    // adding vars in p1
    const Sequence<const DiscreteVariable *> &seq1 = p1.variablesSequence();

    for (Sequence<const DiscreteVariable *>::iterator_safe iter = seq1.beginSafe();
         iter != seq1.endSafe(); ++iter) {
      this->add(**iter);
    }

    // adding vars in p2 not already there
    const Sequence<const DiscreteVariable *> &seq2 = p2.variablesSequence();

    for (Sequence<const DiscreteVariable *>::iterator_safe iter = seq2.beginSafe();
         iter != seq2.endSafe(); ++iter) {
      if (!this->contains(**iter)) {
        this->add(**iter);
      }
    }

    this->endMultipleChanges();

    Instantiation i(this);
    // it looks like we don't need much more optimization (all the sums & prods
    // have to be made once at least) ...
    // remember that p1[i] means p1[just the part of i that concerns p1]

    for (i.setFirst(); !i.end(); ++i)
      this->set(i, p1[i] * p2[i]);
  }

  /// sum of all elements in this
  template <typename GUM_SCALAR>
  INLINE const GUM_SCALAR Potential<GUM_SCALAR>::sum() const {
    Instantiation i(this->_content);
    GUM_SCALAR s = (GUM_SCALAR)0;

    for (i.setFirst(); !i.end(); ++i)
      s += this->get(i);

    return s;
  }

  /// normalisation of this
  /// do nothing is sum is 0
  template <typename GUM_SCALAR>
  INLINE Potential<GUM_SCALAR> &Potential<GUM_SCALAR>::normalize() const {
    GUM_SCALAR s = sum();

    if (s != (GUM_SCALAR)0) {
      Instantiation i(this->_content);

      for (i.setFirst(); !i.end(); ++i)
        this->set(i, this->get(i) / s);
    }

    // a const method should return a const ref BUT WE NEED t return a non const ref
    return const_cast<Potential<GUM_SCALAR> &>(*this);
  }

  template <typename GUM_SCALAR>
  INLINE Potential<GUM_SCALAR> *Potential<GUM_SCALAR>::newFactory() const {
    return new Potential<GUM_SCALAR>(
        static_cast<MultiDimImplementation<GUM_SCALAR> *>(
            this->content()->newFactory()));
  }

  template <typename GUM_SCALAR>
  INLINE void Potential<GUM_SCALAR>::_swap(const DiscreteVariable *x,
                                           const DiscreteVariable *y) {
    MultiDimDecorator<GUM_SCALAR>::content()->swap(*x, *y);
  }

  /// string representation of this.
  template <typename GUM_SCALAR>
  INLINE const std::string Potential<GUM_SCALAR>::toString() const {
    return MultiDimDecorator<GUM_SCALAR>::content()->toString();
  }
} /* namespace gum */

// kate: indent-mode cstyle; indent-width 2; replace-tabs on;
