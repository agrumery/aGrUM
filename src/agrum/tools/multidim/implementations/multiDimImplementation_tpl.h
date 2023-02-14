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
 * @brief Implementation of gum::MultiDimImplementation.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

// to ease IDE parser
#include <agrum/tools/multidim/implementations/multiDimImplementation.h>

namespace gum {

  // Default constructor

  template < typename GUM_SCALAR >
  INLINE MultiDimImplementation< GUM_SCALAR >::MultiDimImplementation() :
      MultiDimContainer< GUM_SCALAR >(), _vars_(), _slaveInstantiations_() {
    GUM_CONSTRUCTOR(MultiDimImplementation);
    _internalChangeMethod_ = _InternalChangeMethod_::DIRECT_CHANGE;
    _internalChangeState_  = _InternalChangeState_::NO_CHANGE;
    _domainSize_           = 1;
  }

  // Copy constructor

  template < typename GUM_SCALAR >
  INLINE MultiDimImplementation< GUM_SCALAR >::MultiDimImplementation(
     const MultiDimImplementation< GUM_SCALAR >& from) :
      MultiDimContainer< GUM_SCALAR >(from),
      _vars_(from._vars_), _internalChangeMethod_(from._internalChangeMethod_),
      _internalChangeState_(from._internalChangeState_), _domainSize_(from._domainSize_) {
    GUM_CONS_CPY(MultiDimImplementation);
    GUM_ASSERT(!this->isCommitNeeded_());
  }

  // destructor

  template < typename GUM_SCALAR >
  INLINE MultiDimImplementation< GUM_SCALAR >::~MultiDimImplementation() {
    GUM_DESTRUCTOR(MultiDimImplementation);
    // unregister all remaining slave instantiations

    for (List< Instantiation* >::iterator_safe iter = _slaveInstantiations_.beginSafe();
         iter != _slaveInstantiations_.endSafe();
         ++iter)
      (*iter)->forgetMaster();
  }

  // add a new var to the sequence of  _vars_.

  template < typename GUM_SCALAR >
  INLINE void MultiDimImplementation< GUM_SCALAR >::add(const DiscreteVariable& v) {
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

  template < typename GUM_SCALAR >
  INLINE void MultiDimImplementation< GUM_SCALAR >::erase(const DiscreteVariable& v) {
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
  template < typename GUM_SCALAR >
  INLINE MultiDimImplementation< GUM_SCALAR >&
         operator<<(MultiDimImplementation< GUM_SCALAR >& array, const DiscreteVariable& v) {
    array.add(v);
    return array;
  }

  // add a Instantiation to the list of slave instantiations

  template < typename GUM_SCALAR >
  INLINE bool MultiDimImplementation< GUM_SCALAR >::registerSlave(Instantiation& slave) {
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

  template < typename GUM_SCALAR >
  INLINE bool MultiDimImplementation< GUM_SCALAR >::unregisterSlave(Instantiation& slave) {
    _slaveInstantiations_.eraseByVal(&slave);
    return true;
  }

  template < typename GUM_SCALAR >
  INLINE Idx MultiDimImplementation< GUM_SCALAR >::nbrDim() const {
    return _vars_.size();
  }

  template < typename GUM_SCALAR >
  INLINE Size MultiDimImplementation< GUM_SCALAR >::domainSize() const {
    return _domainSize_;
  }

  template < typename GUM_SCALAR >
  INLINE const DiscreteVariable& MultiDimImplementation< GUM_SCALAR >::variable(Idx i) const {
    return *(_vars_.atPos(i));
  }

  template < typename GUM_SCALAR >
  INLINE const DiscreteVariable&
     MultiDimImplementation< GUM_SCALAR >::variable(const std::string& name) const {
    for (const auto& v: _vars_) {
      if (v->name() == name) return *v;
    }

    GUM_ERROR(NotFound, "'" << name << "' can not be found in the multidim structure.")
  }

  template < typename GUM_SCALAR >
  INLINE Idx MultiDimImplementation< GUM_SCALAR >::pos(const DiscreteVariable& v) const {
    return _vars_.pos(&v);
  }

  template < typename GUM_SCALAR >
  INLINE bool MultiDimImplementation< GUM_SCALAR >::contains(const DiscreteVariable& v) const {
    return _vars_.exists(&v);
  }

  // returns a const ref to the sequence of DiscreteVariable*

  template < typename GUM_SCALAR >
  INLINE const Sequence< const DiscreteVariable* >&
               MultiDimImplementation< GUM_SCALAR >::variablesSequence() const {
    return _vars_;
  }

  // is this empty ?
  template < typename GUM_SCALAR >
  INLINE bool MultiDimImplementation< GUM_SCALAR >::empty() const {
    GUM_ASSERT(!this->isCommitNeeded_());
    return _vars_.empty();
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimImplementation< GUM_SCALAR >::beginMultipleChanges() {
    _internalChangeMethod_ = _InternalChangeMethod_::MULTIPLE_CHANGE;
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimImplementation< GUM_SCALAR >::endMultipleChanges() {
    if (_internalChangeState_ == _InternalChangeState_::NOT_COMMITTED_CHANGE) {
      commitMultipleChanges_();
      _internalChangeState_ = _InternalChangeState_::NO_CHANGE;
    }

    _internalChangeMethod_ = _InternalChangeMethod_::DIRECT_CHANGE;
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimImplementation< GUM_SCALAR >::endMultipleChanges(const GUM_SCALAR& x) {
    if (_internalChangeState_ == _InternalChangeState_::NOT_COMMITTED_CHANGE) {
      commitMultipleChanges_(x);
      _internalChangeState_ = _InternalChangeState_::NO_CHANGE;
    }

    _internalChangeMethod_ = _InternalChangeMethod_::DIRECT_CHANGE;
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimImplementation< GUM_SCALAR >::commitMultipleChanges_() {
    // empty!
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimImplementation< GUM_SCALAR >::commitMultipleChanges_(const GUM_SCALAR&) {
    // empty!
  }

  // get the actual change method of *this
  template < typename GUM_SCALAR >
  INLINE bool MultiDimImplementation< GUM_SCALAR >::isInMultipleChangeMethod_() const {
    return (_internalChangeMethod_ == _InternalChangeMethod_::MULTIPLE_CHANGE);
  }

  // get the actual state of *this
  template < typename GUM_SCALAR >
  INLINE bool MultiDimImplementation< GUM_SCALAR >::isCommitNeeded_() const {
    return (_internalChangeState_ == _InternalChangeState_::NOT_COMMITTED_CHANGE);
  }

  // Returns a constant reference over the list of slaved instantiations.
  template < typename GUM_SCALAR >
  INLINE const List< Instantiation* >& MultiDimImplementation< GUM_SCALAR >::slaves_() const {
    return _slaveInstantiations_;
  }

  // get the actual state of *this
  template < typename GUM_SCALAR >
  INLINE void MultiDimImplementation< GUM_SCALAR >::_setNotCommitedChange_() {
    _internalChangeState_ = _InternalChangeState_::NOT_COMMITTED_CHANGE;
  }

  // get the actual state of *this
  template < typename GUM_SCALAR >
  INLINE float MultiDimImplementation< GUM_SCALAR >::compressionRate() const {
    return ((float)1) - (float)realSize() / (float)domainSize();
  }

  // returns a basename to be used for default operators
  template < typename GUM_SCALAR >
  const std::string& MultiDimImplementation< GUM_SCALAR >::basename() const {
    static const std::string str = "MultiDimImplementation";
    return str;
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimImplementation< GUM_SCALAR >::replace_(const DiscreteVariable* x,
                                                             const DiscreteVariable* y) {
    _vars_.setAtPos(_vars_.pos(x), y);

    for (List< Instantiation* >::iterator_safe iter = _slaveInstantiations_.beginSafe();
         iter != _slaveInstantiations_.endSafe();
         ++iter) {
      (**iter).replace(*x, *y);
    }
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimImplementation< GUM_SCALAR >::invert_(Idx p1, Idx p2) {
    _vars_.swap(p1, p2);
  }

  // for friendly displaying the content of the array
  template < typename GUM_SCALAR >
  INLINE std::ostream& operator<<(std::ostream&                               out,
                                  const MultiDimImplementation< GUM_SCALAR >& array) {
    return out << static_cast< const MultiDimContainer< GUM_SCALAR >& >(array);
  }


  // protected access to content_
  template < typename GUM_SCALAR >
  INLINE MultiDimImplementation< GUM_SCALAR >* MultiDimImplementation< GUM_SCALAR >::content() {
    return this;
  }

  // protected access to content_
  template < typename GUM_SCALAR >
  INLINE const MultiDimImplementation< GUM_SCALAR >*
               MultiDimImplementation< GUM_SCALAR >::content() const {
    return this;
  }
} /* namespace gum */
