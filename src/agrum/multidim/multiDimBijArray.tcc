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
#include <agrum/multidim/multiDimBijArray.h>

namespace gum {

  template <typename GUM_SCALAR>
  MultiDimBijArray<GUM_SCALAR>::MultiDimBijArray(
      const MultiDimBijArray<GUM_SCALAR> &from)
      : MultiDimWithOffset<GUM_SCALAR>(), __array(from.__array),
        __name(from.__name) {
    GUM_CONS_CPY(MultiDimBijArray);

    for (MultiDimInterface::iterator iter = from.begin(); iter != from.end();
         ++iter) {
      MultiDimWithOffset<GUM_SCALAR>::add(**iter);
    }
  }

  template <typename GUM_SCALAR>
  MultiDimBijArray<GUM_SCALAR>::MultiDimBijArray(
      const VarBijection &bijection, const MultiDimArray<GUM_SCALAR> &array)
      : MultiDimWithOffset<GUM_SCALAR>(), __array(array),
        __name("MultiDimBijArray") {
    GUM_CONSTRUCTOR(MultiDimBijArray);

    for (MultiDimInterface::iterator iter = array.begin(); iter != array.end();
         ++iter) {
      MultiDimWithOffset<GUM_SCALAR>::add(*(bijection.second(*iter)));
    }
  }

  template <typename GUM_SCALAR>
  MultiDimBijArray<GUM_SCALAR>::MultiDimBijArray(
      const VarBijection &bijection, const MultiDimBijArray<GUM_SCALAR> &array)
      : MultiDimWithOffset<GUM_SCALAR>(), __array(array.__array),
        __name("MultiDimBijArray") {
    GUM_CONSTRUCTOR(MultiDimBijArray);

    for (MultiDimInterface::iterator iter = array.begin(); iter != array.end();
         ++iter) {
      MultiDimWithOffset<GUM_SCALAR>::add(*(bijection.second(*iter)));
    }
  }

  template <typename GUM_SCALAR>
  INLINE MultiDimBijArray<GUM_SCALAR>::~MultiDimBijArray() {
    GUM_DESTRUCTOR(MultiDimBijArray);
  }

  template <typename GUM_SCALAR>
  MultiDimBijArray<GUM_SCALAR> &MultiDimBijArray<GUM_SCALAR>::
  operator=(const MultiDimBijArray<GUM_SCALAR> &from) {
    GUM_ERROR(OperationNotAllowed, "MultiDimBijArray are readonly.");
  }

  template <typename GUM_SCALAR>
  INLINE MultiDimBijArray<GUM_SCALAR> *
  MultiDimBijArray<GUM_SCALAR>::newFactory() const {
    return new MultiDimBijArray<GUM_SCALAR>(*this);
  }

  template <typename GUM_SCALAR>
  INLINE const std::string &MultiDimBijArray<GUM_SCALAR>::name() const {
    return __name;
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimBijArray<GUM_SCALAR>::add(const DiscreteVariable &v) {
    GUM_ERROR(OperationNotAllowed, "MultiDimBijArray<GUM_SCALAR> are read only.");
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimBijArray<GUM_SCALAR>::erase(const DiscreteVariable &v) {
    GUM_ERROR(OperationNotAllowed, "MultiDimBijArray<GUM_SCALAR> are read only.");
  }

  template <typename GUM_SCALAR>
  INLINE Size MultiDimBijArray<GUM_SCALAR>::realSize() const {
    return (Size)0;
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimBijArray<GUM_SCALAR>::fill(const GUM_SCALAR &d) const {
    GUM_ERROR(OperationNotAllowed, "MultiDimBijArray<GUM_SCALAR> are read only.");
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimBijArray<GUM_SCALAR>::_commitMultipleChanges(void) {
    // Do nothing
  }

  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR MultiDimBijArray<GUM_SCALAR>::get(const Instantiation &i) const {
    if (i.isMaster(this)) {
      return __array._values[this->_offsets[&i]];
    } else {
      return __array._values[this->_getOffs(i)];
    }
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimBijArray<GUM_SCALAR>::set(const Instantiation &i,
                                                const GUM_SCALAR &value) const {
    GUM_ERROR(OperationNotAllowed, "MultiDimBijArray<GUM_SCALAR> are read only.");
  }

  template <typename GUM_SCALAR>
  INLINE void
  MultiDimBijArray<GUM_SCALAR>::fillWith(const std::vector<GUM_SCALAR> &v) const {
    GUM_ERROR(OperationNotAllowed, "MultiDimBijArray<GUM_SCALAR> are read only.");
  }

  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR &
  MultiDimBijArray<GUM_SCALAR>::_get(const Instantiation &i) const {
    GUM_ERROR(OperationNotAllowed, "MultiDimBijArray<GUM_SCALAR> are read only.");
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimBijArray<GUM_SCALAR>::_swap(const DiscreteVariable *x,
                                                  const DiscreteVariable *y) {
    MultiDimImplementation<GUM_SCALAR>::_swap(x, y);
  }

} // namespace gum
