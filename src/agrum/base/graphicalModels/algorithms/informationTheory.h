/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


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

#include <agrum/base/multidim/tensor.h>

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
  //@todo when CLANG-compliant for virtual class:  requires JointTargettable< INFERENCE_ENGINE<
  // GUM_SCALAR > >
  class InformationTheory {
    public:
    InformationTheory(INFERENCE_ENGINE< GUM_SCALAR >& engine,
                      NodeSet                         X,   // X,Y,Z passed by Value & move
                      NodeSet                         Y,
                      NodeSet                         Z);
    InformationTheory(INFERENCE_ENGINE< GUM_SCALAR >& engine, const NodeSet& X, const NodeSet& Y);
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


    GUM_SCALAR entropyXgivenZ();
    GUM_SCALAR entropyYgivenZ();
    GUM_SCALAR entropyXYgivenZ();
    GUM_SCALAR entropyXgivenYZ();
    GUM_SCALAR mutualInformationXYgivenZ();

    protected:
    INFERENCE_ENGINE< GUM_SCALAR >& engine_;

    const NodeSet X_;
    const NodeSet Y_;
    const NodeSet Z_;

    VariableSet vX_;
    VariableSet vY_;
    VariableSet vZ_;

    Tensor< GUM_SCALAR > pXYZ_;
    Tensor< GUM_SCALAR > pXY_;
    Tensor< GUM_SCALAR > pXZ_;
    Tensor< GUM_SCALAR > pYZ_;
    Tensor< GUM_SCALAR > pX_;
    Tensor< GUM_SCALAR > pY_;
    Tensor< GUM_SCALAR > pZ_;

    void makeInference_();
  };
}   // namespace gum

#include <agrum/base/graphicalModels/algorithms/informationTheory_tpl.h>
#endif   // GUM_INFORMATION_THEORY
