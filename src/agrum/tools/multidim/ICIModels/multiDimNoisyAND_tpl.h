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
 * @brief class for NoisyAND-net implementation as multiDim
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <agrum/tools/core/exceptions.h>
#include <agrum/tools/multidim/ICIModels/multiDimNoisyAND.h>

namespace gum {

  // Default constructor
  template < typename GUM_SCALAR >
  INLINE MultiDimNoisyAND< GUM_SCALAR >::MultiDimNoisyAND(GUM_SCALAR external_weight,
                                                          GUM_SCALAR default_weight) :
      MultiDimICIModel< GUM_SCALAR >(external_weight, default_weight) {
    if (external_weight == 0) {
      GUM_ERROR(InvalidArgument, "external weight can not be null for a NoisyAND")
    }

    GUM_CONSTRUCTOR(MultiDimNoisyAND);
  }

  // Default constructor
  template < typename GUM_SCALAR >
  INLINE
     MultiDimNoisyAND< GUM_SCALAR >::MultiDimNoisyAND(const MultiDimNoisyAND< GUM_SCALAR >& from) :
      MultiDimICIModel< GUM_SCALAR >(from) {
    GUM_CONS_CPY(MultiDimNoisyAND);
  }

  // Copy constructor using a bijection to replace variables from source.
  template < typename GUM_SCALAR >
  INLINE MultiDimNoisyAND< GUM_SCALAR >::MultiDimNoisyAND(
     const Bijection< const DiscreteVariable*, const DiscreteVariable* >& bij,
     const MultiDimNoisyAND< GUM_SCALAR >&                                from) :
      MultiDimICIModel< GUM_SCALAR >(bij, from) {
    GUM_CONSTRUCTOR(MultiDimNoisyAND);
  }

  // destructor
  template < typename GUM_SCALAR >
  INLINE MultiDimNoisyAND< GUM_SCALAR >::~MultiDimNoisyAND() {
    GUM_DESTRUCTOR(MultiDimNoisyAND);
  }

  template < typename GUM_SCALAR >
  GUM_SCALAR MultiDimNoisyAND< GUM_SCALAR >::get(const Instantiation& i) const {
    if (this->nbrDim() < 1) {
      GUM_ERROR(OperationNotAllowed, "Not enough variable for a NoisyAND ")
    }

    const DiscreteVariable& C = this->variable((Idx)0);

    if (i.val(C) > 1) return (GUM_SCALAR)0.0;

    GUM_SCALAR fact = this->externalWeight();

    for (Idx j = 1; j < this->nbrDim(); j++) {
      const DiscreteVariable& v = this->variable(j);

      if (i.val(v) == 0) {
        fact *= ((GUM_SCALAR)1.0 - this->causalWeight(v));
      } else {
        fact *= this->causalWeight(v);
      }
    }

    return (i.val(C) == 1) ? fact : (GUM_SCALAR)1.0 - fact;
  }

  template < typename GUM_SCALAR >
  std::string MultiDimNoisyAND< GUM_SCALAR >::toString() const {
    std::stringstream s;
    s << MultiDimImplementation< GUM_SCALAR >::variable(0) << "=noisyAND(["
      << this->externalWeight() << "]";

    for (Idx i = 1; i < MultiDimImplementation< GUM_SCALAR >::nbrDim(); i++)
      s << ", " << MultiDimImplementation< GUM_SCALAR >::variable(i) << "["
        << this->causalWeight(MultiDimImplementation< GUM_SCALAR >::variable(i)) << "]";

    s << ")";
    return s.str();
  }

  // For friendly displaying the content of the variable.

  template < typename GUM_SCALAR >
  INLINE std::ostream& operator<<(std::ostream& s, const MultiDimNoisyAND< GUM_SCALAR >& ag) {
    return s << ag.toString();
  }

  template < typename GUM_SCALAR >
  INLINE MultiDimContainer< GUM_SCALAR >* MultiDimNoisyAND< GUM_SCALAR >::newFactory() const {
    return new MultiDimNoisyAND< GUM_SCALAR >(this->_external_weight_, this->_default_weight_);
  }

  // returns the name of the implementation
  template < typename GUM_SCALAR >
  INLINE const std::string& MultiDimNoisyAND< GUM_SCALAR >::name() const {
    static const std::string str = "MultiDimNoisyAND";
    return str;
  }

} /* namespace gum */
