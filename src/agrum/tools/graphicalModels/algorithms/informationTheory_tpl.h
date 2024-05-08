/**
 *
 *   Copyright (c) 2005-2024  by Pierre-Henri WUILLEMIN(_at_LIP6) et Christophe GONZALES(_at_AMU)
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
#include <utility>

#include <agrum/tools/core/exceptions.h>
#include <agrum/tools/graphicalModels/algorithms/informationTheory.h>

#include <agrum/BN/inference/lazyPropagation.h>

#include <agrum/tools/core/math/math_utils.h>

#define INFORMATION_THEORY_TEMPLATE                        \
  template < template < typename > class INFERENCE_ENGINE, \
             typename GUM_SCALAR >   //@todo when CLANG-compliant for virtual class :  requires
                                     // JointTargettable< INFERENCE_ENGINE< GUM_SCALAR > >

namespace gum {
  INFORMATION_THEORY_TEMPLATE
  InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >::InformationTheory(
      INFERENCE_ENGINE< GUM_SCALAR >& engine,
      gum::NodeSet                    X,
      gum::NodeSet                    Y,
      gum::NodeSet Z) : engine_(engine), X_(std::move(X)), Y_(std::move(Y)), Z_(std::move(Z)) {
    GUM_CONSTRUCTOR(InformationTheory)
    if ((!(X_ * Y_).empty()) || (!(X_ * Z_).empty()) || (!(Z_ * Y_).empty()))
      GUM_ERROR(OperationNotAllowed, "The intersection between the set of variables must be empty")
    makeInference_();
  }

  INFORMATION_THEORY_TEMPLATE
  InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >::InformationTheory(
      INFERENCE_ENGINE< GUM_SCALAR >& engine,
      const gum::NodeSet&             X,
      const gum::NodeSet&             Y) : InformationTheory(engine, X, Y, NodeSet()) {}

  INFORMATION_THEORY_TEMPLATE
  InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >::InformationTheory(
      INFERENCE_ENGINE< GUM_SCALAR >&   engine,
      const std::vector< std::string >& Xnames,
      const std::vector< std::string >& Ynames) :
      InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >(engine,
                                                        engine.model().nodeset(Xnames),
                                                        engine.model().nodeset(Ynames),
                                                        NodeSet()) {}

  INFORMATION_THEORY_TEMPLATE
  InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >::InformationTheory(
      INFERENCE_ENGINE< GUM_SCALAR >&   engine,
      const std::vector< std::string >& Xnames,
      const std::vector< std::string >& Ynames,
      const std::vector< std::string >& Znames) :
      InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >(engine,
                                                        engine.model().nodeset(Xnames),
                                                        engine.model().nodeset(Ynames),
                                                        engine.model().nodeset(Znames)) {}

  INFORMATION_THEORY_TEMPLATE
  InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >::~InformationTheory() {
    GUM_DESTRUCTOR(InformationTheory);
  }

  INFORMATION_THEORY_TEMPLATE
  void InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >::makeInference_() {
    vX_.clear();
    for (const auto x: X_)
      vX_.insert(&engine_.model().variable(x));

    vY_.clear();
    for (const auto y: Y_)
      vY_.insert(&engine_.model().variable(y));

    vZ_.clear();
    for (const auto z: Z_)
      vZ_.insert(&engine_.model().variable(z));

    const NodeSet joint_vars = X_ + Y_ + Z_;
    if (!engine_.isJointTarget(joint_vars)) {
      // we check if it is not an implicit target : containng a node and some of its parent (could
      // be better)
      bool implicit_target = false;
      for (const auto node: joint_vars)
        if (engine_.model().family(node).isSupersetOrEqual(joint_vars)) {
          implicit_target = true;
          break;
        }
      if (!implicit_target) {
        engine_.eraseAllJointTargets();
        engine_.addJointTarget(joint_vars);
      }
    }
    engine_.makeInference();

    if (!Z_.empty()) {
      pXYZ_ = engine_.jointPosterior(joint_vars);
      pXZ_  = pXYZ_.margSumIn(vX_ + vZ_);
      pYZ_  = pXYZ_.margSumIn(vY_ + vZ_);
      pZ_   = pXZ_.margSumIn(vZ_);
      pXY_  = pXYZ_.margSumIn(vX_ + vY_);
    } else {
      pXY_ = engine_.jointPosterior(joint_vars);
    }
    pX_ = pXY_.margSumIn(vX_);
    pY_ = pXY_.margSumIn(vY_);
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
  GUM_SCALAR InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >::entropyXgivenY() {
    return -pXY_.expectedValue([this](const gum::Instantiation& i) -> GUM_SCALAR {
      // f(x,y)=log (p(x,y)/p(y))
      const auto& pxy = pXY_[i];
      if (pxy == GUM_SCALAR(0.0)) return GUM_SCALAR(0.0);

      const auto& py = pY_[i];
      if (py == GUM_SCALAR(0.0)) return GUM_SCALAR(0.0);

      return GUM_LOG2_OR_0(pxy / py);
    });
  }

  INFORMATION_THEORY_TEMPLATE
  GUM_SCALAR InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >::entropyYgivenX() {
    return -pXY_.expectedValue([this](const gum::Instantiation& i) -> GUM_SCALAR {
      // f(x,y)=log (p(x,y)/p(x))
      const auto& pxy = pXY_[i];
      if (pxy == GUM_SCALAR(0.0)) return GUM_SCALAR(0.0);

      const auto& px = pX_[i];
      if (px == GUM_SCALAR(0.0)) return GUM_SCALAR(0.0);

      return GUM_LOG2_OR_0(pxy / px);
    });
  }

  INFORMATION_THEORY_TEMPLATE
  GUM_SCALAR InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >::entropyXgivenZ() {
    if (Z_.empty()) GUM_ERROR(ArgumentError, "Z has not been specified.")
    return -pXZ_.expectedValue([this](const gum::Instantiation& i) -> GUM_SCALAR {
      // f(x,z)=log (p(x,z)/p(y))
      const auto& pxz = pXZ_[i];
      if (pxz == GUM_SCALAR(0.0)) return GUM_SCALAR(0.0);

      const auto& pz = pZ_[i];
      if (pz == GUM_SCALAR(0.0)) return GUM_SCALAR(0.0);

      return GUM_LOG2_OR_0(pxz / pz);
    });
  }

  INFORMATION_THEORY_TEMPLATE
  GUM_SCALAR InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >::entropyYgivenZ() {
    if (Z_.empty()) GUM_ERROR(ArgumentError, "Z has not been specified.")
    return -pYZ_.expectedValue([this](const gum::Instantiation& i) -> GUM_SCALAR {
      // f(y,z)=log (p(y,z)/p(z))
      const auto& pyz = pYZ_[i];
      if (pyz == GUM_SCALAR(0.0)) return GUM_SCALAR(0.0);

      const auto& pz = pZ_[i];
      if (pz == GUM_SCALAR(0.0)) return GUM_SCALAR(0.0);

      return GUM_LOG2_OR_0(pyz / pz);
    });
  }

  INFORMATION_THEORY_TEMPLATE
  GUM_SCALAR InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >::mutualInformationXY() {
    return pXY_.expectedValue([this](const gum::Instantiation& i) -> GUM_SCALAR {
      // f(x,y)=log (p(x,y)/p(x)p(y))
      const auto& pxy = pXY_[i];
      if (pxy == GUM_SCALAR(0.0)) return GUM_SCALAR(0.0);

      const auto& pxpy = pY_[i] * pX_[i];
      if (pxpy == GUM_SCALAR(0.0)) return GUM_SCALAR(0.0);

      return GUM_LOG2_OR_0(pxy / pxpy);
    });
  }

  INFORMATION_THEORY_TEMPLATE
  GUM_SCALAR InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >::variationOfInformationXY() {
    return -pXY_.expectedValue([this](const gum::Instantiation& i) -> GUM_SCALAR {
      // f(x,y)= p(x)p(y))
      return GUM_LOG2_OR_0(pY_[i] * pX_[i]);
    });
  }

  INFORMATION_THEORY_TEMPLATE
  GUM_SCALAR InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >::entropyXYgivenZ() {
    if (Z_.empty()) GUM_ERROR(ArgumentError, "Z has not been specified.")
    return pXYZ_.expectedValue([this](const gum::Instantiation& i) -> GUM_SCALAR {
      // f(x,y)=log (p(x,y)/p(y))
      const auto& pxyz = pXYZ_[i];
      if (pxyz == GUM_SCALAR(0.0)) return GUM_SCALAR(0.0);

      const auto& pz = pZ_[i];
      if (pz == GUM_SCALAR(0.0)) return GUM_SCALAR(0.0);

      return -GUM_LOG2_OR_0(pxyz / pz);
    });
  }

  INFORMATION_THEORY_TEMPLATE
  GUM_SCALAR InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >::entropyXgivenYZ() {
    if (Z_.empty()) GUM_ERROR(ArgumentError, "Z has not been specified.")
    return pXYZ_.expectedValue([this](const gum::Instantiation& i) -> GUM_SCALAR {
      // f(x,y)=log (p(x,y)/p(y))
      const auto& pxyz = pXYZ_[i];
      if (pxyz == GUM_SCALAR(0.0)) return GUM_SCALAR(0.0);

      const auto& pyz = pYZ_[i];
      if (pyz == GUM_SCALAR(0.0)) return GUM_SCALAR(0.0);

      return -GUM_LOG2_OR_0(pxyz / pyz);
    });
  }

  INFORMATION_THEORY_TEMPLATE
  GUM_SCALAR InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >::mutualInformationXYgivenZ() {
    if (Z_.empty()) GUM_ERROR(ArgumentError, "Z has not been specified.")
    return pXYZ_.expectedValue([this](const gum::Instantiation& i) -> GUM_SCALAR {
      // f(x,y)=log (p(x,y)/p(x)p(y))
      const auto& pzpxyz = pXYZ_[i] * pZ_[i];
      if (pzpxyz == GUM_SCALAR(0.0)) return GUM_SCALAR(0.0);

      const auto& pxzpyz = pXZ_[i] * pYZ_[i];
      if (pxzpyz == GUM_SCALAR(0.0)) return GUM_SCALAR(0.0);

      return GUM_LOG2_OR_0(pzpxyz / pxzpyz);
    });
  }

#undef INFORMATION_THEORY_TEMPLATE
}   // namespace gum
