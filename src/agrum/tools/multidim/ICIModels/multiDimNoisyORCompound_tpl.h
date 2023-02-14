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


/** @file
 * @brief ANoisy-OR as described by Henrion (UAI-3, 1989, pp161-173)
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *<info_at_agrum_dot_org>
 */
#include <agrum/tools/multidim/ICIModels/multiDimNoisyORCompound.h>

namespace gum {

  /// Default constructor

  template < typename GUM_SCALAR >
  INLINE MultiDimNoisyORCompound< GUM_SCALAR >::MultiDimNoisyORCompound(GUM_SCALAR external_weight,
                                                                        GUM_SCALAR default_weight) :
      MultiDimICIModel< GUM_SCALAR >(external_weight, default_weight) {
    GUM_CONSTRUCTOR(MultiDimNoisyORCompound);
  }

  /// Default constructor

  template < typename GUM_SCALAR >
  INLINE MultiDimNoisyORCompound< GUM_SCALAR >::MultiDimNoisyORCompound(
     const MultiDimNoisyORCompound< GUM_SCALAR >& from) :
      MultiDimICIModel< GUM_SCALAR >(from) {
    GUM_CONS_CPY(MultiDimNoisyORCompound);
  }

  /// Copy constructor using a bijection to swap variables from source.

  template < typename GUM_SCALAR >
  INLINE MultiDimNoisyORCompound< GUM_SCALAR >::MultiDimNoisyORCompound(
     const Bijection< const DiscreteVariable*, const DiscreteVariable* >& bij,
     const MultiDimNoisyORCompound< GUM_SCALAR >&                         from) :
      MultiDimICIModel< GUM_SCALAR >(bij, from) {
    GUM_CONSTRUCTOR(MultiDimNoisyORCompound);
  }

  /// destructor

  template < typename GUM_SCALAR >
  INLINE MultiDimNoisyORCompound< GUM_SCALAR >::~MultiDimNoisyORCompound() {
    GUM_DESTRUCTOR(MultiDimNoisyORCompound);
  }

  template < typename GUM_SCALAR >
  GUM_SCALAR MultiDimNoisyORCompound< GUM_SCALAR >::get(const Instantiation& i) const {
    if (this->nbrDim() < 1) { GUM_ERROR(OperationNotAllowed, "Not enough variable for a NoisyOr ") }

    const DiscreteVariable& C = this->variable((Idx)0);

    if (i.val(C) > 1) return (GUM_SCALAR)0.0;

    GUM_SCALAR ratio = (GUM_SCALAR)1.0 - this->externalWeight();

    GUM_SCALAR fact = (GUM_SCALAR)ratio;

    if (fact != (GUM_SCALAR)0) {
      for (Idx j = 1; j < this->nbrDim(); j++) {
        const DiscreteVariable& v = this->variable(j);

        if (i.val(v) == 1) {
          GUM_SCALAR pr = (1 - this->causalWeight(v)) / ratio;

          if (pr == (GUM_SCALAR)0.0) {
            fact = (GUM_SCALAR)0.0;
            break;
          } else {
            fact *= pr;
          }
        }
      }
    }

    return (i.val(C) != 1) ? fact : (GUM_SCALAR)1.0 - fact;
  }

  template < typename GUM_SCALAR >
  std::string MultiDimNoisyORCompound< GUM_SCALAR >::toString() const {
    std::stringstream s;
    s << MultiDimImplementation< GUM_SCALAR >::variable(0) << "=noisyORCompound(["
      << this->externalWeight() << "]";

    for (Idx i = 1; i < MultiDimImplementation< GUM_SCALAR >::nbrDim(); i++)
      s << ", " << MultiDimImplementation< GUM_SCALAR >::variable(i) << "["
        << this->causalWeight(MultiDimImplementation< GUM_SCALAR >::variable(i)) << "]";

    s << ")";
    return s.str();
  }

  // For friendly displaying the content of the variable.

  template < typename GUM_SCALAR >
  INLINE std::ostream& operator<<(std::ostream&                                s,
                                  const MultiDimNoisyORCompound< GUM_SCALAR >& ag) {
    return s << ag.toString();
  }

  template < typename GUM_SCALAR >
  INLINE MultiDimContainer< GUM_SCALAR >*
         MultiDimNoisyORCompound< GUM_SCALAR >::newFactory() const {
    return new MultiDimNoisyORCompound< GUM_SCALAR >(this->_external_weight_,
                                                     this->_default_weight_);
  }

  // returns the name of the implementation
  template < typename GUM_SCALAR >
  INLINE const std::string& MultiDimNoisyORCompound< GUM_SCALAR >::name() const {
    static const std::string str = "MultiDimNoisyORCompound";
    return str;
  }

  // ==================================================
} /* namespace gum */
