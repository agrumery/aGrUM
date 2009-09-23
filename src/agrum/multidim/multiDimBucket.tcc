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
 * @author Lionel Torti
 */
// ============================================================================
#include <agrum/multidim/multiDimBucket.h>
// ============================================================================
namespace gum {

template<typename T_DATA>
MultiDimBucket<T_DATA>::MultiDimBucket(Size bufferSize):
  MultiDimReadOnly<T_DATA>(), __bufferSize(bufferSize),
  __instantiations(0), __bucket(0), __changed(false)
{
  GUM_CONSTRUCTOR( MultiDimBucket );
}

template<typename T_DATA>
MultiDimBucket<T_DATA>::MultiDimBucket(const MultiDimBucket<T_DATA>& source):
  MultiDimReadOnly<T_DATA>(source), __bufferSize(source.__bufferSize),
  __instantiations(0), __bucket(0), __multiDims(source.__multiDims),
  __allVariables(source.__allVariables), __changed(source.__changed)
{
  GUM_CONS_CPY( MultiDimBucket );
}

template<typename T_DATA>
MultiDimBucket<T_DATA>::~MultiDimBucket()
{
  GUM_DESTRUCTOR( MultiDimBucket );
  if (__bucket != 0) {
    delete __bucket;
  }
  if (__instantiations != 0) {
    delete __instantiations;
  }
}

template<typename T_DATA> INLINE
void
MultiDimBucket<T_DATA>::add(const MultiDimContainer<T_DATA>& impl)
{
  this->add(&impl);
}

template<typename T_DATA>
void
MultiDimBucket<T_DATA>::add(const MultiDimContainer<T_DATA>* impl)
{
  __multiDims.insert(impl);
  if (! MultiDimImplementation<T_DATA>::_isInMultipleChangeMethod()) {
    for (MultiDimInterface::iterator iter = impl->begin(); iter != impl->end(); ++iter) {
      __addVariable(*iter);
    }
  }
  __changed = true;
}

template<typename T_DATA> INLINE
void
MultiDimBucket<T_DATA>::erase(const MultiDimContainer<T_DATA>& impl)
{
  this->erase(&impl);
}

template<typename T_DATA>
void
MultiDimBucket<T_DATA>::erase(const MultiDimContainer<T_DATA>* impl)
{
  __multiDims.erase(impl);
  if (! MultiDimImplementation<T_DATA>::_isInMultipleChangeMethod()) {
    for (MultiDimInterface::iterator iter = impl->begin(); iter != impl->end(); ++iter) {
      __eraseVariable(*iter);
    }
  }
  __changed = true;
}

template<typename T_DATA> INLINE
bool
MultiDimBucket<T_DATA>::contains(const MultiDimContainer<T_DATA>& impl) const
{
  return __multiDims.contains(&impl);
}

template<typename T_DATA> INLINE
const Set<const DiscreteVariable*>&
MultiDimBucket<T_DATA>::allVariables() const
{
  return __allVariables;
}

template<typename T_DATA> INLINE
Size
MultiDimBucket<T_DATA>::bucketSize() const
{
  return __multiDims.size();
}


template<typename T_DATA> INLINE
bool
MultiDimBucket<T_DATA>::isBucketEmpty() const
{
  return __multiDims.empty();
}

template<typename T_DATA> INLINE
bool
MultiDimBucket<T_DATA>::bucketChanged() const
{
  return __changed;
}

template<typename T_DATA> INLINE
Size
MultiDimBucket<T_DATA>::bufferSize() const
{
  return __bufferSize;
}

template<typename T_DATA> INLINE
void
MultiDimBucket<T_DATA>::setBufferSize(Size ammount)
{
  __bufferSize = ammount;
  if ( (this->domainSize() > __bufferSize) && (__bucket != 0) ) {
    __eraseBuffer();
  } else if (__bucket == 0) {
    __initializeBuffer();
  }
}

template<typename T_DATA>
void
MultiDimBucket<T_DATA>::compute(bool force) const
{
  if ( (__bucket != 0) and (__changed or force) ) {
    Instantiation values(*__bucket);
    for (values.setFirst(); ! values.end(); values.inc()) {
      __bucket->set(values, __computeValue(values));
    }
  } else if ( (__bucket == 0) and __changed) {
      __slavesValue.clear();
      __changed = false;
  }
  __changed = false;
}

template<typename T_DATA> INLINE
void
MultiDimBucket<T_DATA>::add (const DiscreteVariable &v)
{
  MultiDimImplementation<T_DATA>::add(v);
  if ( (not MultiDimImplementation<T_DATA>::_isInMultipleChangeMethod()) and
       (this->domainSize() <= __bufferSize) ) {
    if (__bucket == 0) {
      __initializeBuffer();
    } else {
      __bucket->add(v);
    }
  }
}

template<typename T_DATA> INLINE
void
MultiDimBucket<T_DATA>::erase (const DiscreteVariable &v)
{
  MultiDimImplementation<T_DATA>::erase(v);
  if ( (not MultiDimImplementation<T_DATA>::_isInMultipleChangeMethod()) and
       (this->domainSize() <= __bufferSize) ) {
    if (__bucket == 0) {
      __initializeBuffer();
    } else {
      __bucket->erase(v);
    }
  }
}

template<typename T_DATA> INLINE
Size
MultiDimBucket<T_DATA>::realSize() const
{
  return (__bucket != 0)?__bucket->realSize():(Size) 0;
}

template<typename T_DATA> INLINE
bool
MultiDimBucket<T_DATA>::contains (const DiscreteVariable &v) const
{
  return MultiDimImplementation<T_DATA>::contains(v);
}

template<typename T_DATA> INLINE
T_DATA
MultiDimBucket<T_DATA>::get(const Instantiation &i) const
{
  compute();
  if (__bucket != 0) {
    return __bucket->get(i);
  } else if (i.isMaster(this)) {
    if (! __slavesValue.exists(&i)) {
      __slavesValue.insert(&i, __computeValue(i));
    }
    return __slavesValue[&i];
  } else {
    return __computeValue(i);
  }
}

template<typename T_DATA> INLINE
void
MultiDimBucket<T_DATA>::changeNotification (Instantiation &i,
    const DiscreteVariable *const var,
    const Idx &oldval, const Idx &newval)
{
  if (__bucket != 0) {
    __bucket->changeNotification(i, var, oldval, newval);
  } else if (i.isMaster(this)) {
    __slavesValue.erase(&i);
  }
}

template<typename T_DATA> INLINE
void
MultiDimBucket<T_DATA>::setFirstNotification(Instantiation &i)
{
  if (__bucket != 0) {
    __bucket->setFirstNotification(i);
  } else if (i.isMaster(this)) {
    __slavesValue.erase(&i);
  }
}

template<typename T_DATA> INLINE
void
MultiDimBucket<T_DATA>::setLastNotification(Instantiation &i)
{
  if (__bucket != 0) {
    __bucket->setLastNotification(i);
  } else if (i.isMaster(this)) {
    __slavesValue.erase(&i);
  }
}

template<typename T_DATA> INLINE
void
MultiDimBucket<T_DATA>::setIncNotification(Instantiation &i)
{
  if (__bucket != 0) {
    __bucket->setIncNotification(i);
  } else if (i.isMaster(this)) {
    __slavesValue.erase(&i);
  }
}

template<typename T_DATA> INLINE
void
MultiDimBucket<T_DATA>::setDecNotification(Instantiation &i)
{
  if (__bucket != 0) {
    __bucket->setDecNotification(i);
  } else if (i.isMaster(this)) {
    __slavesValue.erase(&i);
  }
}

template<typename T_DATA> INLINE
void
MultiDimBucket<T_DATA>::setChangeNotification(Instantiation &i)
{
  if (__bucket != 0) {
    __bucket->setChangeNotification(i);
  } else if (i.isMaster(this)) {
    __slavesValue.erase(&i);
  }
}

template<typename T_DATA> INLINE
bool
MultiDimBucket<T_DATA>::registerSlave (Instantiation &i)
{
  if (__bucket != 0) {
    i.forgetMaster();
    if (i.actAsSlave(*__bucket)) {
      __instantiations->insert(&i);
      return true;
    } else {
      return false;
    }
  } else {
    return MultiDimImplementation<T_DATA>::registerSlave(i);
  }
}

template<typename T_DATA> INLINE
bool
MultiDimBucket<T_DATA>::unregisterSlave (Instantiation &i)
{
  MultiDimReadOnly<T_DATA>::unregisterSlave(i);
  if (__bucket != 0) {
    __bucket->unregisterSlave(i);
    __instantiations->eraseByVal(&i);
    return true;
  } else {
    MultiDimImplementation<T_DATA>::unregisterSlave(i);
    __slavesValue.erase(&i);
    return true;
  }
}

template<typename T_DATA> INLINE
MultiDimAdressable&
MultiDimBucket<T_DATA>::getMasterRef (void)
{
  if (__bucket != 0) {
    return *__bucket;
  } else {
    return *this;
  }
}

template<typename T_DATA> INLINE
const MultiDimAdressable&
MultiDimBucket<T_DATA>::getMasterRef (void) const
{
  if (__bucket != 0) {
    return *__bucket;
  } else {
    return *this;
  }
}

template<typename T_DATA> INLINE
const std::string
MultiDimBucket<T_DATA>::toString(const Instantiation *i) const
{
  std::stringstream sBuff;
  sBuff << (*i) << " = " << get(*i);
  return sBuff.str();
}

template<typename T_DATA>
void
MultiDimBucket<T_DATA>::_commitMultipleChanges()
{
  MultiDimImplementation<T_DATA>::_commitMultipleChanges();
  if (this->domainSize() <= __bufferSize) {
    __initializeBuffer();
  } else {
    __eraseBuffer();
  }
  __allVariables.clear();
  for (SetIterator< const MultiDimContainer<T_DATA>* > iter = __multiDims.begin(); iter != __multiDims.end(); ++iter) {
    for (MultiDimInterface::iterator jter = (*iter)->begin(); jter != (*iter)->end(); ++jter) {
      __addVariable(*jter);
    }
  }
  __changed = true;
}

template<typename T_DATA> INLINE
T_DATA&
MultiDimBucket<T_DATA>::_get(const Instantiation &i) const
{
  GUM_ERROR(OperationNotAllowed, "You should call this method in a MultiDimBucket.");
}

template<typename T_DATA> INLINE
void
MultiDimBucket<T_DATA>::__addVariable(const DiscreteVariable* var)
{
  try {
    __allVariables.insert(var);
  } catch (DuplicateElement& e) {
    // Nothing to do then!
  }
}

template<typename T_DATA>
void
MultiDimBucket<T_DATA>::__eraseVariable(const DiscreteVariable* var)
{
  bool found = false;
  typedef SetIterator<const MultiDimContainer<T_DATA>* > SetIter;
  for (SetIter iter = __multiDims.begin(); iter != __multiDims.end(); ++iter) {
    if ((*iter)->contains(*var)) {
      found = true;
      break;
    }
  }
  // No one use it, we can safely remove it
  if (! found) {
    __allVariables.erase(var);
  }
}

template<typename T_DATA>
void
MultiDimBucket<T_DATA>::__initializeBuffer()
{
  if (__bucket != 0) {
    delete __bucket;
    __bucket = 0;
  }
  // Creating the table.
  __bucket = new MultiDimArray<T_DATA>();
  for (MultiDimInterface::iterator iter = this->begin(); iter != this->end(); ++iter) {
    __bucket->add(**iter);
  }
  if (__instantiations == 0) {
    __instantiations = new List<Instantiation*>(this->_slaves());
  }
  // Associating the instantiations to the new table.
  Instantiation* inst = 0;
  while (__instantiations->size() > 0) {
    inst =  __instantiations->back();
    inst->forgetMaster();
    if (! inst->actAsSlave(*__bucket)) {
      (*inst) = Instantiation(*__bucket);
    }
  }
  delete __instantiations;
  __instantiations = 0;
  __changed = true;
}

template<typename T_DATA>
void
MultiDimBucket<T_DATA>::__eraseBuffer()
{
  if (__bucket != 0) {
    delete __bucket;
    __bucket = 0;
  }
  // Moving the instantiations on this bucket.
  if (__instantiations != 0) {
    for (List<Instantiation*>::iterator iter = __instantiations->begin(); iter != __instantiations->end(); ++iter) {
      (*iter)->forgetMaster();
      if (! (*iter)->actAsSlave(*this)) {
        (**iter) = Instantiation(*this);
      }
    }
    delete __instantiations;
    __instantiations = 0;
  }
}

template<typename T_DATA>
T_DATA
MultiDimBucket<T_DATA>::__computeValue(const Instantiation& value) const
{
  try {
    T_DATA sum = (T_DATA) 0;
    T_DATA current;
    Instantiation allVar_inst;
    for (Set<const DiscreteVariable*>::iterator iter = __allVariables.begin(); iter != __allVariables.end(); ++iter) {
      allVar_inst.add(**iter);
    }
    allVar_inst.chgValIn(value);
    for (allVar_inst.setFirstOut(value); ! allVar_inst.end(); allVar_inst.incOut(value)) {
      current = (T_DATA) 1;
      for (SetIterator< const MultiDimContainer<T_DATA>* > iter = __multiDims.begin(); iter != __multiDims.end(); ++iter) {
        current *= (*iter)->get(allVar_inst);
      }
      sum += current;
    }
    return sum;
  } catch (NotFound&) {
    // This happens if the bucket is empty.
    GUM_ERROR(SizeError, "This MultiDimBucket is empty.");
  }
}

template <typename T_DATA> INLINE
MultiDimContainer<T_DATA>*
MultiDimBucket<T_DATA>::newFactory() const {
  return new MultiDimBucket<T_DATA>;
}

} /* namespace gum */
// ============================================================================
