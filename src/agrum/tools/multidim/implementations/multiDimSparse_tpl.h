
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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
 * @brief Implementation of MultiDimSparse.
 *
 * @author Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
 */

namespace gum {

  // Default constructor: creates an empty null dimensional matrix

  template < typename GUM_SCALAR >
  MultiDimSparse< GUM_SCALAR >::MultiDimSparse(const GUM_SCALAR& default_value) :
      MultiDimWithOffset< GUM_SCALAR >(), _default(default_value) {
    // for debugging purposes
    GUM_CONSTRUCTOR(MultiDimSparse);
  }

  // copy constructor

  template < typename GUM_SCALAR >
  MultiDimSparse< GUM_SCALAR >::MultiDimSparse(
     const MultiDimSparse< GUM_SCALAR >& from) :
      MultiDimWithOffset< GUM_SCALAR >(from),
      _params(from._params), _default(from._default) {
    // for debugging purposes
    GUM_CONS_CPY(MultiDimSparse);
  }

  // destructor

  template < typename GUM_SCALAR >
  MultiDimSparse< GUM_SCALAR >::~MultiDimSparse() {
    // for debugging purposes
    GUM_DESTRUCTOR(MultiDimSparse);
    // no need to unregister all slaves as it will be done by MultiDimWithOffset
  }

  // data access operator

  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR
     MultiDimSparse< GUM_SCALAR >::get(const Instantiation& i) const {
    Size key;

    if (i.isMaster(this)) {
      key = this->_offsets[&i];
    } else {
      key = this->_getOffs(i);
    }

    return _params.exists(key) ? _params[key] : _default;
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimSparse< GUM_SCALAR >::set(const Instantiation& i,
                                                const GUM_SCALAR&    value) const {
    Size key;

    if (i.isMaster(this)) {
      key = this->_offsets[&i];
    } else {
      key = this->_getOffs(i);
    }

    if (value == _default) {
      _params.reset(key);
    } else {
      _params.set(key, value);
    }
  }

  // add a new dimension, needed for updating the _offsets & _gaps

  template < typename GUM_SCALAR >
  INLINE void MultiDimSparse< GUM_SCALAR >::add(const DiscreteVariable& v) {
    MultiDimWithOffset< GUM_SCALAR >::add(v);

    //@todo : anything better than that !
    fill(_default);
  }

  // removes a dimension, needed for updating the _offsets & _gaps

  template < typename GUM_SCALAR >
  INLINE void MultiDimSparse< GUM_SCALAR >::erase(const DiscreteVariable& v) {
    MultiDimWithOffset< GUM_SCALAR >::erase(v);

    //@todo : anything better than that !
    fill(_default);
  }

  // synchronise content after MultipleChanges
  template < typename GUM_SCALAR >
  INLINE void MultiDimSparse< GUM_SCALAR >::_commitMultipleChanges() {
    //@todo : anything better than that !
    fill(_default);
  }

  // fill the array with the arg
  template < typename GUM_SCALAR >
  INLINE void MultiDimSparse< GUM_SCALAR >::fill(const GUM_SCALAR& d) const {
    _params.clear();
    _default = d;
  }

  template < typename GUM_SCALAR >
  INLINE Size MultiDimSparse< GUM_SCALAR >::realSize() const {
    return _params.size();
  }

  template < typename GUM_SCALAR >
  INLINE MultiDimContainer< GUM_SCALAR >*
         MultiDimSparse< GUM_SCALAR >::newFactory() const {
    return new MultiDimSparse< GUM_SCALAR >(_default);
  }

  // returns the name of the implementation
  template < typename GUM_SCALAR >
  INLINE const std::string& MultiDimSparse< GUM_SCALAR >::name() const {
    static const std::string str = "MultiDimSparse";
    return str;
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimSparse< GUM_SCALAR >::_replace(const DiscreteVariable* x,
                                                     const DiscreteVariable* y) {
    MultiDimImplementation< GUM_SCALAR >::_replace(x, y);
  }

  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR&
         MultiDimSparse< GUM_SCALAR >::_get(const Instantiation& i) const {
    GUM_ERROR(OperationNotAllowed,
              "Do not use this with the MultiDimSparse class.");
  }

} /* namespace gum */