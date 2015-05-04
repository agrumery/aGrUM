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
/**
 * @file
 * @brief Implementation of the MultiDimBucket class.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <agrum/multidim/multiDimBucket.h>

namespace gum {

  template <typename GUM_SCALAR>
  MultiDimBucket<GUM_SCALAR>::MultiDimBucket(Size bufferSize)
      : MultiDimReadOnly<GUM_SCALAR>(), __bufferSize(bufferSize), __bucket(0),
        __changed(false), __name("MultiDimBucket") {
    GUM_CONSTRUCTOR(MultiDimBucket);
  }

  template <typename GUM_SCALAR>
  MultiDimBucket<GUM_SCALAR>::MultiDimBucket(
      const MultiDimBucket<GUM_SCALAR> &source)
      : MultiDimReadOnly<GUM_SCALAR>(source), __bufferSize(source.__bufferSize),
        __bucket(0), __multiDims(source.__multiDims),
        __allVariables(source.__allVariables), __allVarsInst(source.__allVarsInst),
        __changed(source.__changed), __name("MultiDimBucket") {
    GUM_CONS_CPY(MultiDimBucket);
  }

  template <typename GUM_SCALAR> MultiDimBucket<GUM_SCALAR>::~MultiDimBucket() {
    GUM_DESTRUCTOR(MultiDimBucket);
    typedef Bijection<Instantiation *, Instantiation *>::iterator_safe BiIter;

    for (BiIter iter = __instantiations.beginSafe();
         iter != __instantiations.endSafe(); ++iter) {
      delete iter.second();
    }

    if (__bucket) {
      delete __bucket;
    }

    for (HashTableIteratorSafe<const MultiDimContainer<GUM_SCALAR> *,
                               Instantiation *> iter = __multiDims.beginSafe();
         iter != __multiDims.endSafe(); ++iter) {
      delete iter.val();
    }
  }

  template <typename GUM_SCALAR>
  INLINE void
  MultiDimBucket<GUM_SCALAR>::add(const MultiDimContainer<GUM_SCALAR> &impl) {
    this->add(&impl);
  }

  template <typename GUM_SCALAR>
  void MultiDimBucket<GUM_SCALAR>::add(const MultiDimContainer<GUM_SCALAR> *impl) {
    __multiDims.insert(impl, new Instantiation(*impl));

    if (!MultiDimImplementation<GUM_SCALAR>::_isInMultipleChangeMethod()) {
      for (MultiDimInterface::iterator iter = impl->begin(); iter != impl->end();
           ++iter) {
        __addVariable(*iter);
      }
    }

    __changed = true;
  }

  template <typename GUM_SCALAR>
  INLINE void
  MultiDimBucket<GUM_SCALAR>::erase(const MultiDimContainer<GUM_SCALAR> &impl) {
    this->erase(&impl);
  }

  template <typename GUM_SCALAR>
  void MultiDimBucket<GUM_SCALAR>::erase(const MultiDimContainer<GUM_SCALAR> *impl) {
    try {
      delete __multiDims[impl];
      __multiDims.erase(impl);

      if (!MultiDimImplementation<GUM_SCALAR>::_isInMultipleChangeMethod()) {
        for (MultiDimInterface::iterator iter = impl->begin(); iter != impl->end();
             ++iter) {
          __eraseVariable(*iter);
        }
      }

      __changed = true;
    } catch (NotFound &) {
      // Do nothing
    }
  }

  template <typename GUM_SCALAR>
  INLINE bool MultiDimBucket<GUM_SCALAR>::contains(
      const MultiDimContainer<GUM_SCALAR> &impl) const {
    return __multiDims.exists(&impl);
  }

  template <typename GUM_SCALAR>
  INLINE const Set<const DiscreteVariable *> &
  MultiDimBucket<GUM_SCALAR>::allVariables() const {
    return __allVariables;
  }

  template <typename GUM_SCALAR>
  INLINE Size MultiDimBucket<GUM_SCALAR>::bucketSize() const {
    return __multiDims.size();
  }

  template <typename GUM_SCALAR>
  INLINE bool MultiDimBucket<GUM_SCALAR>::isBucketEmpty() const {
    return __multiDims.empty();
  }

  template <typename GUM_SCALAR>
  INLINE bool MultiDimBucket<GUM_SCALAR>::bucketChanged() const {
    return __changed;
  }

  template <typename GUM_SCALAR>
  INLINE Size MultiDimBucket<GUM_SCALAR>::bufferSize() const {
    return __bufferSize;
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimBucket<GUM_SCALAR>::setBufferSize(Size ammount) {
    __bufferSize = ammount;

    if ((this->domainSize() > __bufferSize) and (__bucket != 0)) {
      __eraseBuffer();
    } else if (__bucket == 0) {
      __initializeBuffer();
    }
  }

  template <typename GUM_SCALAR>
  void MultiDimBucket<GUM_SCALAR>::compute(bool force) const {
    if ((__bucket) and (__changed or force)) {
      Instantiation values(*__bucket);

      for (values.setFirst(); !values.end(); values.inc()) {
        __bucket->set(values, __computeValue(values));
      }
    } else if ((__bucket == 0) and __changed) {
      __slavesValue.clear();
      __changed = false;
    }

    __changed = false;
  }

  template <typename GUM_SCALAR>
  const std::string &MultiDimBucket<GUM_SCALAR>::name() const {
    return __name;
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimBucket<GUM_SCALAR>::add(const DiscreteVariable &v) {
    MultiDimImplementation<GUM_SCALAR>::add(v);

    if (not MultiDimImplementation<GUM_SCALAR>::_isInMultipleChangeMethod()) {
      if (this->domainSize() <= __bufferSize) {
        if (__bucket)
          __bucket->add(v);
        else
          __initializeBuffer();
      } else if (__bucket) {
        __eraseBuffer();
      }
    }
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimBucket<GUM_SCALAR>::erase(const DiscreteVariable &v) {
    MultiDimImplementation<GUM_SCALAR>::erase(v);

    if ((not MultiDimImplementation<GUM_SCALAR>::_isInMultipleChangeMethod()) and
        (this->domainSize() <= __bufferSize)) {
      if (__bucket) {
        __bucket->erase(v);
      } else {
        __initializeBuffer();
      }
    }
  }

  template <typename GUM_SCALAR>
  INLINE Size MultiDimBucket<GUM_SCALAR>::realSize() const {
    return (__bucket) ? __bucket->realSize() : (Size)0;
  }

  template <typename GUM_SCALAR>
  INLINE bool MultiDimBucket<GUM_SCALAR>::contains(const DiscreteVariable &v) const {
    return MultiDimImplementation<GUM_SCALAR>::contains(v);
  }

  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR MultiDimBucket<GUM_SCALAR>::get(const Instantiation &i) const {
    compute();

    if (__bucket) {
      try {
        return __bucket->get(
            *(__instantiations.second(const_cast<Instantiation *>(&i))));
      } catch (NotFound &) {
        return __bucket->get(i);
      }
    } else if (i.isMaster(this)) {
      if (!__slavesValue.exists(&i)) {
        __slavesValue.insert(&i, __computeValue(i));
      }

      return __slavesValue[&i];
    } else {
      return __computeValue(i);
    }
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimBucket<GUM_SCALAR>::changeNotification(
      Instantiation &i, const DiscreteVariable *const var, const Idx &oldval,
      const Idx &newval) {
    if (__bucket) {
      try {
        __bucket->changeNotification(*(__instantiations).second(&i), var, oldval,
                                     newval);
      } catch (NotFound &) {
        // Then i is not a slave of this
      }
    } else {
      __slavesValue.erase(&i);
    }
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimBucket<GUM_SCALAR>::setFirstNotification(Instantiation &i) {
    if (__bucket) {
      try {
        __bucket->setFirstNotification(*(__instantiations).second(&i));
      } catch (NotFound &) {
        // Then i is not a slave of this
      }
    } else {
      __slavesValue.erase(&i);
    }
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimBucket<GUM_SCALAR>::setLastNotification(Instantiation &i) {
    if (__bucket) {
      try {
        __bucket->setLastNotification(*(__instantiations).second(&i));
      } catch (NotFound &) {
        // Then i is not a slave of this
      }
    } else {
      __slavesValue.erase(&i);
    }
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimBucket<GUM_SCALAR>::setIncNotification(Instantiation &i) {
    if (__bucket) {
      try {
        __bucket->setIncNotification(*(__instantiations.second(&i)));
      } catch (NotFound &) {
        // Then i is not a slave of this
      }
    } else {
      __slavesValue.erase(&i);
    }
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimBucket<GUM_SCALAR>::setDecNotification(Instantiation &i) {
    if (__bucket) {
      try {
        __bucket->setDecNotification(*(__instantiations.second(&i)));
      } catch (NotFound &) {
        // Then i is not a slave of this
      }
    } else {
      __slavesValue.erase(&i);
    }
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimBucket<GUM_SCALAR>::setChangeNotification(Instantiation &i) {
    if (__bucket) {
      try {
        __bucket->setChangeNotification(*(__instantiations.second(&i)));
      } catch (NotFound &) {
        // Then i is not a slave of this
      }
    } else {
      __slavesValue.erase(&i);
    }
  }

  template <typename GUM_SCALAR>
  INLINE bool MultiDimBucket<GUM_SCALAR>::registerSlave(Instantiation &i) {
    if (__bucket) {
      try {
        __instantiations.insert(&i, new Instantiation(*__bucket));
      } catch (DuplicateElement &) {
        return false;
      }
    }

    return MultiDimImplementation<GUM_SCALAR>::registerSlave(i);
  }

  template <typename GUM_SCALAR>
  INLINE bool MultiDimBucket<GUM_SCALAR>::unregisterSlave(Instantiation &i) {
    MultiDimReadOnly<GUM_SCALAR>::unregisterSlave(i);

    if (__bucket) {
      try {
        delete __instantiations.second(&i);
        __instantiations.eraseFirst(&i);
        return true;
      } catch (NotFound &) {
        return false;
      }
    } else {
      if (__slavesValue.exists(&i)) {
        __slavesValue.erase(&i);
        return true;
      } else {
        return false;
      }
    }
  }

  template <typename GUM_SCALAR>
  INLINE MultiDimAdressable &MultiDimBucket<GUM_SCALAR>::getMasterRef(void) {
    if (__bucket) {
      return *__bucket;
    } else {
      return *this;
    }
  }

  template <typename GUM_SCALAR>
  INLINE const MultiDimAdressable &
  MultiDimBucket<GUM_SCALAR>::getMasterRef(void) const {
    if (__bucket) {
      return *__bucket;
    } else {
      return *this;
    }
  }

  template <typename GUM_SCALAR>
  INLINE const std::string
  MultiDimBucket<GUM_SCALAR>::toString(const Instantiation *i) const {
    std::stringstream sBuff;
    sBuff << (*i) << " = " << get(*i);
    return sBuff.str();
  }

  template <typename GUM_SCALAR>
  void MultiDimBucket<GUM_SCALAR>::_commitMultipleChanges() {
    MultiDimImplementation<GUM_SCALAR>::_commitMultipleChanges();

    if (this->domainSize() <= __bufferSize) {
      __initializeBuffer();
    } else {
      __eraseBuffer();
    }

    __allVariables.clear();

    while (not __allVarsInst.empty()) {
      __allVarsInst.erase(**(__allVarsInst.variablesSequence().beginSafe()));
    }

    for (HashTableIteratorSafe<const MultiDimContainer<GUM_SCALAR> *,
                               Instantiation *> iter = __multiDims.beginSafe();
         iter != __multiDims.endSafe(); ++iter) {
      for (MultiDimInterface::iterator jter = iter.key()->begin();
           jter != iter.key()->end(); ++jter) {
        __addVariable(*jter);
      }
    }

    __changed = true;
  }

  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR &MultiDimBucket<GUM_SCALAR>::_get(const Instantiation &i) const {
    GUM_ERROR(OperationNotAllowed, "a MultiDimBucket is a read only MultiDim");
  }

  template <typename GUM_SCALAR>
  INLINE void
  MultiDimBucket<GUM_SCALAR>::__addVariable(const DiscreteVariable *var) {
    try {
      __allVariables.insert(var);
      __allVarsInst.add(*var);
    } catch (DuplicateElement &e) {
      // Nothing to do then!
    }
  }

  template <typename GUM_SCALAR>
  void MultiDimBucket<GUM_SCALAR>::__eraseVariable(const DiscreteVariable *var) {
    bool found = false;

    for (HashTableIteratorSafe<const MultiDimContainer<GUM_SCALAR> *,
                               Instantiation *> iter = __multiDims.beginSafe();
         iter != __multiDims.endSafe(); ++iter) {
      if (iter.key()->contains(*var)) {
        found = true;
        break;
      }
    }

    // No one use it, we can safely remove it
    if (!found) {
      __allVariables.erase(var);
      __allVarsInst.erase(*var);
    }
  }

  template <typename GUM_SCALAR>
  void MultiDimBucket<GUM_SCALAR>::__initializeBuffer() {
    if (__bucket) {
      typedef Bijection<Instantiation *, Instantiation *>::iterator_safe BiIter;

      for (BiIter iter = __instantiations.beginSafe();
           iter != __instantiations.endSafe(); ++iter) {
        delete iter.second();
      }

      __instantiations.clear();
      delete __bucket;
      __bucket = 0;
    }

    // Creating the table.
    __bucket = new MultiDimArray<GUM_SCALAR>();

    for (MultiDimInterface::iterator iter = this->begin(); iter != this->end();
         ++iter) {
      __bucket->add(**iter);
    }

    if (not this->_slaves().empty()) {
      for (List<Instantiation *>::const_iterator_safe iter =
               this->_slaves().cbeginSafe();
           iter != this->_slaves().cendSafe(); ++iter) {
        __instantiations.insert(*iter, new Instantiation(*__bucket));
      }
    }

    __changed = true;
  }

  template <typename GUM_SCALAR> void MultiDimBucket<GUM_SCALAR>::__eraseBuffer() {
    if (__bucket) {
      typedef Bijection<Instantiation *, Instantiation *>::iterator_safe BiIter;

      for (BiIter iter = __instantiations.beginSafe();
           iter != __instantiations.endSafe(); ++iter) {
        delete iter.second();
      }

      __instantiations.clear();
      delete __bucket;
      __bucket = 0;
    }
  }

  template <typename GUM_SCALAR>
  GUM_SCALAR
  MultiDimBucket<GUM_SCALAR>::__computeValue(const Instantiation &value) const {
    try {
      GUM_SCALAR sum = (GUM_SCALAR)0;
      GUM_SCALAR current;
      __allVarsInst.setVals(value);

      for (__allVarsInst.setFirstOut(value); not __allVarsInst.end();
           __allVarsInst.incOut(value)) {
        current = (GUM_SCALAR)1;

        for (HashTableIteratorSafe<const MultiDimContainer<GUM_SCALAR> *,
                                   Instantiation *> iter = __multiDims.beginSafe();
             iter != __multiDims.endSafe(); ++iter) {
          (iter.val())->setVals(__allVarsInst);
          current *= iter.key()->get(*(iter.val()));
        }

        sum += current;
      }

      return sum;
    } catch (NotFound &e) {
      std::cerr << std::endl << e.errorContent() << std::endl;
      // This happens if the bucket is empty.
      GUM_ERROR(SizeError, "This MultiDimBucket is empty.");
    }
  }

  template <typename GUM_SCALAR>
  INLINE MultiDimContainer<GUM_SCALAR> *
  MultiDimBucket<GUM_SCALAR>::newFactory() const {
    return new MultiDimBucket<GUM_SCALAR>;
  }

  template <typename GUM_SCALAR>
  INLINE const MultiDimArray<GUM_SCALAR> &
  MultiDimBucket<GUM_SCALAR>::bucket() const {
    if (__bucket) {
      return *__bucket;
    } else {
      GUM_ERROR(OperationNotAllowed, "bucket not used.");
    }
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimBucket<GUM_SCALAR>::_swap(const DiscreteVariable *x,
                                                const DiscreteVariable *y) {
    MultiDimImplementation<GUM_SCALAR>::_swap(x, y);
    typedef Bijection<Instantiation *, Instantiation *>::iterator_safe Iter;

    for (Iter iter = __instantiations.beginSafe();
         iter != __instantiations.endSafe(); ++iter) {
      iter.first()->swap(*x, *y);
      iter.second()->swap(*x, *y);
    }

    if (__bucket)
      __bucket->swap(*x, *y);

    __allVariables.erase(x);
    __allVariables.insert(y);
    __allVarsInst.swap(*x, *y);
  }

  template <typename GUM_SCALAR>
  INLINE const HashTable<const MultiDimContainer<GUM_SCALAR> *, Instantiation *> &
  MultiDimBucket<GUM_SCALAR>::multidims() const {
    return __multiDims;
  }

} /* namespace gum */
