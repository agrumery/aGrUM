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
 * @brief Implementation of the MultiDimArray class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <agrum/tools/multidim/implementations/multiDimArray.h>
#include <agrum/tools/multidim/implementations/multiDimWithOffset.h>

namespace gum {

  // Default constructor: creates an empty null dimensional matrix
  template < typename GUM_SCALAR >
  MultiDimArray< GUM_SCALAR >::MultiDimArray() : MultiDimWithOffset< GUM_SCALAR >() {
    // for debugging purposes
    GUM_CONSTRUCTOR(MultiDimArray);
  }

  // copy constructor
  template < typename GUM_SCALAR >
  MultiDimArray< GUM_SCALAR >::MultiDimArray(const MultiDimArray< GUM_SCALAR >& src) :
      MultiDimWithOffset< GUM_SCALAR >(src), values_(src.values_) {
    // for debugging purposes
    GUM_CONS_CPY(MultiDimArray);
  }

  // destructor
  template < typename GUM_SCALAR >
  MultiDimArray< GUM_SCALAR >::~MultiDimArray() {
    // for debugging purposes
    GUM_DESTRUCTOR(MultiDimArray);
    // no need to unregister all slaves as it will be done by MultiDimWithOffset
  }

  template < typename GUM_SCALAR >
  void MultiDimArray< GUM_SCALAR >::copyFrom(const MultiDimContainer< GUM_SCALAR >& src) const {
    auto mda = dynamic_cast< const MultiDimArray< GUM_SCALAR >* >(&src);

    if (mda == nullptr) {
      MultiDimContainer< GUM_SCALAR >::copyFrom(src);
    } else {
      values_ = mda->values_;
    }
  }

  template < typename GUM_SCALAR >
  void MultiDimArray< GUM_SCALAR >::apply(std::function< GUM_SCALAR(GUM_SCALAR) > f) const {
    std::transform(values_.begin(), values_.end(), values_.begin(), f);
  }

  template < typename GUM_SCALAR >
  GUM_SCALAR
     MultiDimArray< GUM_SCALAR >::reduce(std::function< GUM_SCALAR(GUM_SCALAR, GUM_SCALAR) > f,
                                         GUM_SCALAR base) const {
    return std::accumulate(values_.begin(), values_.end(), base, f);
  }


  // data access operator
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR& MultiDimArray< GUM_SCALAR >::get_(const Instantiation& i) const {
    if (i.isMaster(this)) {
      return values_[this->offsets_[&i]];
    } else {
      return values_[this->getOffs_(i)];
    }
  }

  // add a new dimension, needed for updating the offsets_ & gaps_
  template < typename GUM_SCALAR >
  INLINE void MultiDimArray< GUM_SCALAR >::add(const DiscreteVariable& v) {
    Size lg = MultiDimWithOffset< GUM_SCALAR >::domainSize();
    MultiDimWithOffset< GUM_SCALAR >::add(v);

    if (!this->isInMultipleChangeMethod_()) { values_.resize(lg * v.domainSize()); }
  }

  // removes a dimension, needed for updating the offsets_ & gaps_
  template < typename GUM_SCALAR >
  INLINE void MultiDimArray< GUM_SCALAR >::erase(const DiscreteVariable& v) {
    Sequence< const DiscreteVariable* > variables = this->variablesSequence();
    Idx                                 pos = variables.pos(&v);   // throw a NotFound if necessary

    if (variables.size() == 1) {
      if (!this->isInMultipleChangeMethod_()) values_.clear();
    } else {
      Size v_size = v.domainSize();
      Size size   = this->domainSize();
      // here, the variable does belong to the array.
      // => if pos = variables.size() - 1 then we just have to extract the
      // beginning of the array (actually the first gap of variable v)
      // if pos = 0, then copy every element whose index is a multiple of |v|
      // Assume now that pos != 0 and pos != variables.size() - 1, then
      // let w be the next variable in the set of variables of the array.
      // Then we must copy |gap(v)| elements every |gap(w)| elements

      if (!this->isInMultipleChangeMethod_()) {
        if (pos != variables.size() - 1) {
          Size gap_v = this->gaps_[variables[pos]];
          Size gap_w = this->gaps_[variables[pos + 1]];

          for (Idx i = 0, j = 0; i < size; i += gap_w) {
            Idx last = i + gap_v;

            for (Idx k = i; k < last; ++k, ++j)
              values_[j] = values_[k];
          }
        }

        // shrink values_
        values_.resize(size / v_size);
      }
    }

    MultiDimWithOffset< GUM_SCALAR >::erase(v);
  }

  template < typename GUM_SCALAR >
  INLINE Size MultiDimArray< GUM_SCALAR >::realSize() const {
    return this->domainSize();
  }

  // synchronise content after MultipleChanges
  template < typename GUM_SCALAR >
  INLINE void MultiDimArray< GUM_SCALAR >::commitMultipleChanges_() {
    if (MultiDimWithOffset< GUM_SCALAR >::domainSize() != values_.size()) {
      values_.resize(MultiDimWithOffset< GUM_SCALAR >::domainSize());
    }
  }

  // synchronise content after MultipleChanges
  template < typename GUM_SCALAR >
  INLINE void MultiDimArray< GUM_SCALAR >::commitMultipleChanges_(const GUM_SCALAR& x) {
    if (MultiDimWithOffset< GUM_SCALAR >::domainSize() != values_.size()) {
      values_.resize(MultiDimWithOffset< GUM_SCALAR >::domainSize(), x);
    }
  }

  // fill the array with the arg
  template < typename GUM_SCALAR >
  INLINE void MultiDimArray< GUM_SCALAR >::fill(const GUM_SCALAR& d) const {
    if (!this->empty()) std::fill(values_.begin(), values_.end(), d);
  }

  // virtual constructor
  template < typename GUM_SCALAR >
  INLINE MultiDimContainer< GUM_SCALAR >* MultiDimArray< GUM_SCALAR >::newFactory() const {
    return new MultiDimArray< GUM_SCALAR >;
  }

  // returns the element stored in the multidimArray at a given offset
  template < typename GUM_SCALAR >
  INLINE const GUM_SCALAR& MultiDimArray< GUM_SCALAR >::unsafeGet(Idx offset) const {
    return values_[offset];
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimArray< GUM_SCALAR >::unsafeSet(Idx offset, const GUM_SCALAR& val) {
    values_[offset] = val;
  }

  // returns the element stored in the multidimArray at a given offset
  template < typename GUM_SCALAR >
  INLINE const GUM_SCALAR& MultiDimArray< GUM_SCALAR >::getByOffset(Idx offset) const {
    if (offset >= values_.size()) { GUM_ERROR(OutOfBounds, "offset too large") }

    return values_[offset];
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimArray< GUM_SCALAR >::setByOffset(Idx offset, const GUM_SCALAR& data) {
    if (offset >= values_.size()) { GUM_ERROR(OutOfBounds, "offset too large") }

    values_[offset] = data;
  }

  // returns the name of the implementation
  template < typename GUM_SCALAR >
  const std::string& MultiDimArray< GUM_SCALAR >::name() const {
    // Here, this initialization is thread-safe due to Meyer’s Singleton property
    static const std::string str("MultiDimArray");
    return str;
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimArray< GUM_SCALAR >::replace_(const DiscreteVariable* x,
                                                    const DiscreteVariable* y) {
    MultiDimImplementation< GUM_SCALAR >::replace_(x, y);
  }
} /* namespace gum */
