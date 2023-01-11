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
 * @brief Headers of the MultiDimWithOffset class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <limits>

// to ease IDE parsers...
#include <agrum/tools/multidim/implementations/multiDimImplementation.h>
#include <agrum/tools/multidim/implementations/multiDimWithOffset.h>

namespace gum {

  // Default constructor: creates an empty null dimensional matrix

  template < typename GUM_SCALAR >
  MultiDimWithOffset< GUM_SCALAR >::MultiDimWithOffset() : MultiDimImplementation< GUM_SCALAR >() {
    // for debugging purposes
    GUM_CONSTRUCTOR(MultiDimWithOffset);
  }

  // copy constructor

  template < typename GUM_SCALAR >
  MultiDimWithOffset< GUM_SCALAR >::MultiDimWithOffset(
     const MultiDimWithOffset< GUM_SCALAR >& from) :
      MultiDimImplementation< GUM_SCALAR >(from),
      gaps_(from.gaps_) {
    // for debugging purposes
    GUM_CONS_CPY(MultiDimWithOffset);
  }

  // destructor

  template < typename GUM_SCALAR >
  MultiDimWithOffset< GUM_SCALAR >::~MultiDimWithOffset() {
    // for debugging purposes
    GUM_DESTRUCTOR(MultiDimWithOffset);
    // no need to unregister all slaves as it will be done by
    // MultiDimImplementation
  }

  // add a new dimension, needed for updating the offsets_ & gaps_

  template < typename GUM_SCALAR >
  INLINE void MultiDimWithOffset< GUM_SCALAR >::add(const DiscreteVariable& v) {
    Size lg = this->domainSize();

    if (lg > std::numeric_limits< Idx >::max() / v.domainSize()) {
      GUM_ERROR(OutOfBounds, "Out of bounds !")
    }

    MultiDimImplementation< GUM_SCALAR >::add(v);
    gaps_.insert(&v, lg);
  }

  // removes a dimension, needed for updating the offsets_ & gaps_

  template < typename GUM_SCALAR >
  INLINE void MultiDimWithOffset< GUM_SCALAR >::erase(const DiscreteVariable& v) {
    Sequence< const DiscreteVariable* > variables = this->variablesSequence();
    Idx                                 pos = variables.pos(&v);   // throw a NotFound if necessary

    if (variables.size() == 1) {
      gaps_.clear();
    } else {
      // update the gaps_
      Size v_size = v.domainSize();
      gaps_.erase(variables[pos]);

      for (Idx i = pos + 1; i < variables.size(); ++i) {
        gaps_[variables[i]] /= v_size;
      }
    }

    MultiDimImplementation< GUM_SCALAR >::erase(v);
  }

  // listen to change in each recorded Instantiation.

  template < typename GUM_SCALAR >
  INLINE void
     MultiDimWithOffset< GUM_SCALAR >::changeNotification(const Instantiation&          i,
                                                          const DiscreteVariable* const var,
                                                          Idx                           oldval,
                                                          Idx                           newval) {
    GUM_ASSERT(offsets_.exists(&i));
    GUM_ASSERT(offsets_[&i] < this->domainSize());
    GUM_ASSERT(newval < var->domainSize());
    GUM_ASSERT(oldval < var->domainSize());

    if (newval >= oldval) {
      offsets_[&i] += gaps_[var] * (newval - oldval);
      GUM_ASSERT(offsets_[&i] < this->domainSize());
    } else {
      GUM_ASSERT(offsets_[&i] >= gaps_[var] * (oldval - newval));
      offsets_[&i] -= gaps_[var] * (oldval - newval);
    }
  }

  // listen to an assignment of a value in a Instantiation

  template < typename GUM_SCALAR >
  INLINE void MultiDimWithOffset< GUM_SCALAR >::setChangeNotification(const Instantiation& i) {
    GUM_ASSERT(offsets_.exists(&i));
    offsets_[&i] = getOffs_(i);
  }

  // listen to setFirst in each recorded Instantiation.

  template < typename GUM_SCALAR >
  INLINE void MultiDimWithOffset< GUM_SCALAR >::setFirstNotification(const Instantiation& i) {
    GUM_ASSERT(offsets_.exists(&i));
    offsets_[&i] = 0;
  }

  // listen to setLast in each recorded Instantiation.

  template < typename GUM_SCALAR >
  INLINE void MultiDimWithOffset< GUM_SCALAR >::setLastNotification(const Instantiation& i) {
    GUM_ASSERT(offsets_.exists(&i));
    offsets_[&i] = this->domainSize() - 1;
  }

  // listen to increment in each recorded Instantiation.

  template < typename GUM_SCALAR >
  INLINE void MultiDimWithOffset< GUM_SCALAR >::setIncNotification(const Instantiation& i) {
    GUM_ASSERT(offsets_.exists(&i));
    GUM_ASSERT(offsets_[&i] != this->domainSize() - 1);
    ++offsets_[&i];
  }

  // listen to increment in each recorded Instantiation.

  template < typename GUM_SCALAR >
  INLINE void MultiDimWithOffset< GUM_SCALAR >::setDecNotification(const Instantiation& i) {
    GUM_ASSERT(offsets_.exists(&i));
    GUM_ASSERT(offsets_[&i] != 0);
    --offsets_[&i];
  }

  // add a Instantiation as a slave

  template < typename GUM_SCALAR >
  INLINE bool MultiDimWithOffset< GUM_SCALAR >::registerSlave(Instantiation& i) {
    if (MultiDimImplementation< GUM_SCALAR >::registerSlave(i)) {
      GUM_ASSERT(!offsets_.exists(&i));
      offsets_.insert(&i, getOffs_(i));
      return true;
    }

    return false;
  }

  // remove a registered slave instantiation

  template < typename GUM_SCALAR >
  INLINE bool MultiDimWithOffset< GUM_SCALAR >::unregisterSlave(Instantiation& i) {
    MultiDimImplementation< GUM_SCALAR >::unregisterSlave(i);
    offsets_.erase(&i);
    return true;
  }

  // Compute the offset of a Instantiation
  /** If the instantiation is not fully compatible with the MultiDimWithOffset,
   * no exception thrown
   * but 0 is assumed for dimensions not present in the instantiation.
   * for instance : M<<a<<b<<c; with i=b:1|c:2|d:1 then M.getOffs_(i) give the
   * offset of a:0|b:1|c:2.
   */

  template < typename GUM_SCALAR >
  INLINE Size MultiDimWithOffset< GUM_SCALAR >::getOffs_(const Instantiation& i) const {
    Idx off = 0;

    for (auto iter = gaps_.begin(); iter != gaps_.end(); ++iter)
      if (i.contains(iter.key())) off += iter.val() * i.valFromPtr(iter.key());
      else
        GUM_ERROR(InvalidArgument, iter.key()->name() << " not present in the instantiation " << i)

    return off;
  }

  /*  template < typename GUM_SCALAR >
    INLINE Size MultiDimWithOffset< GUM_SCALAR >::getOffs_(const Instantiation& i) const {
      Idx off = 0;

      for (HashTableConstIteratorSafe< const DiscreteVariable*, Size > iter = gaps_.beginSafe();
           iter != gaps_.endSafe();
           ++iter)
        if (i.contains(iter.key()))
          off += iter.val() * i.valFromPtr(iter.key());
        else
          GUM_ERROR(InvalidArgument, iter.key()->name() << " not present in the instantiation " <<
    i)

      return off;
    }*/

  // For a given indice of a value in the vector values_, this method computes
  // the corresponding instantiation
  /**
   * @param result the result of this methods, we assume that the given
   * instantiation already contains all the variables
   *               contained in the multidimarray (if V is the set of variables
   *               of this tab, V must be a subset of variables in
   *               result or the exact set)
   * @param indice indice in the vector values_
   */

  template < typename GUM_SCALAR >
  INLINE void MultiDimWithOffset< GUM_SCALAR >::computeInstantiationValue_(Instantiation& result,
                                                                           Size indice) const {
    for (Idx i = 0; i < this->nbrDim(); ++i) {
      const DiscreteVariable& var        = this->variable(i);
      Idx                     domainSize = var.domainSize();
      result.chgVal(var, indice % domainSize);
      indice = indice / domainSize;
    }

    GUM_ASSERT(indice == 0);
  }

  // string representation of internal data about i in this.
  template < typename GUM_SCALAR >
  INLINE std::string MultiDimWithOffset< GUM_SCALAR >::toString(const Instantiation* i) const {
    if (i->isMaster(this)) {
      std::stringstream s;
      s << offsets_[i];
      std::string res;
      s >> res;
      return res;
    } else {
      return "--";
    }
  }

  template < typename GUM_SCALAR >
  INLINE Size MultiDimWithOffset< GUM_SCALAR >::toOffset(const Instantiation& i) const {
    return getOffs_(i);
  }

  // set the Instantiation to the values corresponding to the offset (in this
  // array)
  template < typename GUM_SCALAR >
  INLINE Instantiation& MultiDimWithOffset< GUM_SCALAR >::fromOffset(Instantiation& i,
                                                                     Size           offset) const {
    this->computeInstantiationValue_(i, offset);
    return i;
  }

} /* namespace gum */
