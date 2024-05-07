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
 * @brief Contains useful methods for random stuff.
 *
 * @author Vincent RENAUDINEAU and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

namespace gum {

  // return a random discrete distribution
  // @param n is the number of modalities for the distribution
  template < typename GUM_SCALAR >
  std::vector< GUM_SCALAR > randomDistribution(Size n) {
    if (n < 2) n = 2;

    // using sort method for uniformly distributed distributions
    // https://doi.org/10.1016/0377-2217(82)90161-8
    std::vector< GUM_SCALAR > v(n + 1);
    std::vector< GUM_SCALAR > res(n);
    v[0] = (GUM_SCALAR)0.0;
    v[n] = (GUM_SCALAR)1.0;
    for (Idx i = 1; i < n; i++) {
      v[i] = (GUM_SCALAR)randomProba();
    }
    std::sort(v.begin(), v.end());

    for (Idx i = 0; i < n; i++) {
      res[i] = v[i + 1] - v[i];
    }

    return res;
  }

  template < typename GENERATOR >
  INLINE Idx randomValue(GENERATOR& generator, const Size max) {
    std::uniform_int_distribution< Idx > uni_int(0, max - 1);
    return uni_int(generator);
  }


}   // namespace gum
