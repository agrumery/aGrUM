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

// to ease IDE parsers...
#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/multidim/multiDimWithOffset.h>

namespace gum {

  /// Default constructor: creates an empty null dimensional matrix

  template <typename GUM_SCALAR>
  MultiDimWithOffset<GUM_SCALAR>::MultiDimWithOffset()
      : MultiDimImplementation<GUM_SCALAR>() {
    // for debugging purposes
    GUM_CONSTRUCTOR(MultiDimWithOffset);
  }

  /// copy constructor

  template <typename GUM_SCALAR>
  MultiDimWithOffset<GUM_SCALAR>::MultiDimWithOffset(
      const MultiDimWithOffset<GUM_SCALAR> &from)
      : MultiDimImplementation<GUM_SCALAR>(from), _gaps(from._gaps) {
    // for debugging purposes
    GUM_CONS_CPY(MultiDimWithOffset);
  }

  /// destructor

  template <typename GUM_SCALAR>
  MultiDimWithOffset<GUM_SCALAR>::~MultiDimWithOffset() {
    // for debugging purposes
    GUM_DESTRUCTOR(MultiDimWithOffset);
    // no need to unregister all slaves as it will be done by MultiDimImplementation
  }

  /// add a new dimension, needed for updating the _offsets & _gaps

  template <typename GUM_SCALAR>
  INLINE void MultiDimWithOffset<GUM_SCALAR>::add(const DiscreteVariable &v) {
    Size lg = this->domainSize();

    if (lg > GUM_MAX_SIZE / v.domainSize()) {
      GUM_ERROR(OutOfBounds, "Out of bounds !");
    }

    MultiDimImplementation<GUM_SCALAR>::add(v);
    _gaps.insert(&v, lg);
  }

  /// removes a dimension, needed for updating the _offsets & _gaps

  template <typename GUM_SCALAR>
  INLINE void MultiDimWithOffset<GUM_SCALAR>::erase(const DiscreteVariable &v) {
    Sequence<const DiscreteVariable *> variables = this->variablesSequence();
    Idx pos = variables.pos(&v); // throw a NotFound if necessary

    if (variables.size() == 1) {
      _gaps.clear();
    } else {
      // update the _gaps
      Size v_size = v.domainSize();
      _gaps.erase(variables[pos]);

      for (Idx i = pos + 1; i < variables.size(); ++i) {
        _gaps[variables[i]] /= v_size;
      }
    }

    MultiDimImplementation<GUM_SCALAR>::erase(v);
  }

  /// listen to change in each recorded Instantiation.

  template <typename GUM_SCALAR>
  INLINE void MultiDimWithOffset<GUM_SCALAR>::changeNotification(
      Instantiation &i, const DiscreteVariable *const var, const Idx &oldval,
      const Idx &newval) {
    GUM_ASSERT(_offsets.exists(&i));
    GUM_ASSERT(_offsets[&i] < this->domainSize());
    GUM_ASSERT(newval < var->domainSize());
    GUM_ASSERT(oldval < var->domainSize());

    if (newval >= oldval) {
      _offsets[&i] += _gaps[var] * (newval - oldval);
      GUM_ASSERT(_offsets[&i] < this->domainSize());
    } else {
      GUM_ASSERT(_offsets[&i] >= _gaps[var] * (oldval - newval));
      _offsets[&i] -= _gaps[var] * (oldval - newval);
    }
  }

  /// listen to an assignment of a value in a Instantiation

  template <typename GUM_SCALAR>
  INLINE void
  MultiDimWithOffset<GUM_SCALAR>::setChangeNotification(Instantiation &i) {
    GUM_ASSERT(_offsets.exists(&i));
    _offsets[&i] = _getOffs(i);
  }

  /// listen to setFirst in each recorded Instantiation.

  template <typename GUM_SCALAR>
  INLINE void
  MultiDimWithOffset<GUM_SCALAR>::setFirstNotification(Instantiation &i) {
    GUM_ASSERT(_offsets.exists(&i));
    _offsets[&i] = 0;
  }

  /// listen to setLast in each recorded Instantiation.

  template <typename GUM_SCALAR>
  INLINE void MultiDimWithOffset<GUM_SCALAR>::setLastNotification(Instantiation &i) {
    GUM_ASSERT(_offsets.exists(&i));
    _offsets[&i] = this->domainSize() - 1;
  }

  /// listen to increment in each recorded Instantiation.

  template <typename GUM_SCALAR>
  INLINE void MultiDimWithOffset<GUM_SCALAR>::setIncNotification(Instantiation &i) {
    GUM_ASSERT(_offsets.exists(&i));
    GUM_ASSERT(_offsets[&i] != this->domainSize() - 1);
    ++_offsets[&i];
  }

  /// listen to increment in each recorded Instantiation.

  template <typename GUM_SCALAR>
  INLINE void MultiDimWithOffset<GUM_SCALAR>::setDecNotification(Instantiation &i) {
    GUM_ASSERT(_offsets.exists(&i));
    GUM_ASSERT(_offsets[&i] != 0);
    --_offsets[&i];
  }

  /// add a Instantiation as a slave

  template <typename GUM_SCALAR>
  INLINE bool MultiDimWithOffset<GUM_SCALAR>::registerSlave(Instantiation &i) {
    if (MultiDimImplementation<GUM_SCALAR>::registerSlave(i)) {
      GUM_ASSERT(!_offsets.exists(&i));
      _offsets.insert(&i, _getOffs(i));
      return true;
    }

    return false;
  }

  /// remove a registered slave instantiation

  template <typename GUM_SCALAR>
  INLINE bool MultiDimWithOffset<GUM_SCALAR>::unregisterSlave(Instantiation &i) {
    MultiDimImplementation<GUM_SCALAR>::unregisterSlave(i);
    _offsets.erase(&i);
    return true;
  }

  /// Compute the offset of a Instantiation
  /** If the instantiation is not fully compatible with the MultiDimWithOffset,
   * no exception thrown
   * but 0 is assumed for dimensions not present in the instantiation.
   * for instance : M<<a<<b<<c; with i=b:1|c:2|d:1 then M._getOffs(i) give the
   * offset of a:0|b:1|c:2.
   */

  template <typename GUM_SCALAR>
  INLINE Size
      MultiDimWithOffset<GUM_SCALAR>::_getOffs(const Instantiation &i) const {
    Idx off = 0;

    for (HashTableConstIteratorSafe<const DiscreteVariable *, Size> iter =
             _gaps.beginSafe();
         iter != _gaps.endSafe(); ++iter)
      if (i.contains(iter.key()))
        off += iter.val() * i.valFromPtr(iter.key());

    return off;
  }

  /// For a given indice of a value in the vector _values, this method computes
  /// the corresponding instantiation
  /**
   * @param result the result of this methods, we assume that the given
   * instantiation already contains all the variables
   *               contained in the multidimarray (if V is the set of variables
   *               of this tab, V must be a subset of variables in
   *               result or the exact set)
   * @param indice indice in the vector _values
   */

  template <typename GUM_SCALAR>
  INLINE void
  MultiDimWithOffset<GUM_SCALAR>::_computeInstantiationValue(Instantiation &result,
                                                             Size indice) const {
    for (Idx i = 0; i < this->nbrDim(); ++i) {
      const DiscreteVariable &var = this->variable(i);
      Idx domainSize = var.domainSize();
      result.chgVal(var, indice % domainSize);
      indice = indice / domainSize;
    }

    GUM_ASSERT(indice == 0);
  }

  /// string representation of internal data about i in this.
  template <typename GUM_SCALAR>
  INLINE const std::string
  MultiDimWithOffset<GUM_SCALAR>::toString(const Instantiation *i) const {
    if (i->isMaster(this)) {
      std::stringstream s;
      s << _offsets[i];
      std::string res;
      s >> res;
      return res;
    } else {
      return "--";
    }
  }

  template <typename GUM_SCALAR>
  INLINE Size
      MultiDimWithOffset<GUM_SCALAR>::toOffset(const Instantiation &i) const {
    return _getOffs(i);
  }

  /// set the Instantiation to the values corresponding to the offset (in this array)
  template <typename GUM_SCALAR>
  INLINE Instantiation &
  MultiDimWithOffset<GUM_SCALAR>::fromOffset(Instantiation &i, Size offset) const {
    this->_computeInstantiationValue(i, offset);
    return i;
  }

} /* namespace gum */
