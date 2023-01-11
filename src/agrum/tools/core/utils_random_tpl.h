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

    std::vector< GUM_SCALAR > v(n);
    GUM_SCALAR                s;

    do {
      for (Idx i = 0; i < n; i++) {
        v[i] = (GUM_SCALAR)randomProba();
      }

      s = std::accumulate(v.begin(), v.end(), (GUM_SCALAR)0.0);

    } while (s < (GUM_SCALAR)(1e-5));

    for (Idx i = 0; i < n; i++) {
      v[i] /= s;
    }

    return v;
  }


  template < typename GENERATOR >
  INLINE Idx randomValue(GENERATOR& generator, const Size max) {
    std::uniform_int_distribution< Idx > uni_int(0, max - 1);
    return uni_int(generator);
  }


}   // namespace gum
