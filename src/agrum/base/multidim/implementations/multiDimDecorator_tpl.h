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
 * @brief Implementation for MultiDimDecorator.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <mutex>

// include the operators that will be used by the decorators
#include <agrum/base/multidim/implementations/multiDimDecorator.h>
#include <agrum/base/multidim/utils/operators/completeProjections4MultiDim.h>
#include <agrum/base/multidim/utils/operators/operators4MultiDim.h>
#include <agrum/base/multidim/utils/operators/projections4MultiDim.h>
#include <agrum/base/multidim/utils/partialInstantiation4MultiDim.h>

namespace gum {
  // instrumental and non-API function
  template < typename GUM_ELEMENT >
  void _initTensorOperators__() {
    // ensure that only one thread will register the projections
    static std::once_flag first;
    std::call_once(first, []() {
      // register the operators that will be used by the decorator
      Operators4MultiDimInitialize< GUM_ELEMENT > op;
      op.init();

      // register the projectors that will be used by the decorator
      Projections4MultiDimInitialize< GUM_ELEMENT > proj;
      proj.init();

      // register the projectors that will be used by the decorator
      CompleteProjections4MultiDimInitialize< GUM_ELEMENT > comp_proj;
      comp_proj.init();

      // register the partial instantiators that will be used by the decorator
      PartialInstantiation4MultiDimInitialize< GUM_ELEMENT > inst;
      inst.init();
    });
  }

  // constructors
  template < typename GUM_ELEMENT >
  MultiDimDecorator< GUM_ELEMENT >::MultiDimDecorator(
      MultiDimImplementation< GUM_ELEMENT >* aContent,
      GUM_ELEMENT empty_value) : content_(aContent), empty_value_(empty_value) {
    _initTensorOperators__< GUM_ELEMENT >();
    GUM_CONSTRUCTOR(MultiDimDecorator);
  }

  template < typename GUM_ELEMENT >
  MultiDimDecorator< GUM_ELEMENT >::MultiDimDecorator(
      const MultiDimDecorator< GUM_ELEMENT >& from) : MultiDimContainer< GUM_ELEMENT >(from) {
    GUM_CONS_CPY(MultiDimDecorator);
    _initTensorOperators__< GUM_ELEMENT >();
    empty_value_ = from.empty_value_;
    content()->copy(from.content());
  }

  template < typename GUM_ELEMENT >
  MultiDimDecorator< GUM_ELEMENT >&
      MultiDimDecorator< GUM_ELEMENT >::operator=(MultiDimDecorator< GUM_ELEMENT >&& from) {
    MultiDimContainer< GUM_ELEMENT >::operator=(
        std::forward< MultiDimContainer< GUM_ELEMENT > >(from));
    GUM_OP_MOV(MultiDimDecorator);

    if (this != &from) {
      if (content_ != nullptr) delete (content_);   // should be the case
      empty_value_  = from.empty_value_;
      content_      = from.content_;
      from.content_ = nullptr;
    }

    return *this;
  }

  template < typename GUM_ELEMENT >
  MultiDimDecorator< GUM_ELEMENT >::MultiDimDecorator(
      MultiDimDecorator< GUM_ELEMENT >&& from) noexcept :
      MultiDimContainer< GUM_ELEMENT >(std::forward< MultiDimContainer< GUM_ELEMENT > >(from)) {
    GUM_CONS_MOV(MultiDimDecorator);

    empty_value_  = from.empty_value_;
    content_      = from.content_;
    from.content_ = nullptr;
  }

  template < typename GUM_ELEMENT >
  MultiDimDecorator< GUM_ELEMENT >& MultiDimDecorator< GUM_ELEMENT >::operator=(
      const MultiDimDecorator< GUM_ELEMENT >& from) noexcept {
    GUM_OP_CPY(MultiDimDecorator);
    _initTensorOperators__< GUM_ELEMENT >();
    MultiDimContainer< GUM_ELEMENT >::operator=(from);
    empty_value_ = from.empty_value_;
    if (content_ == nullptr)
      content_
          = static_cast< MultiDimImplementation< GUM_ELEMENT >* >(from.content()->newFactory());
    MultiDimDecorator< GUM_ELEMENT >::content()->copy(*from.content());
    return *this;
  }

  // destructor

  template < typename GUM_ELEMENT >
  MultiDimDecorator< GUM_ELEMENT >::~MultiDimDecorator() {
    if (content_ != nullptr) { delete (content_); }

    GUM_DESTRUCTOR(MultiDimDecorator);
  }

  // return a data, given a Instantiation - final method

  template < typename GUM_ELEMENT >
  GUM_ELEMENT& MultiDimDecorator< GUM_ELEMENT >::get_(const Instantiation& i) const {
    GUM_ERROR(OperationNotAllowed, "_get in the implementation !")
  }

  template < typename GUM_ELEMENT >
  GUM_ELEMENT MultiDimDecorator< GUM_ELEMENT >::get(const Instantiation& i) const {
    if (static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->empty()) {
      return empty_value_;
    } else {
      return static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->get(i);
    }
  }

  template < typename GUM_ELEMENT >
  void MultiDimDecorator< GUM_ELEMENT >::set(const Instantiation& i,
                                             const GUM_ELEMENT&   value) const {
    if (static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->nbrDim() == 0) {
      empty_value_ = value;
    } else {
      static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->set(i, value);
    }
  }

  // get the size of domains - final method

  template < typename GUM_ELEMENT >
  Size MultiDimDecorator< GUM_ELEMENT >::domainSize() const {
    return static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->domainSize();
  }

  // add a new var to the sequence of vars - final method

  template < typename GUM_ELEMENT >
  void MultiDimDecorator< GUM_ELEMENT >::add(const DiscreteVariable& v) {
    if (v.domainSize() < 1) {
      GUM_ERROR(InvalidArgument, "Empty variable " << v << " cannot be added in a Tensor")
    }
    static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->add(v);
  }

  // listen to change in each recorded Instantiation. final method

  template < typename GUM_ELEMENT >
  void MultiDimDecorator< GUM_ELEMENT >::changeNotification(const Instantiation&          i,
                                                            const DiscreteVariable* const var,
                                                            Idx                           oldval,
                                                            Idx                           newval) {
    static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->changeNotification(i,
                                                                                   var,
                                                                                   oldval,
                                                                                   newval);
  }

  // listen to an assignment of a value in a Instantiation

  template < typename GUM_ELEMENT >
  void MultiDimDecorator< GUM_ELEMENT >::setChangeNotification(const Instantiation& i) {
    static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->setChangeNotification(i);
  }

  // listen to setFirst in each recorded Instantiation. final method.

  template < typename GUM_ELEMENT >
  void MultiDimDecorator< GUM_ELEMENT >::setFirstNotification(const Instantiation& i) {
    static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->setFirstNotification(i);
  }

  // listen to setLast in each recorded Instantiation. final method.

  template < typename GUM_ELEMENT >
  void MultiDimDecorator< GUM_ELEMENT >::setLastNotification(const Instantiation& i) {
    static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->setLastNotification(i);
  }

  // listen to increment in each recorded Instantiation. final method.

  template < typename GUM_ELEMENT >
  void MultiDimDecorator< GUM_ELEMENT >::setIncNotification(const Instantiation& i) {
    static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->setIncNotification(i);
  }

  // listen to increment in each recorded Instantiation. final method.

  template < typename GUM_ELEMENT >
  void MultiDimDecorator< GUM_ELEMENT >::setDecNotification(const Instantiation& i) {
    static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->setDecNotification(i);
  }

  // add a Instantiation as a slave of this

  template < typename GUM_ELEMENT >
  bool MultiDimDecorator< GUM_ELEMENT >::registerSlave(Instantiation& i) {
    return static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->registerSlave(i);
  }

  template < typename GUM_ELEMENT >
  void MultiDimDecorator< GUM_ELEMENT >::erase(const DiscreteVariable& var) {
    static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->erase(var);
  }

  template < typename GUM_ELEMENT >
  void MultiDimDecorator< GUM_ELEMENT >::erase(std::string_view name) {
    erase(variable(name));
  }

  template < typename GUM_ELEMENT >
  const DiscreteVariable& MultiDimDecorator< GUM_ELEMENT >::variable(Idx i) const {
    return static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->variable(i);
  }

  template < typename GUM_ELEMENT >
  const DiscreteVariable& MultiDimDecorator< GUM_ELEMENT >::variable(std::string_view name) const {
    return static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->variable(name);
  }

  template < typename GUM_ELEMENT >
  Idx MultiDimDecorator< GUM_ELEMENT >::pos(const DiscreteVariable& var) const {
    return static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->pos(var);
  }

  template < typename GUM_ELEMENT >
  bool MultiDimDecorator< GUM_ELEMENT >::contains(const DiscreteVariable& var) const {
    return static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->contains(var);
  }

  template < typename GUM_ELEMENT >
  bool MultiDimDecorator< GUM_ELEMENT >::empty() const {
    if (content_ == nullptr) return true;
    return static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->empty();
  }

  template < typename GUM_ELEMENT >
  bool MultiDimDecorator< GUM_ELEMENT >::unregisterSlave(Instantiation& i) {
    return static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->unregisterSlave(i);
  }

  template < typename GUM_ELEMENT >
  void MultiDimDecorator< GUM_ELEMENT >::fill(const GUM_ELEMENT& d) const {
    if (static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->empty()) {
      empty_value_ = d;
    } else {
      static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->fill(d);
    }
  }

  // notification modification on vars to all Instantiation listeners.

  template < typename GUM_ELEMENT >
  void MultiDimDecorator< GUM_ELEMENT >::notifyChange() const {
    /*( (MultiDimContainer<GUM_ELEMENT> *) content_)->notifyChange();*/
    GUM_ERROR(OperationNotAllowed, "Not implemented yet")
  }

  // give a const ref to the sequence of DiscreteVariable*. final method.

  template < typename GUM_ELEMENT >
  const Sequence< const DiscreteVariable* >&
      MultiDimDecorator< GUM_ELEMENT >::variablesSequence() const {
    return static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->variablesSequence();
  }

  // get the nbr of vars in the sequence. final method

  template < typename GUM_ELEMENT >
  Idx MultiDimDecorator< GUM_ELEMENT >::nbrDim() const {
    return static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->nbrDim();
  }

  template < typename GUM_ELEMENT >
  void MultiDimDecorator< GUM_ELEMENT >::populate(const std::vector< GUM_ELEMENT >& v) const {
    if (static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->empty()) {
      if (v.size() == 1) {
        empty_value_ = v[0];
      } else {
        GUM_ERROR(SizeError, "Size do not match in populate")
      }
    } else {
      content_->populate(v);
    }
  }

  template < typename GUM_ELEMENT >
  void MultiDimDecorator< GUM_ELEMENT >::apply(std::function< GUM_ELEMENT(GUM_ELEMENT) > f) const {
    if (static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->empty()) {
      empty_value_ = f(empty_value_);
    } else {
      content_->apply(f);
    }
  }

  template < typename GUM_ELEMENT >
  GUM_ELEMENT MultiDimDecorator< GUM_ELEMENT >::reduce(
      std::function< GUM_ELEMENT(GUM_ELEMENT, GUM_ELEMENT) > f,
      GUM_ELEMENT                                            base) const {
    if (static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->empty()) {
      return base;
    } else {
      return content_->reduce(f, base);
    }
  }

  // protected access to content_
  template < typename GUM_ELEMENT >
  MultiDimImplementation< GUM_ELEMENT >* MultiDimDecorator< GUM_ELEMENT >::content() {
    return content_;
  }

  // protected access to content_
  template < typename GUM_ELEMENT >
  const MultiDimImplementation< GUM_ELEMENT >* MultiDimDecorator< GUM_ELEMENT >::content() const {
    return content_;
  }

  template < typename GUM_ELEMENT >
  void MultiDimDecorator< GUM_ELEMENT >::beginMultipleChanges() {
    static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->beginMultipleChanges();
  }

  template < typename GUM_ELEMENT >
  void MultiDimDecorator< GUM_ELEMENT >::endMultipleChanges() {
    static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->endMultipleChanges();
  }

  template < typename GUM_ELEMENT >
  void MultiDimDecorator< GUM_ELEMENT >::endMultipleChanges(const GUM_ELEMENT& x) {
    static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->endMultipleChanges(x);
  }

  template < typename GUM_ELEMENT >
  void MultiDimDecorator< GUM_ELEMENT >::swapContent_(
      MultiDimImplementation< GUM_ELEMENT >* aContent) const {
    if (aContent != nullptr) {
      // TODO : frees all slave instantiations
      // TODO : control the dimensions ?
      MultiDimImplementation< GUM_ELEMENT >* tmp = content_;
      content_                                   = aContent;
      // registers all instantiations
      delete (tmp);
    }
  }

  // string representation of internal data about i in this.
  template < typename GUM_ELEMENT >
  std::string MultiDimDecorator< GUM_ELEMENT >::toString(const Instantiation* i) const {
    return content_->toString(i);
  }

  template < typename GUM_ELEMENT >
  void MultiDimDecorator< GUM_ELEMENT >::replace_(const DiscreteVariable* x,
                                                  const DiscreteVariable* y) {
    this->content()->replace(*x, *y);
  }

  template < typename GUM_ELEMENT >
  std::string MultiDimDecorator< GUM_ELEMENT >::toString() const {
    if (static_cast< MultiDimContainer< GUM_ELEMENT >* >(content_)->empty()) {
      std::stringstream ss;
      ss << "<> :: " << empty_value_;
      return ss.str();
    } else {
      return content_->toString();
    }
  }

  //@todo force GUM_ELEMENT to be double-castable (to be able to use fabs,etc.)
} /* namespace gum */
