/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
/** @file
 * @brief a class for sampling w.r.t. Dirichlet distributions
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /// returns a sample from the Dirichlet distribution
  template <class URNG>
  INLINE Dirichlet::result_type Dirichlet::
  operator()(URNG &generator, const Dirichlet::param_type &parm) {
    unsigned int size = parm.size();
    result_type res(size);
    float sum = 0.0f;
    while (sum == 0.0f) {
      for (unsigned int i = 0; i < size; ++i) {
        __gamma.param(std::gamma_distribution<float>::param_type(parm[i], 1));
        res[i] = __gamma(generator);
        sum += res[i];
      }
    }
    for (unsigned int i = 0; i < size; ++i) {
      res[i] /= sum;
    }
    return res;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
