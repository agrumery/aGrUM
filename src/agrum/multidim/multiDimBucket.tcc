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

// Default constructor.
template<typename T_DATA> INLINE
MultiDimBucket<T_DATA>::MultiDimBucket(Size bufferSize):
  MultiDimReadOnly<T_DATA>(), __bufferSize(bufferSize),
  __bucket(0), __changed(false)
{
  GUM_CONSTRUCTOR( MultiDimBucket );
}

// Copy constructor.
template<typename T_DATA> INLINE
MultiDimBucket<T_DATA>::MultiDimBucket(const MultiDimBucket<T_DATA>& source):
  MultiDimReadOnly<T_DATA>(source), __bufferSize(source.__bufferSize),
  __bucket(0), __multiDims(source.__multiDims), __allVariables(source.__allVariables),
  __changed(source.__changed)
{
  GUM_CONS_CPY( MultiDimBucket );
}

// Destructor.
template<typename T_DATA> INLINE
MultiDimBucket<T_DATA>::~MultiDimBucket()
{
  GUM_DESTRUCTOR( MultiDimBucket );
  if (__bucket != 0) delete __bucket;
}

// Add a gum::MultiDimImplementation in the bucket.
// @throw DuplicateElement Raised if impl is already in the bucket.
template<typename T_DATA> INLINE
void
MultiDimBucket<T_DATA>::add(const MultiDimImplementation<T_DATA>& impl)
{
  __multiDims.insert(&impl);
  __changed = true;
  if (! MultiDimImplementation<T_DATA>::_isInMultipleChangeMethod()) {
    for (MultiDimInterface::iterator iter = impl.begin(); iter != impl.end(); ++iter) {
      __addVariable(**iter);
    }
  }
}

// Remove a gum::MultiDimImplementation from this bucket.
template<typename T_DATA> INLINE
void
MultiDimBucket<T_DATA>::erase(const MultiDimImplementation<T_DATA>& impl)
{
  __multiDims.erase(&impl);
  __changed = true;
  if (! MultiDimImplementation<T_DATA>::_isInMultipleChangeMethod()) {
    for (MultiDimInterface::iterator iter = impl.begin(); iter != impl.end(); ++iter) {
      __eraseVariable(**iter);
    }
  }
}

// Returns true if the gum::MultiDimImplementation is in this bucket.
template<typename T_DATA> INLINE
bool
MultiDimBucket<T_DATA>::contains(const MultiDimImplementation<T_DATA>& impl) const
{
  return __multiDims.contains(&impl);
}

// Returns the number of gum::MultiDimImplementation in in this bukcet.
template<typename T_DATA> INLINE
Size
MultiDimBucket<T_DATA>::bucketSize() const
{
  return __multiDims.size();
}


// Returns true if this bucket is empty.
// Which mean thath there is no gum::MultiDimImplementation in this bucket.
template<typename T_DATA> INLINE
bool
MultiDimBucket<T_DATA>::isBucketEmpty() const
{
  return __multiDims.empty();
}

// Returns true if the bucket need re-computation since the last 
// computation.
template<typename T_DATA> INLINE
bool
MultiDimBucket<T_DATA>::bucketChanged() const
{
  return __changed;
}

// @brief This method computes the final table of this bucket.
// A flag is used to prevent unnecessary computation if the table has
// already been computed.
// If the size of the final table is above the amount of authorized memory,
// an gum::OperationNotAllowed is raised.
// Remember that this method is constant because the content of a
// multidimensional table is mutable.
// @param force If true (default set at false) then the final table is
//              re-computed.
// @throw OperationNotAllowed Raised if the size of the final table is above
//                            the authorized amount of memory.
template<typename T_DATA> INLINE
void
MultiDimBucket<T_DATA>::compute(bool force) const
{
  if ( (__bucket != 0) && (__changed || force) ) {
    Instantiation values(*__bucket);
    for (values.setFirst(); ! values.end(); values.inc()) {
      __bucket->set(values, __computeValue(values));
    }
    __changed = false;
  }
}

// See gum::MultiDimInterface::add().
template<typename T_DATA> INLINE
void
MultiDimBucket<T_DATA>::add (const DiscreteVariable &v)
{
  MultiDimImplementation<T_DATA>::add(v);
  if (! MultiDimImplementation<T_DATA>::_isInMultipleChangeMethod()) {
    if (this->domainSize() <= __bufferSize) {
      if (__bucket == 0) {
        __bucket = new MultiDimArray<T_DATA>();
        for (MultiDimInterface::iterator iter = this->begin(); iter != this->end(); ++iter) {
          __bucket->add(**iter);
        }
      } else {
        __bucket->add(v);
      }
    }
  }
}

// See gum::MultiDimInterface::erase().
template<typename T_DATA> INLINE
void
MultiDimBucket<T_DATA>::erase (const DiscreteVariable &v)
{
  MultiDimImplementation<T_DATA>::erase(v);
  if (! MultiDimImplementation<T_DATA>::_isInMultipleChangeMethod()) {
    if (this->domainSize() <= __bufferSize) {
      if (__bucket == 0) {
        __bucket = new MultiDimArray<T_DATA>();
        for (MultiDimInterface::iterator iter = this->begin(); iter != this->end(); ++iter) {
          __bucket->add(**iter);
        }
      } else {
        __bucket->erase(v);
      }
    }
  }
}

// See gum::MultiDimImplementation::realSize().
template<typename T_DATA> INLINE
Size
MultiDimBucket<T_DATA>::realSize() const
{
  compute();
  if (__bucket != 0) {
    return __bucket->realSize();
  } else {
    return (Size) 0;
  }
}

// See gum::MultiDimContainer::get().
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

// See gum::MultiDimContainer::operator[]().
template<typename T_DATA> INLINE
T_DATA
MultiDimBucket<T_DATA>::operator[] (const Instantiation &i) const
{
  return this->get(i);
}

// See gum::MultiDimAdressable::changeNotification().
template<typename T_DATA> INLINE
void
MultiDimBucket<T_DATA>::changeNotification (Instantiation &i,
    const DiscreteVariable *const var,
    const Idx &oldval, const Idx &newval)
{
  if (__bucket != 0) {
    __bucket->changeNotification(i, var, oldval, newval);
  }
}

// See gum::MultiDimAdressable::setFirstNotification().
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

// See gum::MultiDimAdressable::setLastNotification().
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

// See gum::MultiDimAdressable::setIncNotification().
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

// See gum::MultiDimAdressable::setDecNotification().
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

// See gum::MultiDimAdressable::setFirstNotification().
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

// See gum::MultiDimAdressable::registerSlave().
template<typename T_DATA> INLINE
bool
MultiDimBucket<T_DATA>::registerSlave (Instantiation &i)
{
  if (__bucket != 0) {
    return __bucket->registerSlave(i);
  } else {
    return this->registerSlave(i);
  }
}

// See gum::MultiDimAdressable::unregisterSlave().
template<typename T_DATA> INLINE
bool
MultiDimBucket<T_DATA>::unregisterSlave (Instantiation &i)
{
  if (__bucket != 0) {
    return __bucket->unregisterSlave(i);
  } else {
    __slavesValue.erase(&i);
    return this->unregisterSlave(i);
  }
}

// String representation of internal data about i in this.
template<typename T_DATA> INLINE
const std::string
MultiDimBucket<T_DATA>::toString(const Instantiation *i) const
{
  std::stringstream sBuff;
  sBuff << (*i) << " = " << get(*i);
  return sBuff.str();
}

// See gum::MultiDimImplementation::_commitMultipleChanges().
template<typename T_DATA> INLINE
void
MultiDimBucket<T_DATA>::_commitMultipleChanges()
{
  MultiDimImplementation<T_DATA>::_commitMultipleChanges();
  if (__bucket != 0) {
    delete __bucket;
    __bucket = 0;
  }
  if (this->domainSize() <= __bufferSize) {
    __bucket = new MultiDimArray<T_DATA>();
    for (MultiDimInterface::iterator iter = this->begin(); iter != this->end(); ++iter) {
      __bucket->add(**iter);
    }
  }
  __allVariables.clear();
  for (SetIterator< const MultiDimImplementation<T_DATA>* > iter = __multiDims.begin(); iter != __multiDims.end(); ++iter) {
    for (MultiDimInterface::iterator jter = (*iter)->begin(); jter != (*iter)->end(); ++jter) {
      __addVariable(*jter);
    }
  }
  __changed = true;
}

/// This will raise en exception, you should directly use the get() and
/// operator[]() methods.
template<typename T_DATA> INLINE
T_DATA&
MultiDimBucket<T_DATA>::_get(const Instantiation &i) const
{
  GUM_ERROR(OperationNotAllowed, "You should call this method in a MultiDimBucket.");
}

// Add a variable to __allVariables, and do nothing if var is already in
// the set.
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

// Erase a variable from __allVariables if no other multidimensional table
// uses it in this bucket.
template<typename T_DATA> INLINE
void
MultiDimBucket<T_DATA>::__eraseVariable(const DiscreteVariable* var)
{
  bool found = false;
  for (SetIterator< const MultiDimImplementation<T_DATA>* > iter = __multiDims.begin(); iter != __multiDims.end(); ++iter) {
    if ((*iter)->contains(*var)) {
      found = true;
      break;
    }
  }
  if (! found) {
    __allVariables.erase(var);
  }
}

// Compute the value of the final table of this bucket given i.
// If i variables are a subset of this bucket, then the missing values are
// supposed to be at 0.
template<typename T_DATA> INLINE
T_DATA
MultiDimBucket<T_DATA>::__computeValue(const Instantiation& value) const
{
  T_DATA sum = (T_DATA) 0;
  T_DATA current;
  Instantiation allVar_inst;
  for (Set<const DiscreteVariable*>::iterator iter = __allVariables.begin(); iter != __allVariables.end(); ++iter) {
    allVar_inst.add(**iter);
  }
  allVar_inst.chgValIn(value);
  for (allVar_inst.setFirstOut(value); ! allVar_inst.end(); allVar_inst.incOut(value)) {
    current = (T_DATA) 1;
    for (SetIterator< const MultiDimImplementation<T_DATA>* > iter = __multiDims.begin(); iter != __multiDims.end(); ++iter) {
      current *= (*iter)->get(allVar_inst);
    }
    sum += current;
  }
  return sum;
}

} /* namespace gum */
// ============================================================================
