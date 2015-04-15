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

#include <agrum/core/utils_random.h>

namespace gum {

  /// default constructor
  INLINE Dirichlet::Dirichlet(const param_type &params, unsigned int seed)
      : __generator(seed ? seed : gum::randomGeneratorSeed()), __params(params) {
    GUM_CONSTRUCTOR(Dirichlet);
  }

  /// copy constructor
  INLINE Dirichlet::Dirichlet(const Dirichlet &from)
      : __generator(from.__generator), __gamma(from.__gamma),
        __params(from.__params) {
    GUM_CONS_CPY(Dirichlet);
  }

  /// move constructor
  INLINE Dirichlet::Dirichlet(Dirichlet &&from)
      : __generator(std::move(from.__generator)), __gamma(std::move(from.__gamma)),
        __params(std::move(from.__params)) {
    GUM_CONS_MOV(Dirichlet);
  }

  /// destructor
  INLINE Dirichlet::~Dirichlet() { GUM_DESTRUCTOR(Dirichlet); }

  /// copy operator
  INLINE Dirichlet &Dirichlet::operator=(const Dirichlet &from) {
    if (&from != this) {
      __generator = from.__generator;
      __gamma = from.__gamma;
      __params = from.__params;
    }
    return *this;
  }

  /// move operator
  INLINE Dirichlet &Dirichlet::operator=(Dirichlet &&from) {
    if (&from != this) {
      __generator = std::move(from.__generator);
      __gamma = std::move(from.__gamma);
      __params = std::move(from.__params);
    }
    return *this;
  }

  /// returns a sample from the Dirichlet distribution
  INLINE Dirichlet::result_type Dirichlet::operator()() {
    unsigned int size = __params.size();
    result_type res(size);
    float sum = 0.0f;
    while (sum == 0.0f) {
      for (unsigned int i = 0; i < size; ++i) {
        __gamma.param(std::gamma_distribution<float>::param_type(__params[i], 1));
        res[i] = __gamma(__generator);
        sum += res[i];
      }
    }
    for (unsigned int i = 0; i < size; ++i) {
      res[i] /= sum;
    }
    return res;
  }

  /// returns a sample from the Dirichlet distribution
  INLINE Dirichlet::result_type Dirichlet::
  operator()(const Dirichlet::param_type &parm) {
    unsigned int size = parm.size();
    result_type res(size);
    float sum = 0.0f;
    while (sum == 0.0f) {
      for (unsigned int i = 0; i < size; ++i) {
        __gamma.param(std::gamma_distribution<float>::param_type(parm[i], 1));
        res[i] = __gamma(__generator);
        sum += res[i];
      }
    }
    for (unsigned int i = 0; i < size; ++i) {
      res[i] /= sum;
    }
    return res;
  }

  /// returns the parameters of the distribution
  INLINE const Dirichlet::param_type &Dirichlet::param() const noexcept {
    return __params;
  }

  /// sets the parameters of the distribution
  INLINE void Dirichlet::param(const Dirichlet::param_type &parm) {
    __params = parm;
  }

  // Returns the greatest lower bound of the range of values possibly returned
  INLINE float Dirichlet::min() const noexcept { return 0.0f; }

  // Returns the lowest higher bound of the range of values possibly returned
  INLINE float Dirichlet::max() const noexcept { return 1.0f; }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
