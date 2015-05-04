/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr   *
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

// include the operators that will be used by the decorators
#include <agrum/multidim/operators/operators4MultiDim.h>
#include <agrum/multidim/operators/projections4MultiDim.h>
#include <agrum/multidim/operators/completeProjections4MultiDim.h>
#include <agrum/multidim/partialInstantiation4MultiDim.h>

// to ease IDE parsers
#include <agrum/multidim/multiDimDecorator.h>

namespace gum {

  /// constructor

  template <typename GUM_SCALAR>
  INLINE MultiDimDecorator<GUM_SCALAR>::MultiDimDecorator(
      MultiDimImplementation<GUM_SCALAR> *aContent)
      : _content(aContent) {
    GUM_CONSTRUCTOR(MultiDimDecorator);

    static bool first = true;

    if (first) {
      first = false;

      // register the operators that will be used by the decorator
      Operators4MultiDimInitialize<GUM_SCALAR> op;
      op.init();

      // register the projectors that will be used by the decorator
      Projections4MultiDimInitialize<GUM_SCALAR> proj;
      proj.init();

      // register the projectors that will be used by the decorator
      CompleteProjections4MultiDimInitialize<GUM_SCALAR> comp_proj;
      comp_proj.init();

      // register the partial instantiators that will be used by the decorator
      PartialInstantiation4MultiDimInitialize<GUM_SCALAR> inst;
      inst.init();
    }
  }

  /// destructor

  template <typename GUM_SCALAR>
  INLINE MultiDimDecorator<GUM_SCALAR>::~MultiDimDecorator() {
    if (_content) {
      delete _content;
    }

    GUM_DESTRUCTOR(MultiDimDecorator);
  }

  /// return a data, given a Insantiation - final method

  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR &
  MultiDimDecorator<GUM_SCALAR>::_get(const Instantiation &i) const {
    GUM_ERROR(OperationNotAllowed, "_get in the implementation !");
  }

  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR
      MultiDimDecorator<GUM_SCALAR>::get(const Instantiation &i) const {
    return ((MultiDimContainer<GUM_SCALAR> *)_content)->get(i);
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecorator<GUM_SCALAR>::set(const Instantiation &i,
                                                 const GUM_SCALAR &value) const {
    return ((MultiDimContainer<GUM_SCALAR> *)_content)->set(i, value);
  }

  /// get the size of domains - final method

  template <typename GUM_SCALAR>
  INLINE Size MultiDimDecorator<GUM_SCALAR>::domainSize() const {
    return ((MultiDimContainer<GUM_SCALAR> *)_content)->domainSize();
  }

  /// add a new var to the sequence of vars - final method

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecorator<GUM_SCALAR>::add(const DiscreteVariable &v) {
    ((MultiDimContainer<GUM_SCALAR> *)_content)->add(v);
  }

  /// listen to change in each recorded Instantiation. final method

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecorator<GUM_SCALAR>::changeNotification(
      Instantiation &i, const DiscreteVariable *const var, const Idx &oldval,
      const Idx &newval) {
    ((MultiDimContainer<GUM_SCALAR> *)_content)
        ->changeNotification(i, var, oldval, newval);
  }

  /// listen to an assignment of a value in a Instantiation

  template <typename GUM_SCALAR>
  INLINE void
  MultiDimDecorator<GUM_SCALAR>::setChangeNotification(Instantiation &i) {
    ((MultiDimContainer<GUM_SCALAR> *)_content)->setChangeNotification(i);
  }

  /// listen to setFirst in each recorded Instantiation. final method.

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecorator<GUM_SCALAR>::setFirstNotification(Instantiation &i) {
    ((MultiDimContainer<GUM_SCALAR> *)_content)->setFirstNotification(i);
  }

  /// listen to setLast in each recorded Instantiation. final method.

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecorator<GUM_SCALAR>::setLastNotification(Instantiation &i) {
    ((MultiDimContainer<GUM_SCALAR> *)_content)->setLastNotification(i);
  }

  /// listen to increment in each recorded Instantiation. final method.

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecorator<GUM_SCALAR>::setIncNotification(Instantiation &i) {
    ((MultiDimContainer<GUM_SCALAR> *)_content)->setIncNotification(i);
  }

  /// listen to increment in each recorded Instantiation. final method.

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecorator<GUM_SCALAR>::setDecNotification(Instantiation &i) {
    ((MultiDimContainer<GUM_SCALAR> *)_content)->setDecNotification(i);
  }

  /// add a Instantiation as a slave of this

  template <typename GUM_SCALAR>
  INLINE bool MultiDimDecorator<GUM_SCALAR>::registerSlave(Instantiation &i) {
    return ((MultiDimContainer<GUM_SCALAR> *)_content)->registerSlave(i);
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecorator<GUM_SCALAR>::erase(const DiscreteVariable &d) {
    ((MultiDimContainer<GUM_SCALAR> *)_content)->erase(d);
  }

  template <typename GUM_SCALAR>
  INLINE const DiscreteVariable &
  MultiDimDecorator<GUM_SCALAR>::variable(Idx i) const {
    return ((MultiDimContainer<GUM_SCALAR> *)_content)->variable(i);
  }

  template <typename GUM_SCALAR>
  INLINE Idx MultiDimDecorator<GUM_SCALAR>::pos(const DiscreteVariable &d) const {
    return ((MultiDimContainer<GUM_SCALAR> *)_content)->pos(d);
  }

  template <typename GUM_SCALAR>
  INLINE bool
  MultiDimDecorator<GUM_SCALAR>::contains(const DiscreteVariable &d) const {
    return ((MultiDimContainer<GUM_SCALAR> *)_content)->contains(d);
  }

  template <typename GUM_SCALAR>
  INLINE bool MultiDimDecorator<GUM_SCALAR>::empty() const {
    return ((MultiDimContainer<GUM_SCALAR> *)_content)->empty();
  }

  template <typename GUM_SCALAR>
  INLINE bool MultiDimDecorator<GUM_SCALAR>::unregisterSlave(Instantiation &i) {
    return ((MultiDimContainer<GUM_SCALAR> *)_content)->unregisterSlave(i);
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecorator<GUM_SCALAR>::fill(const GUM_SCALAR &d) const {
    ((MultiDimContainer<GUM_SCALAR> *)_content)->fill(d);
  }

  /// notification modification on vars to all Instantiation listeners.

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecorator<GUM_SCALAR>::notifyChange() const {
    /*( (MultiDimContainer<GUM_SCALAR> *) _content)->notifyChange();*/
    GUM_ERROR(OperationNotAllowed, "Not implemented yet");
  }

  /// give a const ref to the sequence of DiscreteVariable*. final method.

  template <typename GUM_SCALAR>
  INLINE const Sequence<const DiscreteVariable *> &
  MultiDimDecorator<GUM_SCALAR>::variablesSequence() const {
    return ((MultiDimContainer<GUM_SCALAR> *)_content)->variablesSequence();
  }

  /// get the nbr of vars in the sequence. final method

  template <typename GUM_SCALAR>
  INLINE Idx MultiDimDecorator<GUM_SCALAR>::nbrDim() const {
    return ((MultiDimContainer<GUM_SCALAR> *)_content)->nbrDim();
  }

  /// In order to insure the deref. for decorators, we need to virtualize the
  /// access to master pointer
  template <typename GUM_SCALAR>
  INLINE MultiDimImplementation<GUM_SCALAR> &
  MultiDimDecorator<GUM_SCALAR>::getMasterRef(void) {
    return *_content;
  }

  /// In order to insure the deref. for decorators, we need to virtualize the
  /// access to master pointer
  template <typename GUM_SCALAR>
  INLINE const MultiDimImplementation<GUM_SCALAR> &
  MultiDimDecorator<GUM_SCALAR>::getMasterRef(void) const {
    return *_content;
  }

  /// protected access to _content
  template <typename GUM_SCALAR>
  INLINE MultiDimImplementation<GUM_SCALAR> *
  MultiDimDecorator<GUM_SCALAR>::content() {
    return _content;
  }

  /// protected access to _content
  template <typename GUM_SCALAR>
  INLINE const MultiDimImplementation<GUM_SCALAR> *
  MultiDimDecorator<GUM_SCALAR>::content() const {
    return _content;
  }

  /// string representation of internal data about i in this.
  template <typename GUM_SCALAR>
  INLINE const std::string
  MultiDimDecorator<GUM_SCALAR>::toString(const Instantiation *i) const {
    return _content->toString(i);
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecorator<GUM_SCALAR>::beginMultipleChanges(void) {
    ((MultiDimContainer<GUM_SCALAR> *)_content)->beginMultipleChanges();
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecorator<GUM_SCALAR>::endMultipleChanges(void) {
    ((MultiDimContainer<GUM_SCALAR> *)_content)->endMultipleChanges();
  }

  template <typename GUM_SCALAR>
  INLINE void
  MultiDimDecorator<GUM_SCALAR>::endMultipleChanges(const GUM_SCALAR &x) {
    ((MultiDimContainer<GUM_SCALAR> *)_content)->endMultipleChanges(x);
  }

  /// Perform an homothety on a multiDim container
  template <typename GUM_SCALAR>
  INLINE void MultiDimDecorator<GUM_SCALAR>::homothetic(
      const GUM_SCALAR alpha,
      GUM_SCALAR (*mul)(const GUM_SCALAR, const GUM_SCALAR)) {
    Instantiation i(this);
    i.setFirst();

    while (!i.end()) {
      set(i, mul(alpha, get(i)));
      ++i;
    }
  }

  /// iterate add on each element of a multiDim container
  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR MultiDimDecorator<GUM_SCALAR>::fold(
      GUM_SCALAR (*add)(const GUM_SCALAR, const GUM_SCALAR)) const {
    Instantiation i(this);
    GUM_SCALAR res;
    i.setFirst();
    res = get(i);
    ++i;

    while (!i.end()) {
      res = add(res, get(i));
      ++i;
    }

    return res;
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecorator<GUM_SCALAR>::_swapContent(
      MultiDimImplementation<GUM_SCALAR> *aContent) const {
    if (aContent != 0) {
      // TODO : frees all slave instantiations
      // TODO : control the dimensions ?
      MultiDimImplementation<GUM_SCALAR> *tmp = _content;
      _content = aContent;
      /// registers all instantiations
      delete (tmp);
    }
  }
  // template <typename GUM_SCALAR>
  // MultiDimDecorator<GUM_SCALAR>* MultiDimDecorator<GUM_SCALAR>::newFactory() const
  // {
  //   return new MultiDimDecorator<GUM_SCALAR>
  //     ( static_cast<MultiDimImplementation<GUM_SCALAR>*>
  //       ( this->getContent()->newFactory() ) );
  // }

  template <typename GUM_SCALAR>
  INLINE const std::string MultiDimDecorator<GUM_SCALAR>::toString() const {
    return content()->toString();
  }

} /* namespace gum */
