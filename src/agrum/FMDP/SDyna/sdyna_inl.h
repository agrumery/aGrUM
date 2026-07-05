/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once

#include <agrum/FMDP/SDyna/sdyna.h>

namespace gum {


  INLINE SDYNA* SDYNA::spitiInstance(double attributeSelectionThreshold,
                                     double discountFactor,
                                     double epsilon,
                                     Idx    observationPhaseLenght,
                                     Idx    nbValueIterationStep) {
    bool               actionReward = false;
    ILearningStrategy* ls
        = new FMDPLearner< CHI2TEST, CHI2TEST, ITILEARNER >(attributeSelectionThreshold,
                                                            actionReward);
    IPlanningStrategy< double >* ps
        = StructuredPlaner< double >::sviInstance(discountFactor, epsilon);
    IDecisionStrategy* ds = new E_GreedyDecider();
    return new SDYNA(ls, ps, ds, observationPhaseLenght, nbValueIterationStep, actionReward);
  }

  INLINE SDYNA* SDYNA::spimddiInstance(double attributeSelectionThreshold,
                                       double similarityThreshold,
                                       double discountFactor,
                                       double epsilon,
                                       Idx    observationPhaseLenght,
                                       Idx    nbValueIterationStep) {
    bool               actionReward = false;
    ILearningStrategy* ls
        = new FMDPLearner< GTEST, GTEST, IMDDILEARNER >(attributeSelectionThreshold,
                                                        actionReward,
                                                        similarityThreshold);
    IPlanningStrategy< double >* ps
        = StructuredPlaner< double >::spumddInstance(discountFactor, epsilon, false);
    IDecisionStrategy* ds = new E_GreedyDecider();
    return new SDYNA(ls, ps, ds, observationPhaseLenght, nbValueIterationStep, actionReward, false);
  }

  INLINE SDYNA* SDYNA::RMaxMDDInstance(double attributeSelectionThreshold,
                                       double similarityThreshold,
                                       double discountFactor,
                                       double epsilon,
                                       Idx    observationPhaseLenght,
                                       Idx    nbValueIterationStep) {
    bool               actionReward = true;
    ILearningStrategy* ls
        = new FMDPLearner< GTEST, GTEST, IMDDILEARNER >(attributeSelectionThreshold,
                                                        actionReward,
                                                        similarityThreshold);
    AdaptiveRMaxPlaner* rm
        = AdaptiveRMaxPlaner::ReducedAndOrderedInstance(ls, discountFactor, epsilon);
    IPlanningStrategy< double >* ps = rm;
    IDecisionStrategy*           ds = rm;
    return new SDYNA(ls, ps, ds, observationPhaseLenght, nbValueIterationStep, actionReward);
  }

  INLINE SDYNA* SDYNA::RMaxTreeInstance(double attributeSelectionThreshold,
                                        double discountFactor,
                                        double epsilon,
                                        Idx    observationPhaseLenght,
                                        Idx    nbValueIterationStep) {
    bool               actionReward = true;
    ILearningStrategy* ls
        = new FMDPLearner< GTEST, GTEST, ITILEARNER >(attributeSelectionThreshold, actionReward);
    AdaptiveRMaxPlaner*          rm = AdaptiveRMaxPlaner::TreeInstance(ls, discountFactor, epsilon);
    IPlanningStrategy< double >* ps = rm;
    IDecisionStrategy*           ds = rm;
    return new SDYNA(ls, ps, ds, observationPhaseLenght, nbValueIterationStep, actionReward);
  }

  INLINE SDYNA* SDYNA::RandomMDDInstance(double attributeSelectionThreshold,
                                         double similarityThreshold,
                                         double discountFactor,
                                         double epsilon,
                                         Idx    observationPhaseLenght,
                                         Idx    nbValueIterationStep) {
    bool               actionReward = true;
    ILearningStrategy* ls
        = new FMDPLearner< GTEST, GTEST, IMDDILEARNER >(attributeSelectionThreshold,
                                                        actionReward,
                                                        similarityThreshold);
    IPlanningStrategy< double >* ps
        = StructuredPlaner< double >::spumddInstance(discountFactor, epsilon);
    IDecisionStrategy* ds = new RandomDecider();
    return new SDYNA(ls, ps, ds, observationPhaseLenght, nbValueIterationStep, actionReward);
  }

  INLINE SDYNA* SDYNA::RandomTreeInstance(double attributeSelectionThreshold,
                                          double discountFactor,
                                          double epsilon,
                                          Idx    observationPhaseLenght,
                                          Idx    nbValueIterationStep) {
    bool               actionReward = true;
    ILearningStrategy* ls
        = new FMDPLearner< CHI2TEST, CHI2TEST, ITILEARNER >(attributeSelectionThreshold,
                                                            actionReward);
    IPlanningStrategy< double >* ps
        = StructuredPlaner< double >::sviInstance(discountFactor, epsilon);
    IDecisionStrategy* ds = new RandomDecider();
    return new SDYNA(ls, ps, ds, observationPhaseLenght, nbValueIterationStep, actionReward);
  }

  INLINE void SDYNA::addAction(const Idx actionId, std::string_view actionName) {
    fmdp_->addAction(actionId, std::string(actionName));
  }

  INLINE void SDYNA::addVariable(const DiscreteVariable* var) { fmdp_->addVariable(var); }

  INLINE void SDYNA::setCurrentState(const Instantiation& currentState) {
    lastState_ = currentState;
  }

  INLINE std::string SDYNA::optimalPolicy2String() { return _planer_->optimalPolicy2String(); }

  INLINE Size SDYNA::learnerSize() { return _learner_->size(); }

  INLINE Size SDYNA::modelSize() { return fmdp_->size(); }

  INLINE Size SDYNA::valueFunctionSize() { return _planer_->vFunctionSize(); }

  INLINE Size SDYNA::optimalPolicySize() { return _planer_->optimalPolicySize(); }

}   // namespace gum
