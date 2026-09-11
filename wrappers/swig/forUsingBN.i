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

// BN-owned %extend blocks (approximation-scheme accessors on BN's own inference/
// learning classes), split out of forUsing.i. forUsing.i used to be %include-d
// unconditionally by every module's wrap file, and since leaf modules (ID/MRF/CN/CM)
// %import "pyagrum.i" (the full BASE+BN type universe) before processing their own
// aGrUM_wrap_*.i, every one of these BN-only %extend blocks was redundantly
// reinstantiated inside every leaf module's own generated wrap .cxx -- code the leaf
// never needed, that pulled in BASE/BN internals (e.g. gum::learning::KNML via
// BNLearner::correctedMutualInformation) not exported by core once
// PYAGRUM_TRIM_CORE_EXPORTS restricts core's dynamic symbol table to what the leaves
// actually use, causing an "undefined symbol" ImportError at leaf .so load time.
// Only aGrUM_wrap_BN.i %include's this file.

ADD_APPROXIMATIONSCHEME_API(gum::ApproximationScheme,gum::GibbsSampling<double>)
ADD_APPROXIMATIONSCHEME_API(gum::ApproximationScheme,gum::ImportanceSampling<double>)
ADD_APPROXIMATIONSCHEME_API(gum::ApproximationScheme,gum::WeightedSampling<double>)
ADD_APPROXIMATIONSCHEME_API(gum::ApproximationScheme,gum::MonteCarloSampling<double>)
ADD_APPROXIMATIONSCHEME_API(gum::ApproximationScheme,gum::LoopySamplingInference<double,gum::ImportanceSampling>)
ADD_APPROXIMATIONSCHEME_API(gum::ApproximationScheme,gum::LoopySamplingInference<double,gum::WeightedSampling>)
ADD_APPROXIMATIONSCHEME_API(gum::ApproximationScheme,gum::LoopySamplingInference<double,gum::GibbsSampling>)
ADD_APPROXIMATIONSCHEME_API(gum::ApproximationScheme,gum::LoopySamplingInference<double,gum::MonteCarloSampling>)

ADD_APPROXIMATIONSCHEME_API(gum::ApproximationScheme,gum::LoopyBeliefPropagation<double>)

ADD_APPROXIMATIONSCHEME_API(gum::ApproximationScheme,gum::GibbsBNdistance<double>)
ADD_APPROXIMATIONSCHEME_API(gum::ApproximationScheme,gum::MCBNDistance<double>)

ADD_APPROXIMATIONSCHEME_API(gum::learning::IBNLearner,gum::learning::BNLearner<double>)

%extend gum::learning::BNLearner<double> {
  using gum::learning::IBNLearner::setMaxTime;
  using gum::learning::IBNLearner::maxTime;
  using gum::learning::IBNLearner::currentTime;

  using gum::learning::IBNLearner::learnDAG;
  using gum::learning::IBNLearner::learnPDAG;
  using gum::learning::IBNLearner::learnPAG;
  using gum::learning::IBNLearner::names;
  using gum::learning::IBNLearner::idFromName;
  using gum::learning::IBNLearner::nameFromId;
  using gum::learning::IBNLearner::setDatabaseWeight;
  using gum::learning::IBNLearner::setRecordWeight;
  using gum::learning::IBNLearner::databaseWeight;
  using gum::learning::IBNLearner::recordWeight;
  using gum::learning::IBNLearner::hasMissingValues;
  using gum::learning::IBNLearner::logLikelihood;
  using gum::learning::IBNLearner::score;
  using gum::learning::IBNLearner::mutualInformation;
  using gum::learning::IBNLearner::correctedMutualInformation;
  using gum::learning::IBNLearner::rawPseudoCount;
  using gum::learning::IBNLearner::nbRows;
  using gum::learning::IBNLearner::nbCols;
  using gum::learning::IBNLearner::domainSize;
  using gum::learning::IBNLearner::evEq;
  using gum::learning::IBNLearner::evIn;

  using gum::learning::IBNLearner::isUsingEM;
  using gum::learning::IBNLearner::EMEpsilon;
  using gum::learning::IBNLearner::EMisEnabledEpsilon;
  using gum::learning::IBNLearner::EMMinEpsilonRate;
  using gum::learning::IBNLearner::EMisEnabledMinEpsilonRate;
  using gum::learning::IBNLearner::EMMaxIter;
  using gum::learning::IBNLearner::EMisEnabledMaxIter;
  using gum::learning::IBNLearner::EMMaxTime;
  using gum::learning::IBNLearner::EMisEnabledMaxTime;
  using gum::learning::IBNLearner::EMVerbosity;
  using gum::learning::IBNLearner::EMnbrIterations;
  using gum::learning::IBNLearner::EMHistory;
  using gum::learning::IBNLearner::EMStateApproximationScheme;
  using gum::learning::IBNLearner::EMStateMessage;
  using gum::learning::IBNLearner::EMnbrIterations;

  using gum::learning::IBNLearner::setNumberOfThreads;

  Size getNumberOfThreads() const override {
     return self->getNumberOfThreads();
  }
  bool isGumNumberOfThreadsOverriden() const override {
     return self->isGumNumberOfThreadsOverriden();
  }
}

%define ADD_BN_MONOTARGET_INFERENCE_API(baseclassname,classname...)
ADD_MONOTARGET_INFERENCE_API(baseclassname,classname)
  %extend classname {
    const IBayesNet< double >& BN() const { return self->baseclassname::BN(); }
  }
%enddef

ADD_BN_MONOTARGET_INFERENCE_API(gum::MarginalTargetedInference<double>,gum::VariableElimination<double>)
ADD_BN_MONOTARGET_INFERENCE_API(gum::MarginalTargetedInference<double>,gum::LoopyBeliefPropagation<double>)


%define ADD_SAMPLING_INFERENCE_API(classname...)
ADD_BN_MONOTARGET_INFERENCE_API(gum::MarginalTargetedInference<double>,classname)
%extend classname  {
    const gum::Tensor<double>& currentPosterior(const NodeId id)
        {return self->gum::SamplingInference<double>::currentPosterior(id);};
    const gum::Tensor<double>& currentPosterior(const std::string& name)
        {return self->gum::SamplingInference<double>::currentPosterior(name);};
}
%enddef
ADD_SAMPLING_INFERENCE_API(gum::GibbsSampling<double>)
ADD_SAMPLING_INFERENCE_API(gum::MonteCarloSampling<double>)
ADD_SAMPLING_INFERENCE_API(gum::WeightedSampling<double>)
ADD_SAMPLING_INFERENCE_API(gum::ImportanceSampling<double>)
ADD_SAMPLING_INFERENCE_API(gum::LoopySamplingInference<double,gum::ImportanceSampling>)
ADD_SAMPLING_INFERENCE_API(gum::LoopySamplingInference<double,gum::GibbsSampling>)
ADD_SAMPLING_INFERENCE_API(gum::LoopySamplingInference<double,gum::WeightedSampling>)
ADD_SAMPLING_INFERENCE_API(gum::LoopySamplingInference<double,gum::MonteCarloSampling>)

%define ADD_JOINT_INFERENCE_API(classname)
ADD_BN_MONOTARGET_INFERENCE_API(gum::MarginalTargetedInference<double>,classname)
%extend classname {
  const Tensor<double> posterior( const NodeId var ) {
    return self->JointTargetedInference<double>::posterior(var);
  }
  const Tensor<double> posterior( const std::string nodeName ) {
    return self->JointTargetedInference<double>::posterior(nodeName);
  }
  void eraseAllTargets() {
    self->gum::JointTargetedInference<double>::eraseAllTargets();
  }
  void eraseAllJointTargets() {
    self->gum::JointTargetedInference<double>::eraseAllJointTargets();
  }
  void eraseAllMarginalTargets() {
    self->gum::JointTargetedInference<double>::eraseAllMarginalTargets();
  }

  gum::Size nbrJointTargets() {
    return self->gum::JointTargetedInference<double>::nbrJointTargets();
  }

  Tensor<double> evidenceJointImpact(const NodeSet& targets,const NodeSet& evs){
    return self->gum::JointTargetedInference<double>::evidenceJointImpact(targets,evs);
  }
  Tensor<double> evidenceJointImpact(const std::vector<std::string>& targets,const std::vector<std::string>& evs){
   return self->gum::JointTargetedInference<double>::evidenceJointImpact(targets,evs);
  }
}
%enddef
ADD_JOINT_INFERENCE_API(gum::LazyPropagation<double>)
ADD_JOINT_INFERENCE_API(gum::ShaferShenoyInference<double>)


%define ADD_PARALLELIZED_INFERENCE_API(classname)
%extend classname {

  void setNumberOfThreads (int nb) {
    self->setNumberOfThreads(nb);
  }
  int getNumberOfThreads () {
    return self->getNumberOfThreads();
  }
  bool isGumNumberOfThreadsOverriden () {
    return self->isGumNumberOfThreadsOverriden();
  }
  void setMaxMemory (int gigabytes) {
    self->setMaxMemory(gigabytes);
  }
}
%enddef
ADD_PARALLELIZED_INFERENCE_API(gum::LazyPropagation<double>)
ADD_PARALLELIZED_INFERENCE_API(gum::ShaferShenoyInference<double>)
ADD_PARALLELIZED_INFERENCE_API(gum::VariableElimination<double>)

%define ADD_GIBBS_OPERATOR_API(classname...)
%extend classname {
    /** Getters and setters*/
    gum::Size nbrDrawnVar() const { return self->GibbsOperator<double>::nbrDrawnVar(); }
    void setNbrDrawnVar(Size _nbr) { self->GibbsOperator<double>::setNbrDrawnVar(_nbr); }
    bool isDrawnAtRandom() const { return self->GibbsOperator<double>::isDrawnAtRandom(); }
    void setDrawnAtRandom(bool _atRandom) { self->GibbsOperator<double>::setDrawnAtRandom(_atRandom); }
}
%enddef
ADD_GIBBS_OPERATOR_API(gum::GibbsSampling<double>)
ADD_GIBBS_OPERATOR_API(gum::LoopySamplingInference<double,gum::GibbsSampling>)
ADD_GIBBS_OPERATOR_API(gum::GibbsBNdistance<double>)

%extend gum::LoopySamplingInference<double,gum::GibbsSampling> {
  gum::Size burnIn() const { return self->gum::GibbsSampling<double>::burnIn();}
  void setBurnIn(gum::Size b) { self->gum::GibbsSampling<double>::setBurnIn(b);}
}
