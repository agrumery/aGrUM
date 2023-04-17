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

/**
 * @file
 * @brief Class encapsulating computations of notions from Information Theory
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */

#ifndef GUM_INFORMATION_THEORY
#define GUM_INFORMATION_THEORY
#include <concepts>
#include <functional>

#include <agrum/agrum.h>
#include <agrum/tools/variables/discreteVariable.h>
#include <agrum/tools/multidim/potential.h>

namespace gum {
  template < typename T >
  concept JointTargettable = requires(T t, const NodeSet& target) {
    { t.addJointTarget(target) } -> std::same_as< void >;
  };

  /** InformationTheory is a template class which aims at gathering the implementation of
   * informational functions (entropy, mutual information, etc.). All these operations start with
   * the inference of a joint distribution. Moreover, it is possible to want to condition these
   * calculations. The inference (able to compute joint distributions) passed as an argument can
   * therefore have previously defined evidence.
   *
   * @warning The joint distribution underlying the computations has a definition domain of
   * exponential size depending on the number of variables passed as arguments. One must be aware of
   * the complexity of the inference that will initially be carried out.
   *
   * @note All computations are made with std::log2
   *
   * @tparam INFERENCE_ENGINE : the inference engine
   * @tparam GUM_SCALAR : the numeric type of parameters
   */
  template < template < typename > class INFERENCE_ENGINE, typename GUM_SCALAR >
    requires JointTargettable< INFERENCE_ENGINE< GUM_SCALAR > >
  class InformationTheory {
    public:
    InformationTheory(INFERENCE_ENGINE< GUM_SCALAR >& engine,
                      gum::NodeSet                    X,
                      gum::NodeSet                    Y,
                      gum::NodeSet                    Z);
    InformationTheory(INFERENCE_ENGINE< GUM_SCALAR >& engine, gum::NodeSet X, gum::NodeSet Y);
    InformationTheory(INFERENCE_ENGINE< GUM_SCALAR >&   engine,
                      const std::vector< std::string >& Xnames,
                      const std::vector< std::string >& Ynames);
    InformationTheory(INFERENCE_ENGINE< GUM_SCALAR >&   engine,
                      const std::vector< std::string >& Xnames,
                      const std::vector< std::string >& Ynames,
                      const std::vector< std::string >& Znames);
    ~InformationTheory();

    GUM_SCALAR entropyXY();
    GUM_SCALAR entropyX();
    GUM_SCALAR entropyY();

    GUM_SCALAR entropyXgivenY();
    GUM_SCALAR entropyYgivenX();
    GUM_SCALAR mutualInformationXY();
    GUM_SCALAR variationOfInformationXY();


    GUM_SCALAR entropyXYgivenZ();
    GUM_SCALAR mutualInformationXYgivenZ();

    protected:
    INFERENCE_ENGINE< GUM_SCALAR >& engine_;

    const gum::NodeSet X_;
    const gum::NodeSet Y_;
    const gum::NodeSet Z_;

    gum::Set< const DiscreteVariable* > vX_;
    gum::Set< const DiscreteVariable* > vY_;
    gum::Set< const DiscreteVariable* > vZ_;

    gum::Potential< GUM_SCALAR > pXYZ_;
    gum::Potential< GUM_SCALAR > pXY_;
    gum::Potential< GUM_SCALAR > pXZ_;
    gum::Potential< GUM_SCALAR > pYZ_;
    gum::Potential< GUM_SCALAR > pX_;
    gum::Potential< GUM_SCALAR > pY_;
    gum::Potential< GUM_SCALAR > pZ_;

    void       makeInference_();
  };
}   // namespace gum

#include <agrum/tools/graphicalModels/algorithms/informationTheory_tpl.h>
#endif   // GUM_INFORMATION_THEORY
