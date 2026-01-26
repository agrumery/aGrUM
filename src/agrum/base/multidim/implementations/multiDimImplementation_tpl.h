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
 * @brief Implementation of gum::MultiDimImplementation.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

// to ease IDE parser
#include <agrum/base/multidim/implementations/multiDimImplementation.h>

namespace gum {

  // Default constructor

  template < typename GUM_ELEMENT >
  INLINE MultiDimImplementation< GUM_ELEMENT >::MultiDimImplementation() :
      MultiDimContainer< GUM_ELEMENT >(), _vars_(), _slaveInstantiations_() {
    GUM_CONSTRUCTOR(MultiDimImplementation);
    _internalChangeMethod_ = _InternalChangeMethod_::DIRECT_CHANGE;
    _internalChangeState_  = _InternalChangeState_::NO_CHANGE;
    _domainSize_           = 1;
  }

  // Copy constructor

  template < typename GUM_ELEMENT >
  INLINE MultiDimImplementation< GUM_ELEMENT >::MultiDimImplementation(
      const MultiDimImplementation< GUM_ELEMENT >& from) :
      MultiDimContainer< GUM_ELEMENT >(from), _vars_(from._vars_),
      _internalChangeMethod_(from._internalChangeMethod_),
      _internalChangeState_(from._internalChangeState_), _domainSize_(from._domainSize_) {
    GUM_CONS_CPY(MultiDimImplementation);
    GUM_ASSERT(!this->isCommitNeeded_());
  }

  // destructor

  template < typename GUM_ELEMENT >
  INLINE MultiDimImplementation< GUM_ELEMENT >::~MultiDimImplementation() {
    GUM_DESTRUCTOR(MultiDimImplementation);
    // unregister all remaining slave instantiations

    for (List< Instantiation* >::iterator_safe iter = _slaveInstantiations_.beginSafe();
         iter != _slaveInstantiations_.endSafe();
         ++iter)
      (*iter)->forgetMaster();
  }

  // add a new var to the sequence of  _vars_.

  template < typename GUM_ELEMENT >
  INLINE void MultiDimImplementation< GUM_ELEMENT >::add(const DiscreteVariable& v) {
    // check if the variable already belongs to the tuple of variables
    // of the Instantiation
    if (_vars_.exists(&v)) {
      GUM_ERROR(DuplicateElement, "Var " << v.name() << " already exists in this instantiation")
    }
    for (const auto& w: _vars_) {
      if (w->name() == v.name())
        GUM_ERROR(DuplicateElement,
                  "A var with name '" << v.name() << "' already exists in this instantiation");
    }

    _domainSize_ *= v.domainSize();

    _vars_.insert(&v);

    // informs all the slaves that they have to update themselves
    for (List< Instantiation* >::iterator_safe iter = _slaveInstantiations_.beginSafe();
         iter != _slaveInstantiations_.endSafe();
         ++iter) {
      (*iter)->addWithMaster(this, v);
    }

    if (isInMultipleChangeMethod_()) _setNotCommitedChange_();
  }

  // removes a var from the variables of the multidimensional matrix

  template < typename GUM_ELEMENT >
  INLINE void MultiDimImplementation< GUM_ELEMENT >::erase(const DiscreteVariable& v) {
    // check that the variable does actually belong to the
    // MultiDimImplementation
    if (!_vars_.exists(&v)) { GUM_ERROR(NotFound, "Var does not exist in this implementation") }

    _domainSize_ /= v.domainSize();

    _vars_.erase(&v);

    // informs all the slaves that they have to update themselves
    for (List< Instantiation* >::iterator_safe iter = _slaveInstantiations_.beginSafe();
         iter != _slaveInstantiations_.endSafe();
         ++iter) {
      (*iter)->eraseWithMaster(this, v);
    }

    if (isInMultipleChangeMethod_()) _setNotCommitedChange_();
  }

  // adds a new var to the sequence of  _vars_
  template < typename GUM_ELEMENT >
  INLINE MultiDimImplementation< GUM_ELEMENT >&
         operator<<(MultiDimImplementation< GUM_ELEMENT >& array, const DiscreteVariable& v) {
    array.add(v);
    return array;
  }

  // add a Instantiation to the list of slave instantiations

  template < typename GUM_ELEMENT >
  INLINE bool MultiDimImplementation< GUM_ELEMENT >::registerSlave(Instantiation& slave) {
    // check that the Instantiation has the same variables as this
    if (slave.nbrDim() != _vars_.size()) return false;

    for (Sequence< const DiscreteVariable* >::iterator_safe iter = _vars_.beginSafe();
         iter != _vars_.endSafe();
         ++iter)
      if (!slave.contains(*iter)) return false;

    slave.synchronizeWithMaster(this);

    _slaveInstantiations_ += (&slave);

    return true;
  }

  // removes a Instantiation from the list of slave instantiations

  template < typename GUM_ELEMENT >
  INLINE bool MultiDimImplementation< GUM_ELEMENT >::unregisterSlave(Instantiation& slave) {
    _slaveInstantiations_.eraseByVal(&slave);
    return true;
  }

  template < typename GUM_ELEMENT >
  INLINE Idx MultiDimImplementation< GUM_ELEMENT >::nbrDim() const {
    return _vars_.size();
  }

  template < typename GUM_ELEMENT >
  INLINE Size MultiDimImplementation< GUM_ELEMENT >::domainSize() const {
    return _domainSize_;
  }

  template < typename GUM_ELEMENT >
  INLINE const DiscreteVariable& MultiDimImplementation< GUM_ELEMENT >::variable(Idx i) const {
    return *(_vars_.atPos(i));
  }

  template < typename GUM_ELEMENT >
  INLINE const DiscreteVariable&
      MultiDimImplementation< GUM_ELEMENT >::variable(const std::string& name) const {
    for (const auto& v: _vars_) {
      if (v->name() == name) return *v;
    }

    GUM_ERROR(NotFound, "'" << name << "' can not be found in the multidim structure.")
  }

  template < typename GUM_ELEMENT >
  INLINE Idx MultiDimImplementation< GUM_ELEMENT >::pos(const DiscreteVariable& v) const {
    return _vars_.pos(&v);
  }

  template < typename GUM_ELEMENT >
  INLINE bool MultiDimImplementation< GUM_ELEMENT >::contains(const DiscreteVariable& v) const {
    return _vars_.exists(&v);
  }

  // returns a const ref to the sequence of DiscreteVariable*

  template < typename GUM_ELEMENT >
  INLINE const Sequence< const DiscreteVariable* >&
               MultiDimImplementation< GUM_ELEMENT >::variablesSequence() const {
    return _vars_;
  }

  // is this empty ?
  template < typename GUM_ELEMENT >
  INLINE bool MultiDimImplementation< GUM_ELEMENT >::empty() const {
    GUM_ASSERT(!this->isCommitNeeded_());
    return _vars_.empty();
  }

  template < typename GUM_ELEMENT >
  INLINE void MultiDimImplementation< GUM_ELEMENT >::beginMultipleChanges() {
    _internalChangeMethod_ = _InternalChangeMethod_::MULTIPLE_CHANGE;
  }

  template < typename GUM_ELEMENT >
  INLINE void MultiDimImplementation< GUM_ELEMENT >::endMultipleChanges() {
    if (_internalChangeState_ == _InternalChangeState_::NOT_COMMITTED_CHANGE) {
      commitMultipleChanges_();
      _internalChangeState_ = _InternalChangeState_::NO_CHANGE;
    }

    _internalChangeMethod_ = _InternalChangeMethod_::DIRECT_CHANGE;
  }

  template < typename GUM_ELEMENT >
  INLINE void MultiDimImplementation< GUM_ELEMENT >::endMultipleChanges(const GUM_ELEMENT& x) {
    if (_internalChangeState_ == _InternalChangeState_::NOT_COMMITTED_CHANGE) {
      commitMultipleChanges_(x);
      _internalChangeState_ = _InternalChangeState_::NO_CHANGE;
    }

    _internalChangeMethod_ = _InternalChangeMethod_::DIRECT_CHANGE;
  }

  template < typename GUM_ELEMENT >
  INLINE void MultiDimImplementation< GUM_ELEMENT >::commitMultipleChanges_() {
    // empty!
  }

  template < typename GUM_ELEMENT >
  INLINE void MultiDimImplementation< GUM_ELEMENT >::commitMultipleChanges_(const GUM_ELEMENT&) {
    // empty!
  }

  // get the actual change method of *this
  template < typename GUM_ELEMENT >
  INLINE bool MultiDimImplementation< GUM_ELEMENT >::isInMultipleChangeMethod_() const {
    return (_internalChangeMethod_ == _InternalChangeMethod_::MULTIPLE_CHANGE);
  }

  // get the actual state of *this
  template < typename GUM_ELEMENT >
  INLINE bool MultiDimImplementation< GUM_ELEMENT >::isCommitNeeded_() const {
    return (_internalChangeState_ == _InternalChangeState_::NOT_COMMITTED_CHANGE);
  }

  // Returns a constant reference over the list of slaved instantiations.
  template < typename GUM_ELEMENT >
  INLINE const List< Instantiation* >& MultiDimImplementation< GUM_ELEMENT >::slaves_() const {
    return _slaveInstantiations_;
  }

  // get the actual state of *this
  template < typename GUM_ELEMENT >
  INLINE void MultiDimImplementation< GUM_ELEMENT >::_setNotCommitedChange_() {
    _internalChangeState_ = _InternalChangeState_::NOT_COMMITTED_CHANGE;
  }

  // get the actual state of *this
  template < typename GUM_ELEMENT >
  INLINE float MultiDimImplementation< GUM_ELEMENT >::compressionRate() const {
    return ((float)1) - (float)realSize() / (float)domainSize();
  }

  // returns a basename to be used for default operators
  template < typename GUM_ELEMENT >
  const std::string& MultiDimImplementation< GUM_ELEMENT >::basename() const {
    static const std::string str = "MultiDimImplementation";
    return str;
  }

  template < typename GUM_ELEMENT >
  INLINE void MultiDimImplementation< GUM_ELEMENT >::replace_(const DiscreteVariable* x,
                                                              const DiscreteVariable* y) {
    _vars_.setAtPos(_vars_.pos(x), y);

    for (List< Instantiation* >::iterator_safe iter = _slaveInstantiations_.beginSafe();
         iter != _slaveInstantiations_.endSafe();
         ++iter) {
      (**iter).replace(*x, *y);
    }
  }

  template < typename GUM_ELEMENT >
  INLINE void MultiDimImplementation< GUM_ELEMENT >::invert_(Idx p1, Idx p2) {
    _vars_.swap(p1, p2);
  }

  // for friendly displaying the content of the array
  template < typename GUM_ELEMENT >
  INLINE std::ostream& operator<<(std::ostream&                                out,
                                  const MultiDimImplementation< GUM_ELEMENT >& array) {
    return out << static_cast< const MultiDimContainer< GUM_ELEMENT >& >(array);
  }

  // protected access to content_
  template < typename GUM_ELEMENT >
  INLINE MultiDimImplementation< GUM_ELEMENT >* MultiDimImplementation< GUM_ELEMENT >::content() {
    return this;
  }

  // protected access to content_
  template < typename GUM_ELEMENT >
  INLINE const MultiDimImplementation< GUM_ELEMENT >*
               MultiDimImplementation< GUM_ELEMENT >::content() const {
    return this;
  }
} /* namespace gum */
