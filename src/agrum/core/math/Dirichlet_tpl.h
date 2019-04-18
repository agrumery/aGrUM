
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief A class for sampling w.r.t. Dirichlet distributions.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

namespace gum {

  // returns a sample from the Dirichlet distribution
  template < class URNG >
  INLINE Dirichlet::result_type Dirichlet::
                                operator()(URNG& generator, const Dirichlet::param_type& parm) {
    Size        size = Size(parm.size());
    result_type res(size);
    float       sum = 0.0f;
    while (sum == 0.0f) {
      for (Idx i = 0; i < size; ++i) {
        __gamma.param(std::gamma_distribution< float >::param_type(parm[i], 1));
        res[i] = __gamma(generator);
        sum += res[i];
      }
    }
    for (Idx i = 0; i < size; ++i) {
      res[i] /= sum;
    }
    return res;
  }

} /* namespace gum */
