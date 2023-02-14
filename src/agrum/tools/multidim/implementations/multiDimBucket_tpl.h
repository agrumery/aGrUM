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
 * @brief Implementation of the MultiDimBucket class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 * @author Lionel TORTI
 */

#include <agrum/tools/multidim/implementations/multiDimBucket.h>

namespace gum {

  template < typename GUM_SCALAR >
  MultiDimBucket< GUM_SCALAR >::MultiDimBucket(Size bufferSize) :
      MultiDimReadOnly< GUM_SCALAR >(), _bufferSize_(bufferSize), _bucket_(0), _changed_(false),
      _name_("MultiDimBucket") {
    GUM_CONSTRUCTOR(MultiDimBucket);
  }

  template < typename GUM_SCALAR >
  MultiDimBucket< GUM_SCALAR >::MultiDimBucket(const MultiDimBucket< GUM_SCALAR >& source) :
      MultiDimReadOnly< GUM_SCALAR >(source), _bufferSize_(source._bufferSize_), _bucket_(0),
      _multiDims_(source._multiDims_), _allVariables_(source._allVariables_),
      _allVarsInst_(source._allVarsInst_), _changed_(source._changed_), _name_("MultiDimBucket") {
    GUM_CONS_CPY(MultiDimBucket);
  }

  template < typename GUM_SCALAR >
  MultiDimBucket< GUM_SCALAR >::~MultiDimBucket() {
    GUM_DESTRUCTOR(MultiDimBucket);
    using BiIter = Bijection< Instantiation*, Instantiation* >::iterator_safe;

    for (BiIter iter = _instantiations_.beginSafe(); iter != _instantiations_.endSafe(); ++iter) {
      delete iter.second();
    }

    if (_bucket_) { delete _bucket_; }

    for (HashTableIteratorSafe< const MultiDimContainer< GUM_SCALAR >*, Instantiation* > iter
         = _multiDims_.beginSafe();
         iter != _multiDims_.endSafe();
         ++iter) {
      delete iter.val();
    }
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimBucket< GUM_SCALAR >::add(const MultiDimContainer< GUM_SCALAR >& impl) {
    this->add(&impl);
  }

  template < typename GUM_SCALAR >
  void MultiDimBucket< GUM_SCALAR >::add(const MultiDimContainer< GUM_SCALAR >* impl) {
    _multiDims_.insert(impl, new Instantiation(*impl));

    if (!MultiDimImplementation< GUM_SCALAR >::isInMultipleChangeMethod_()) {
      for (const auto var: impl->variablesSequence()) {
        _addVariable_(var);
      }
    }

    _changed_ = true;
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimBucket< GUM_SCALAR >::erase(const MultiDimContainer< GUM_SCALAR >& impl) {
    this->erase(&impl);
  }

  template < typename GUM_SCALAR >
  void MultiDimBucket< GUM_SCALAR >::erase(const MultiDimContainer< GUM_SCALAR >* impl) {
    try {
      delete _multiDims_[impl];
      _multiDims_.erase(impl);

      if (!MultiDimImplementation< GUM_SCALAR >::isInMultipleChangeMethod_()) {
        for (auto var: impl->variablesSequence()) {
          _eraseVariable_(var);
        }
      }

      _changed_ = true;
    } catch (NotFound const&) {
      // Do nothing
    }
  }

  template < typename GUM_SCALAR >
  INLINE bool
     MultiDimBucket< GUM_SCALAR >::contains(const MultiDimContainer< GUM_SCALAR >& impl) const {
    return _multiDims_.exists(&impl);
  }

  template < typename GUM_SCALAR >
  INLINE const Set< const DiscreteVariable* >& MultiDimBucket< GUM_SCALAR >::allVariables() const {
    return _allVariables_;
  }

  template < typename GUM_SCALAR >
  INLINE Size MultiDimBucket< GUM_SCALAR >::bucketSize() const {
    return _multiDims_.size();
  }

  template < typename GUM_SCALAR >
  INLINE bool MultiDimBucket< GUM_SCALAR >::isBucketEmpty() const {
    return _multiDims_.empty();
  }

  template < typename GUM_SCALAR >
  INLINE bool MultiDimBucket< GUM_SCALAR >::bucketChanged() const {
    return _changed_;
  }

  template < typename GUM_SCALAR >
  INLINE Size MultiDimBucket< GUM_SCALAR >::bufferSize() const {
    return _bufferSize_;
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimBucket< GUM_SCALAR >::setBufferSize(Size ammount) {
    _bufferSize_ = ammount;

    if ((this->domainSize() > _bufferSize_) && (_bucket_ != 0)) {
      _eraseBuffer_();
    } else if (_bucket_ == 0) {
      _initializeBuffer_();
    }
  }

  template < typename GUM_SCALAR >
  void MultiDimBucket< GUM_SCALAR >::compute(bool force) const {
    if ((_bucket_) && (_changed_ || force)) {
      Instantiation values(*_bucket_);

      for (values.setFirst(); !values.end(); values.inc()) {
        _bucket_->set(values, _computeValue_(values));
      }
    } else if ((_bucket_ == 0) && _changed_) {
      _slavesValue_.clear();
      _changed_ = false;
    }

    _changed_ = false;
  }

  template < typename GUM_SCALAR >
  const std::string& MultiDimBucket< GUM_SCALAR >::name() const {
    return _name_;
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimBucket< GUM_SCALAR >::add(const DiscreteVariable& v) {
    MultiDimImplementation< GUM_SCALAR >::add(v);

    if (!MultiDimImplementation< GUM_SCALAR >::isInMultipleChangeMethod_()) {
      if (this->domainSize() <= _bufferSize_) {
        if (_bucket_) _bucket_->add(v);
        else _initializeBuffer_();
      } else if (_bucket_) {
        _eraseBuffer_();
      }
    }
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimBucket< GUM_SCALAR >::erase(const DiscreteVariable& v) {
    MultiDimImplementation< GUM_SCALAR >::erase(v);

    if ((!MultiDimImplementation< GUM_SCALAR >::isInMultipleChangeMethod_())
        && (this->domainSize() <= _bufferSize_)) {
      if (_bucket_) {
        _bucket_->erase(v);
      } else {
        _initializeBuffer_();
      }
    }
  }

  template < typename GUM_SCALAR >
  INLINE Size MultiDimBucket< GUM_SCALAR >::realSize() const {
    return (_bucket_) ? _bucket_->realSize() : (Size)0;
  }

  template < typename GUM_SCALAR >
  INLINE bool MultiDimBucket< GUM_SCALAR >::contains(const DiscreteVariable& v) const {
    return MultiDimImplementation< GUM_SCALAR >::contains(v);
  }

  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR MultiDimBucket< GUM_SCALAR >::get(const Instantiation& i) const {
    compute();

    if (_bucket_) {
      try {
        return _bucket_->get(*(_instantiations_.second(const_cast< Instantiation* >(&i))));
      } catch (NotFound const&) { return _bucket_->get(i); }
    } else if (i.isMaster(this)) {
      if (!_slavesValue_.exists(&i)) { _slavesValue_.insert(&i, _computeValue_(i)); }

      return _slavesValue_[&i];
    } else {
      return _computeValue_(i);
    }
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimBucket< GUM_SCALAR >::changeNotification(const Instantiation&          i,
                                                               const DiscreteVariable* const var,
                                                               Idx                           oldval,
                                                               Idx newval) {
    if (_bucket_) {
      try {
        _bucket_->changeNotification(*(_instantiations_).second(const_cast< Instantiation* >(&i)),
                                     var,
                                     oldval,
                                     newval);
      } catch (NotFound const&) {
        // Then i is not a slave of this
      }
    } else {
      _slavesValue_.erase(&i);
    }
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimBucket< GUM_SCALAR >::setFirstNotification(const Instantiation& i) {
    if (_bucket_) {
      try {
        _bucket_->setFirstNotification(
           *(_instantiations_).second(const_cast< Instantiation* >(&i)));
      } catch (NotFound const&) {
        // Then i is not a slave of this
      }
    } else {
      _slavesValue_.erase(&i);
    }
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimBucket< GUM_SCALAR >::setLastNotification(const Instantiation& i) {
    if (_bucket_) {
      try {
        _bucket_->setLastNotification(*(_instantiations_).second(const_cast< Instantiation* >(&i)));
      } catch (NotFound const&) {
        // Then i is not a slave of this
      }
    } else {
      _slavesValue_.erase(&i);
    }
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimBucket< GUM_SCALAR >::setIncNotification(const Instantiation& i) {
    if (_bucket_) {
      try {
        _bucket_->setIncNotification(*(_instantiations_.second(const_cast< Instantiation* >(&i))));
      } catch (NotFound const&) {
        // Then i is not a slave of this
      }
    } else {
      _slavesValue_.erase(&i);
    }
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimBucket< GUM_SCALAR >::setDecNotification(const Instantiation& i) {
    if (_bucket_) {
      try {
        _bucket_->setDecNotification(*(_instantiations_.second(const_cast< Instantiation* >(&i))));
      } catch (NotFound const&) {
        // Then i is not a slave of this
      }
    } else {
      _slavesValue_.erase(&i);
    }
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimBucket< GUM_SCALAR >::setChangeNotification(const Instantiation& i) {
    if (_bucket_) {
      try {
        _bucket_->setChangeNotification(
           *(_instantiations_.second(const_cast< Instantiation* >(&i))));
      } catch (NotFound const&) {
        // Then i is not a slave of this
      }
    } else {
      _slavesValue_.erase(&i);
    }
  }

  template < typename GUM_SCALAR >
  INLINE bool MultiDimBucket< GUM_SCALAR >::registerSlave(Instantiation& i) {
    if (_bucket_) {
      try {
        _instantiations_.insert(&i, new Instantiation(*_bucket_));
      } catch (DuplicateElement const&) { return false; }
    }

    return MultiDimImplementation< GUM_SCALAR >::registerSlave(i);
  }

  template < typename GUM_SCALAR >
  INLINE bool MultiDimBucket< GUM_SCALAR >::unregisterSlave(Instantiation& i) {
    MultiDimReadOnly< GUM_SCALAR >::unregisterSlave(i);

    if (_bucket_) {
      try {
        delete _instantiations_.second(&i);
        _instantiations_.eraseFirst(&i);
        return true;
      } catch (NotFound const&) { return false; }
    } else {
      if (_slavesValue_.exists(&i)) {
        _slavesValue_.erase(&i);
        return true;
      } else {
        return false;
      }
    }
  }

  template < typename GUM_SCALAR >
  INLINE MultiDimAdressable& MultiDimBucket< GUM_SCALAR >::getMasterRef() {
    if (_bucket_) {
      return *_bucket_;
    } else {
      return *this;
    }
  }

  template < typename GUM_SCALAR >
  INLINE const MultiDimAdressable& MultiDimBucket< GUM_SCALAR >::getMasterRef() const {
    if (_bucket_) {
      return *_bucket_;
    } else {
      return *this;
    }
  }

  template < typename GUM_SCALAR >
  INLINE std::string MultiDimBucket< GUM_SCALAR >::toString(const Instantiation* i) const {
    std::stringstream sBuff;
    sBuff << (*i) << " = " << get(*i);
    return sBuff.str();
  }

  template < typename GUM_SCALAR >
  void MultiDimBucket< GUM_SCALAR >::commitMultipleChanges_() {
    MultiDimImplementation< GUM_SCALAR >::commitMultipleChanges_();

    if (this->domainSize() <= _bufferSize_) {
      _initializeBuffer_();
    } else {
      _eraseBuffer_();
    }

    _allVariables_.clear();

    while (!_allVarsInst_.empty()) {
      _allVarsInst_.erase(**(_allVarsInst_.variablesSequence().beginSafe()));
    }

    for (   // HashTableIteratorSafe<const MultiDimContainer<GUM_SCALAR>*,
            // Instantiation*>
       auto iter = _multiDims_.beginSafe(); iter != _multiDims_.endSafe(); ++iter) {
      for (auto var: iter.key()->variablesSequence()) {
        _addVariable_(var);
      }
    }

    _changed_ = true;
  }

  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR& MultiDimBucket< GUM_SCALAR >::get_(const Instantiation& i) const {
    GUM_ERROR(OperationNotAllowed, "a MultiDimBucket is a read only MultiDim")
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimBucket< GUM_SCALAR >::_addVariable_(const DiscreteVariable* var) {
    try {
      _allVariables_.insert(var);
      _allVarsInst_.add(*var);
    } catch (DuplicateElement const&) {
      // Nothing to do then!
    }
  }

  template < typename GUM_SCALAR >
  void MultiDimBucket< GUM_SCALAR >::_eraseVariable_(const DiscreteVariable* var) {
    bool found = false;

    for (HashTableIteratorSafe< const MultiDimContainer< GUM_SCALAR >*, Instantiation* > iter
         = _multiDims_.beginSafe();
         iter != _multiDims_.endSafe();
         ++iter) {
      if (iter.key()->contains(*var)) {
        found = true;
        break;
      }
    }

    // No one use it, we can safely remove it
    if (!found) {
      _allVariables_.erase(var);
      _allVarsInst_.erase(*var);
    }
  }

  template < typename GUM_SCALAR >
  void MultiDimBucket< GUM_SCALAR >::_initializeBuffer_() {
    if (_bucket_) {
      for (auto iter = _instantiations_.beginSafe(); iter != _instantiations_.endSafe(); ++iter) {
        delete iter.second();
      }

      _instantiations_.clear();
      delete _bucket_;
      _bucket_ = 0;
    }

    // Creating the table.
    _bucket_ = new MultiDimArray< GUM_SCALAR >();

    for (auto var: this->variablesSequence()) {
      _bucket_->add(*var);
    }

    if (!this->slaves_().empty()) {
      for (List< Instantiation* >::const_iterator_safe iter = this->slaves_().cbeginSafe();
           iter != this->slaves_().cendSafe();
           ++iter) {
        _instantiations_.insert(*iter, new Instantiation(*_bucket_));
      }
    }

    _changed_ = true;
  }

  template < typename GUM_SCALAR >
  void MultiDimBucket< GUM_SCALAR >::_eraseBuffer_() {
    if (_bucket_) {
      for (auto iter = _instantiations_.beginSafe(); iter != _instantiations_.endSafe(); ++iter) {
        delete iter.second();
      }

      _instantiations_.clear();
      delete _bucket_;
      _bucket_ = 0;
    }
  }

  template < typename GUM_SCALAR >
  GUM_SCALAR MultiDimBucket< GUM_SCALAR >::_computeValue_(const Instantiation& value) const {
    try {
      GUM_SCALAR sum = (GUM_SCALAR)0;
      GUM_SCALAR current;
      _allVarsInst_.setVals(value);

      for (_allVarsInst_.setFirstOut(value); !_allVarsInst_.end(); _allVarsInst_.incOut(value)) {
        current = (GUM_SCALAR)1;

        for (HashTableIteratorSafe< const MultiDimContainer< GUM_SCALAR >*, Instantiation* > iter
             = _multiDims_.beginSafe();
             iter != _multiDims_.endSafe();
             ++iter) {
          (iter.val())->setVals(_allVarsInst_);
          current *= iter.key()->get(*(iter.val()));
        }

        sum += current;
      }

      return sum;
    } catch (NotFound& e) {
      std::cerr << std::endl << e.errorContent() << std::endl;
      // This happens if the bucket is empty.
      GUM_ERROR(SizeError, "This MultiDimBucket is empty.")
    }
  }

  template < typename GUM_SCALAR >
  INLINE MultiDimContainer< GUM_SCALAR >* MultiDimBucket< GUM_SCALAR >::newFactory() const {
    return new MultiDimBucket< GUM_SCALAR >;
  }

  template < typename GUM_SCALAR >
  INLINE const MultiDimArray< GUM_SCALAR >& MultiDimBucket< GUM_SCALAR >::bucket() const {
    if (_bucket_) {
      return *_bucket_;
    } else {
      GUM_ERROR(OperationNotAllowed, "bucket not used.")
    }
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimBucket< GUM_SCALAR >::replace_(const DiscreteVariable* x,
                                                     const DiscreteVariable* y) {
    MultiDimImplementation< GUM_SCALAR >::replace_(x, y);
    for (auto iter = _instantiations_.beginSafe(); iter != _instantiations_.endSafe(); ++iter) {
      iter.first()->replace(*x, *y);
      iter.second()->replace(*x, *y);
    }

    if (_bucket_) _bucket_->replace(*x, *y);

    _allVariables_.erase(x);
    _allVariables_.insert(y);
    _allVarsInst_.replace(*x, *y);
  }

  template < typename GUM_SCALAR >
  INLINE const HashTable< const MultiDimContainer< GUM_SCALAR >*, Instantiation* >&
               MultiDimBucket< GUM_SCALAR >::multidims() const {
    return _multiDims_;
  }

} /* namespace gum */
