
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @author Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 * @author Lionel TORTI
 */

#include <agrum/tools/multidim/implementations/multiDimBucket.h>

namespace gum {

  template < typename GUM_SCALAR >
  MultiDimBucket< GUM_SCALAR >::MultiDimBucket(Size bufferSize) :
      MultiDimReadOnly< GUM_SCALAR >(), bufferSize__(bufferSize), bucket__(0),
      changed__(false), name__("MultiDimBucket") {
    GUM_CONSTRUCTOR(MultiDimBucket);
  }

  template < typename GUM_SCALAR >
  MultiDimBucket< GUM_SCALAR >::MultiDimBucket(
     const MultiDimBucket< GUM_SCALAR >& source) :
      MultiDimReadOnly< GUM_SCALAR >(source),
      bufferSize__(source.bufferSize__), bucket__(0),
      multiDims__(source.multiDims__), allVariables__(source.allVariables__),
      allVarsInst__(source.allVarsInst__), changed__(source.changed__),
      name__("MultiDimBucket") {
    GUM_CONS_CPY(MultiDimBucket);
  }

  template < typename GUM_SCALAR >
  MultiDimBucket< GUM_SCALAR >::~MultiDimBucket() {
    GUM_DESTRUCTOR(MultiDimBucket);
    typedef Bijection< Instantiation*, Instantiation* >::iterator_safe BiIter;

    for (BiIter iter = instantiations__.beginSafe();
         iter != instantiations__.endSafe();
         ++iter) {
      delete iter.second();
    }

    if (bucket__) { delete bucket__; }

    for (HashTableIteratorSafe< const MultiDimContainer< GUM_SCALAR >*,
                                Instantiation* > iter = multiDims__.beginSafe();
         iter != multiDims__.endSafe();
         ++iter) {
      delete iter.val();
    }
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimBucket< GUM_SCALAR >::add(
     const MultiDimContainer< GUM_SCALAR >& impl) {
    this->add(&impl);
  }

  template < typename GUM_SCALAR >
  void MultiDimBucket< GUM_SCALAR >::add(
     const MultiDimContainer< GUM_SCALAR >* impl) {
    multiDims__.insert(impl, new Instantiation(*impl));

    if (!MultiDimImplementation< GUM_SCALAR >::isInMultipleChangeMethod_()) {
      for (const auto var: impl->variablesSequence()) {
        addVariable__(var);
      }
    }

    changed__ = true;
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimBucket< GUM_SCALAR >::erase(
     const MultiDimContainer< GUM_SCALAR >& impl) {
    this->erase(&impl);
  }

  template < typename GUM_SCALAR >
  void MultiDimBucket< GUM_SCALAR >::erase(
     const MultiDimContainer< GUM_SCALAR >* impl) {
    try {
      delete multiDims__[impl];
      multiDims__.erase(impl);

      if (!MultiDimImplementation< GUM_SCALAR >::isInMultipleChangeMethod_()) {
        for (auto var: impl->variablesSequence()) {
          eraseVariable__(var);
        }
      }

      changed__ = true;
    } catch (NotFound&) {
      // Do nothing
    }
  }

  template < typename GUM_SCALAR >
  INLINE bool MultiDimBucket< GUM_SCALAR >::contains(
     const MultiDimContainer< GUM_SCALAR >& impl) const {
    return multiDims__.exists(&impl);
  }

  template < typename GUM_SCALAR >
  INLINE const Set< const DiscreteVariable* >&
               MultiDimBucket< GUM_SCALAR >::allVariables() const {
    return allVariables__;
  }

  template < typename GUM_SCALAR >
  INLINE Size MultiDimBucket< GUM_SCALAR >::bucketSize() const {
    return multiDims__.size();
  }

  template < typename GUM_SCALAR >
  INLINE bool MultiDimBucket< GUM_SCALAR >::isBucketEmpty() const {
    return multiDims__.empty();
  }

  template < typename GUM_SCALAR >
  INLINE bool MultiDimBucket< GUM_SCALAR >::bucketChanged() const {
    return changed__;
  }

  template < typename GUM_SCALAR >
  INLINE Size MultiDimBucket< GUM_SCALAR >::bufferSize() const {
    return bufferSize__;
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimBucket< GUM_SCALAR >::setBufferSize(Size ammount) {
    bufferSize__ = ammount;

    if ((this->domainSize() > bufferSize__) && (bucket__ != 0)) {
      eraseBuffer__();
    } else if (bucket__ == 0) {
      initializeBuffer__();
    }
  }

  template < typename GUM_SCALAR >
  void MultiDimBucket< GUM_SCALAR >::compute(bool force) const {
    if ((bucket__) && (changed__ || force)) {
      Instantiation values(*bucket__);

      for (values.setFirst(); !values.end(); values.inc()) {
        bucket__->set(values, computeValue__(values));
      }
    } else if ((bucket__ == 0) && changed__) {
      slavesValue__.clear();
      changed__ = false;
    }

    changed__ = false;
  }

  template < typename GUM_SCALAR >
  const std::string& MultiDimBucket< GUM_SCALAR >::name() const {
    return name__;
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimBucket< GUM_SCALAR >::add(const DiscreteVariable& v) {
    MultiDimImplementation< GUM_SCALAR >::add(v);

    if (!MultiDimImplementation< GUM_SCALAR >::isInMultipleChangeMethod_()) {
      if (this->domainSize() <= bufferSize__) {
        if (bucket__)
          bucket__->add(v);
        else
          initializeBuffer__();
      } else if (bucket__) {
        eraseBuffer__();
      }
    }
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimBucket< GUM_SCALAR >::erase(const DiscreteVariable& v) {
    MultiDimImplementation< GUM_SCALAR >::erase(v);

    if ((!MultiDimImplementation< GUM_SCALAR >::isInMultipleChangeMethod_())
        && (this->domainSize() <= bufferSize__)) {
      if (bucket__) {
        bucket__->erase(v);
      } else {
        initializeBuffer__();
      }
    }
  }

  template < typename GUM_SCALAR >
  INLINE Size MultiDimBucket< GUM_SCALAR >::realSize() const {
    return (bucket__) ? bucket__->realSize() : (Size)0;
  }

  template < typename GUM_SCALAR >
  INLINE bool
     MultiDimBucket< GUM_SCALAR >::contains(const DiscreteVariable& v) const {
    return MultiDimImplementation< GUM_SCALAR >::contains(v);
  }

  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR
     MultiDimBucket< GUM_SCALAR >::get(const Instantiation& i) const {
    compute();

    if (bucket__) {
      try {
        return bucket__->get(
           *(instantiations__.second(const_cast< Instantiation* >(&i))));
      } catch (NotFound&) { return bucket__->get(i); }
    } else if (i.isMaster(this)) {
      if (!slavesValue__.exists(&i)) {
        slavesValue__.insert(&i, computeValue__(i));
      }

      return slavesValue__[&i];
    } else {
      return computeValue__(i);
    }
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimBucket< GUM_SCALAR >::changeNotification(
     const Instantiation&          i,
     const DiscreteVariable* const var,
     Idx                           oldval,
     Idx                           newval) {
    if (bucket__) {
      try {
        bucket__->changeNotification(
           *(instantiations__).second(const_cast< Instantiation* >(&i)),
           var,
           oldval,
           newval);
      } catch (NotFound&) {
        // Then i is not a slave of this
      }
    } else {
      slavesValue__.erase(&i);
    }
  }

  template < typename GUM_SCALAR >
  INLINE void
     MultiDimBucket< GUM_SCALAR >::setFirstNotification(const Instantiation& i) {
    if (bucket__) {
      try {
        bucket__->setFirstNotification(
           *(instantiations__).second(const_cast< Instantiation* >(&i)));
      } catch (NotFound&) {
        // Then i is not a slave of this
      }
    } else {
      slavesValue__.erase(&i);
    }
  }

  template < typename GUM_SCALAR >
  INLINE void
     MultiDimBucket< GUM_SCALAR >::setLastNotification(const Instantiation& i) {
    if (bucket__) {
      try {
        bucket__->setLastNotification(
           *(instantiations__).second(const_cast< Instantiation* >(&i)));
      } catch (NotFound&) {
        // Then i is not a slave of this
      }
    } else {
      slavesValue__.erase(&i);
    }
  }

  template < typename GUM_SCALAR >
  INLINE void
     MultiDimBucket< GUM_SCALAR >::setIncNotification(const Instantiation& i) {
    if (bucket__) {
      try {
        bucket__->setIncNotification(
           *(instantiations__.second(const_cast< Instantiation* >(&i))));
      } catch (NotFound&) {
        // Then i is not a slave of this
      }
    } else {
      slavesValue__.erase(&i);
    }
  }

  template < typename GUM_SCALAR >
  INLINE void
     MultiDimBucket< GUM_SCALAR >::setDecNotification(const Instantiation& i) {
    if (bucket__) {
      try {
        bucket__->setDecNotification(
           *(instantiations__.second(const_cast< Instantiation* >(&i))));
      } catch (NotFound&) {
        // Then i is not a slave of this
      }
    } else {
      slavesValue__.erase(&i);
    }
  }

  template < typename GUM_SCALAR >
  INLINE void
     MultiDimBucket< GUM_SCALAR >::setChangeNotification(const Instantiation& i) {
    if (bucket__) {
      try {
        bucket__->setChangeNotification(
           *(instantiations__.second(const_cast< Instantiation* >(&i))));
      } catch (NotFound&) {
        // Then i is not a slave of this
      }
    } else {
      slavesValue__.erase(&i);
    }
  }

  template < typename GUM_SCALAR >
  INLINE bool MultiDimBucket< GUM_SCALAR >::registerSlave(Instantiation& i) {
    if (bucket__) {
      try {
        instantiations__.insert(&i, new Instantiation(*bucket__));
      } catch (DuplicateElement&) { return false; }
    }

    return MultiDimImplementation< GUM_SCALAR >::registerSlave(i);
  }

  template < typename GUM_SCALAR >
  INLINE bool MultiDimBucket< GUM_SCALAR >::unregisterSlave(Instantiation& i) {
    MultiDimReadOnly< GUM_SCALAR >::unregisterSlave(i);

    if (bucket__) {
      try {
        delete instantiations__.second(&i);
        instantiations__.eraseFirst(&i);
        return true;
      } catch (NotFound&) { return false; }
    } else {
      if (slavesValue__.exists(&i)) {
        slavesValue__.erase(&i);
        return true;
      } else {
        return false;
      }
    }
  }

  template < typename GUM_SCALAR >
  INLINE MultiDimAdressable& MultiDimBucket< GUM_SCALAR >::getMasterRef() {
    if (bucket__) {
      return *bucket__;
    } else {
      return *this;
    }
  }

  template < typename GUM_SCALAR >
  INLINE const MultiDimAdressable&
               MultiDimBucket< GUM_SCALAR >::getMasterRef() const {
    if (bucket__) {
      return *bucket__;
    } else {
      return *this;
    }
  }

  template < typename GUM_SCALAR >
  INLINE const std::string
               MultiDimBucket< GUM_SCALAR >::toString(const Instantiation* i) const {
    std::stringstream sBuff;
    sBuff << (*i) << " = " << get(*i);
    return sBuff.str();
  }

  template < typename GUM_SCALAR >
  void MultiDimBucket< GUM_SCALAR >::commitMultipleChanges_() {
    MultiDimImplementation< GUM_SCALAR >::commitMultipleChanges_();

    if (this->domainSize() <= bufferSize__) {
      initializeBuffer__();
    } else {
      eraseBuffer__();
    }

    allVariables__.clear();

    while (!allVarsInst__.empty()) {
      allVarsInst__.erase(**(allVarsInst__.variablesSequence().beginSafe()));
    }

    for (   // HashTableIteratorSafe<const MultiDimContainer<GUM_SCALAR>*,
            // Instantiation*>
       auto iter = multiDims__.beginSafe(); iter != multiDims__.endSafe();
       ++iter) {
      for (auto var: iter.key()->variablesSequence()) {
        addVariable__(var);
      }
    }

    changed__ = true;
  }

  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR&
         MultiDimBucket< GUM_SCALAR >::get_(const Instantiation& i) const {
    GUM_ERROR(OperationNotAllowed, "a MultiDimBucket is a read only MultiDim");
  }

  template < typename GUM_SCALAR >
  INLINE void
     MultiDimBucket< GUM_SCALAR >::addVariable__(const DiscreteVariable* var) {
    try {
      allVariables__.insert(var);
      allVarsInst__.add(*var);
    } catch (DuplicateElement&) {
      // Nothing to do then!
    }
  }

  template < typename GUM_SCALAR >
  void MultiDimBucket< GUM_SCALAR >::eraseVariable__(const DiscreteVariable* var) {
    bool found = false;

    for (HashTableIteratorSafe< const MultiDimContainer< GUM_SCALAR >*,
                                Instantiation* > iter = multiDims__.beginSafe();
         iter != multiDims__.endSafe();
         ++iter) {
      if (iter.key()->contains(*var)) {
        found = true;
        break;
      }
    }

    // No one use it, we can safely remove it
    if (!found) {
      allVariables__.erase(var);
      allVarsInst__.erase(*var);
    }
  }

  template < typename GUM_SCALAR >
  void MultiDimBucket< GUM_SCALAR >::initializeBuffer__() {
    if (bucket__) {
      typedef Bijection< Instantiation*, Instantiation* >::iterator_safe BiIter;

      for (BiIter iter = instantiations__.beginSafe();
           iter != instantiations__.endSafe();
           ++iter) {
        delete iter.second();
      }

      instantiations__.clear();
      delete bucket__;
      bucket__ = 0;
    }

    // Creating the table.
    bucket__ = new MultiDimArray< GUM_SCALAR >();

    for (auto var: this->variablesSequence()) {
      bucket__->add(*var);
    }

    if (!this->slaves_().empty()) {
      for (List< Instantiation* >::const_iterator_safe iter =
              this->slaves_().cbeginSafe();
           iter != this->slaves_().cendSafe();
           ++iter) {
        instantiations__.insert(*iter, new Instantiation(*bucket__));
      }
    }

    changed__ = true;
  }

  template < typename GUM_SCALAR >
  void MultiDimBucket< GUM_SCALAR >::eraseBuffer__() {
    if (bucket__) {
      typedef Bijection< Instantiation*, Instantiation* >::iterator_safe BiIter;

      for (BiIter iter = instantiations__.beginSafe();
           iter != instantiations__.endSafe();
           ++iter) {
        delete iter.second();
      }

      instantiations__.clear();
      delete bucket__;
      bucket__ = 0;
    }
  }

  template < typename GUM_SCALAR >
  GUM_SCALAR MultiDimBucket< GUM_SCALAR >::computeValue__(
     const Instantiation& value) const {
    try {
      GUM_SCALAR sum = (GUM_SCALAR)0;
      GUM_SCALAR current;
      allVarsInst__.setVals(value);

      for (allVarsInst__.setFirstOut(value); !allVarsInst__.end();
           allVarsInst__.incOut(value)) {
        current = (GUM_SCALAR)1;

        for (HashTableIteratorSafe< const MultiDimContainer< GUM_SCALAR >*,
                                    Instantiation* > iter =
                multiDims__.beginSafe();
             iter != multiDims__.endSafe();
             ++iter) {
          (iter.val())->setVals(allVarsInst__);
          current *= iter.key()->get(*(iter.val()));
        }

        sum += current;
      }

      return sum;
    } catch (NotFound& e) {
      std::cerr << std::endl << e.errorContent() << std::endl;
      // This happens if the bucket is empty.
      GUM_ERROR(SizeError, "This MultiDimBucket is empty.");
    }
  }

  template < typename GUM_SCALAR >
  INLINE MultiDimContainer< GUM_SCALAR >*
         MultiDimBucket< GUM_SCALAR >::newFactory() const {
    return new MultiDimBucket< GUM_SCALAR >;
  }

  template < typename GUM_SCALAR >
  INLINE const MultiDimArray< GUM_SCALAR >&
               MultiDimBucket< GUM_SCALAR >::bucket() const {
    if (bucket__) {
      return *bucket__;
    } else {
      GUM_ERROR(OperationNotAllowed, "bucket not used.");
    }
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimBucket< GUM_SCALAR >::replace_(const DiscreteVariable* x,
                                                     const DiscreteVariable* y) {
    MultiDimImplementation< GUM_SCALAR >::replace_(x, y);
    typedef Bijection< Instantiation*, Instantiation* >::iterator_safe Iter;

    for (Iter iter = instantiations__.beginSafe();
         iter != instantiations__.endSafe();
         ++iter) {
      iter.first()->replace(*x, *y);
      iter.second()->replace(*x, *y);
    }

    if (bucket__) bucket__->replace(*x, *y);

    allVariables__.erase(x);
    allVariables__.insert(y);
    allVarsInst__.replace(*x, *y);
  }

  template < typename GUM_SCALAR >
  INLINE const HashTable< const MultiDimContainer< GUM_SCALAR >*, Instantiation* >&
               MultiDimBucket< GUM_SCALAR >::multidims() const {
    return multiDims__;
  }

} /* namespace gum */
