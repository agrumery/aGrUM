/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

namespace gum {

  // default constructor
  INLINE Dirichlet::Dirichlet(const param_type& params) : _params_(params) {
    GUM_CONSTRUCTOR(Dirichlet);
  }

  // copy constructor
  INLINE Dirichlet::Dirichlet(const Dirichlet& from) :
      _gamma_(from._gamma_), _params_(from._params_) {
    GUM_CONS_CPY(Dirichlet);
  }

  // move constructor
  INLINE Dirichlet::Dirichlet(Dirichlet&& from) :
      _gamma_(std::move(from._gamma_)), _params_(std::move(from._params_)) {
    GUM_CONS_MOV(Dirichlet);
  }

  // destructor
  INLINE Dirichlet::~Dirichlet() {
    GUM_DESTRUCTOR(Dirichlet);
    ;
  }

  // copy operator
  INLINE Dirichlet& Dirichlet::operator=(const Dirichlet& from) {
    if (&from != this) {
      _gamma_  = from._gamma_;
      _params_ = from._params_;
    }
    return *this;
  }

  // move operator
  INLINE Dirichlet& Dirichlet::operator=(Dirichlet&& from) {
    if (&from != this) {
      _gamma_  = std::move(from._gamma_);
      _params_ = std::move(from._params_);
    }
    return *this;
  }

  // returns a sample from the Dirichlet distribution
  INLINE Dirichlet::result_type Dirichlet::operator()() {
    Size        size = Size(_params_.size());
    result_type res(size);
    float       sum = 0.0f;
    while (sum == 0.0f) {
      for (Idx i = 0; i < size; ++i) {
        _gamma_.param(std::gamma_distribution< float >::param_type(_params_[i], 1));
        res[i] = _gamma_(gum::randomGenerator());
        sum += res[i];
      }
    }
    for (Idx i = 0; i < size; ++i) {
      res[i] /= sum;
    }
    return res;
  }

  // returns a sample from the Dirichlet distribution
  INLINE Dirichlet::result_type Dirichlet::operator()(const Dirichlet::param_type& parm) {
    Size        size = Size(parm.size());
    result_type res(size);
    float       sum = 0.0f;
    while (sum == 0.0f) {
      for (Idx i = 0; i < size; ++i) {
        _gamma_.param(std::gamma_distribution< float >::param_type(parm[i], 1));
        res[i] = _gamma_(gum::randomGenerator());
        sum += res[i];
      }
    }
    for (Idx i = 0; i < size; ++i) {
      res[i] /= sum;
    }
    return res;
  }

  // returns the parameters of the distribution
  INLINE const Dirichlet::param_type& Dirichlet::param() const noexcept { return _params_; }

  // sets the parameters of the distribution
  INLINE void Dirichlet::param(const Dirichlet::param_type& parm) { _params_ = parm; }

  // Returns the greatest lower bound of the range of values possibly returned
  INLINE float Dirichlet::min() const noexcept { return 0.0f; }

  // Returns the lowest higher bound of the range of values possibly returned
  INLINE float Dirichlet::max() const noexcept { return 1.0f; }
} /* namespace gum */
