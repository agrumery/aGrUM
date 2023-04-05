/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) et Christophe GONZALES(_at_AMU)
 * (_at_AMU) info_at_agrum_dot_org
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
 * @brief Implementation of the Class encapsulating computations of notions from Information Theory
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#include <cmath>
#include <utility>

#include <agrum/tools/graphicalModels/algorithms/informationTheory.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/tools/core/exceptions.h>

#define INFORMATION_THEORY_TEMPLATE                                              \
  template < template < typename > class INFERENCE_ENGINE, typename GUM_SCALAR > \
    requires JointTargettable< INFERENCE_ENGINE< GUM_SCALAR > >

namespace gum {
  INFORMATION_THEORY_TEMPLATE
  InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >::InformationTheory(
     INFERENCE_ENGINE< GUM_SCALAR >& engine,
     gum::NodeSet                    X,
     gum::NodeSet                    Y) :
      engine_(engine),
      X_(std::move(X)), Y_(std::move(Y)) {
    makeInference_();
    GUM_CONSTRUCTOR(InformationTheory)
  }

  INFORMATION_THEORY_TEMPLATE
  InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >::InformationTheory(
     INFERENCE_ENGINE< GUM_SCALAR >&   engine,
     const std::vector< std::string >& Xnames,
     const std::vector< std::string >& Ynames) :
      InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >(engine,
                                                        engine.model().nodeset(Xnames),
                                                        engine.model().nodeset(Ynames)) {}


  INFORMATION_THEORY_TEMPLATE
  void InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >::makeInference_() {
    vX_.clear();
    for (const auto x: X_)
      vX_.insert(&engine_.model().variable(x));

    vY_.clear();
    for (const auto y: Y_)
      vY_.insert(&engine_.model().variable(y));

    engine_.eraseAllTargets();
    engine_.addJointTarget(X_ + Y_);
    engine_.makeInference();

    pXY_ = engine_.jointPosterior(X_ + Y_);
    pX_  = pXY_.margSumIn(vX_);
    pY_  = pXY_.margSumIn(vY_);
  }

  INFORMATION_THEORY_TEMPLATE
  GUM_SCALAR InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >::entropyX() { return pX_.entropy(); }

  INFORMATION_THEORY_TEMPLATE
  GUM_SCALAR InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >::entropyY() { return pY_.entropy(); }

  INFORMATION_THEORY_TEMPLATE
  GUM_SCALAR InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >::entropyXY() {
    return pXY_.entropy();
  }

  INFORMATION_THEORY_TEMPLATE
  GUM_SCALAR InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >::logOr0_(GUM_SCALAR x) {
    return (x == GUM_SCALAR(0.0)) ? GUM_SCALAR(0.0) : std::log2(x);
  }

  INFORMATION_THEORY_TEMPLATE
  GUM_SCALAR InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >::entropyXgivenY() {
    return expectedValueXY([this](const gum::Instantiation& i) -> GUM_SCALAR {
      // f(x,y)=log (p(x,y)/p(y))
      const auto& pxy = pXY_[i];
      if (pxy == GUM_SCALAR(0.0)) return GUM_SCALAR(0.0);

      const auto& py = pY_[i];
      if (py == GUM_SCALAR(0.0)) return GUM_SCALAR(0.0);

      return -logOr0_(pxy / py);
    });
  }
  INFORMATION_THEORY_TEMPLATE
  GUM_SCALAR InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >::entropyYgivenX() {
    return expectedValueXY([this](const gum::Instantiation& i) -> GUM_SCALAR {
      // f(x,y)=log (p(x,y)/p(x))
      const auto& pxy = pXY_[i];
      if (pxy == GUM_SCALAR(0.0)) return GUM_SCALAR(0.0);

      const auto& px = pX_[i];
      if (px == GUM_SCALAR(0.0)) return GUM_SCALAR(0.0);

      return -logOr0_(pxy / px);
    });
  }

  INFORMATION_THEORY_TEMPLATE
  GUM_SCALAR InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >::mutualInformationXY() {
    return expectedValueXY([this](const gum::Instantiation& i) -> GUM_SCALAR {
      // f(x,y)=log (p(x,y)/p(x)p(y))
      const auto& pxy = pXY_[i];
      if (pxy == GUM_SCALAR(0.0)) return GUM_SCALAR(0.0);

      const auto& pxpy = pY_[i] * pX_[i];
      if (pxpy == GUM_SCALAR(0.0)) return GUM_SCALAR(0.0);

      return logOr0_(pxy / pxpy);
    });
  }

  INFORMATION_THEORY_TEMPLATE
  GUM_SCALAR InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >::expectedValueXY(
     std::function< GUM_SCALAR(const Instantiation&) > f) {
    GUM_SCALAR res = 0;
    auto       i   = Instantiation(pXY_);
    for (i.setFirst(); !i.end(); i.inc()) {
      const GUM_SCALAR v_f = f(i);
      if (v_f != GUM_SCALAR(0.0)) res += pXY_[i] * v_f;
    }
    return res;
  }
#undef INFORMATION_THEORY_TEMPLATE
}   // namespace gum
