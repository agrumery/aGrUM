
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief a class for sampling w.r.t. Dirichlet distributions
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

namespace gum {

  // default constructor
  INLINE Dirichlet::Dirichlet(const param_type& params, unsigned int seed) :
      generator__(gum::getRandomGenerator(seed)), params__(params) {
    GUM_CONSTRUCTOR(Dirichlet);
  }

  // copy constructor
  INLINE Dirichlet::Dirichlet(const Dirichlet& from) :
      generator__(from.generator__), gamma__(from.gamma__),
      params__(from.params__) {
    GUM_CONS_CPY(Dirichlet);
  }

  // move constructor
  INLINE Dirichlet::Dirichlet(Dirichlet&& from) :
      generator__(std::move(from.generator__)), gamma__(std::move(from.gamma__)),
      params__(std::move(from.params__)) {
    GUM_CONS_MOV(Dirichlet);
  }

  // destructor
  INLINE Dirichlet::~Dirichlet() { GUM_DESTRUCTOR(Dirichlet); }

  // copy operator
  INLINE Dirichlet& Dirichlet::operator=(const Dirichlet& from) {
    if (&from != this) {
      generator__ = from.generator__;
      gamma__ = from.gamma__;
      params__ = from.params__;
    }
    return *this;
  }

  // move operator
  INLINE Dirichlet& Dirichlet::operator=(Dirichlet&& from) {
    if (&from != this) {
      generator__ = std::move(from.generator__);
      gamma__ = std::move(from.gamma__);
      params__ = std::move(from.params__);
    }
    return *this;
  }

  // returns a sample from the Dirichlet distribution
  INLINE Dirichlet::result_type Dirichlet::operator()() {
    Size        size = Size(params__.size());
    result_type res(size);
    float       sum = 0.0f;
    while (sum == 0.0f) {
      for (Idx i = 0; i < size; ++i) {
        gamma__.param(
           std::gamma_distribution< float >::param_type(params__[i], 1));
        res[i] = gamma__(generator__);
        sum += res[i];
      }
    }
    for (Idx i = 0; i < size; ++i) {
      res[i] /= sum;
    }
    return res;
  }

  // returns a sample from the Dirichlet distribution
  INLINE Dirichlet::result_type
     Dirichlet::operator()(const Dirichlet::param_type& parm) {
    Size        size = Size(parm.size());
    result_type res(size);
    float       sum = 0.0f;
    while (sum == 0.0f) {
      for (Idx i = 0; i < size; ++i) {
        gamma__.param(std::gamma_distribution< float >::param_type(parm[i], 1));
        res[i] = gamma__(generator__);
        sum += res[i];
      }
    }
    for (Idx i = 0; i < size; ++i) {
      res[i] /= sum;
    }
    return res;
  }

  // returns the parameters of the distribution
  INLINE const Dirichlet::param_type& Dirichlet::param() const noexcept {
    return params__;
  }

  // sets the parameters of the distribution
  INLINE void Dirichlet::param(const Dirichlet::param_type& parm) {
    params__ = parm;
  }

  // Returns the greatest lower bound of the range of values possibly returned
  INLINE float Dirichlet::min() const noexcept { return 0.0f; }

  // Returns the lowest higher bound of the range of values possibly returned
  INLINE float Dirichlet::max() const noexcept { return 1.0f; }

} /* namespace gum */
