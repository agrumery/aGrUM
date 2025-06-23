/****************************************************************************
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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/





// this file is for giving access to methods defined in ancestor.

%define ADD_APPROXIMATIONSCHEME_API(parent,classname...)
%extend classname {
  using parent::setVerbosity;
  using parent::setEpsilon;
  using parent::setMinEpsilonRate;
  using parent::setMaxIter;
  using parent::setMaxTime;
  using parent::setPeriodSize;

  using parent::verbosity;
  using parent::epsilon;
  using parent::minEpsilonRate;
  using parent::maxIter;
  using parent::maxTime;
  using parent::periodSize;

  using parent::nbrIterations;
  using parent::currentTime;

  using parent::messageApproximationScheme;
  using parent::history;

  const gum::IApproximationSchemeConfiguration& _asIApproximationSchemeConfiguration() const {
    return *(dynamic_cast<const gum::IApproximationSchemeConfiguration *>(self));
  }
}
%enddef
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

ADD_APPROXIMATIONSCHEME_API(gum::ApproximationScheme,gum::credal::CNMonteCarloSampling<double>)
ADD_APPROXIMATIONSCHEME_API(gum::ApproximationScheme,gum::credal::CNLoopyPropagation<double>)
ADD_APPROXIMATIONSCHEME_API(gum::learning::IBNLearner,gum::learning::BNLearner<double>)

%extend gum::learning::BNLearner<double> {
  using gum::learning::IBNLearner::setMaxTime;
  using gum::learning::IBNLearner::maxTime;
  using gum::learning::IBNLearner::currentTime;

  using gum::learning::IBNLearner::learnDAG;
  using gum::learning::IBNLearner::learnPDAG;
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
  using gum::learning::IBNLearner::isEnabledEMEpsilon;
  using gum::learning::IBNLearner::EMMinEpsilonRate;
  using gum::learning::IBNLearner::isEnabledEMMinEpsilonRate;
  using gum::learning::IBNLearner::EMMaxIter;
  using gum::learning::IBNLearner::isEnabledEMMaxIter;
  using gum::learning::IBNLearner::EMMaxTime;
  using gum::learning::IBNLearner::isEnabledEMMaxTime;
  using gum::learning::IBNLearner::EMVerbosity;
  using gum::learning::IBNLearner::EMStateApproximationScheme;
  using gum::learning::IBNLearner::nbrEMIterations;
  using gum::learning::IBNLearner::EMHistory;

  using gum::learning::IBNLearner::setNumberOfThreads;

  Size getNumberOfThreads() const override {
     return self->getNumberOfThreads();
  }
  bool isGumNumberOfThreadsOverriden() const override {
     return self->isGumNumberOfThreadsOverriden();
  }
}

#####################################

%define ADD_NODEGRAPHPART_API(classname)
%extend classname {
  // erase node is not in this list since it is redefined by the very classes {Mixed|Di|Undi}Graph)
  gum::NodeId addNode() {
    return self->gum::NodeGraphPart::addNode();
  }
  std::vector<gum::NodeId> addNodes(gum::Size n) {
    return self->gum::NodeGraphPart::addNodes(n);
  }
  void addNodeWithId(const gum::NodeId id) {
    self->gum::NodeGraphPart::addNodeWithId(id);
  }
  bool existsNode(const gum::NodeId id) const {
    return self->gum::NodeGraphPart::existsNode(id);
  }
  gum::Size size() const {
    return self->gum::NodeGraphPart::size();
  }
  bool empty() const {
    return self->gum::NodeGraphPart::empty();
  }
}
%enddef
ADD_NODEGRAPHPART_API(gum::DiGraph)
ADD_ARCGRAPHPART_API(gum::DAG);
ADD_NODEGRAPHPART_API(gum::UndiGraph)
ADD_NODEGRAPHPART_API(gum::MixedGraph)
ADD_NODEGRAPHPART_API(gum::PDAG)

%define ADD_EDGEGRAPHPART_API(classname)
%extend classname {
  void addEdge(const NodeId n1,const NodeId n2) {
    self->addEdge(n1,n2);
  }
  void eraseEdge(const NodeId n1,const NodeId n2) {
    self->eraseEdge(gum::Edge(n1,n2));
  }
  bool existsEdge(const NodeId n1, const NodeId n2) const {
    return self->existsEdge(n1,n2);
  }
  gum::Size sizeEdges() const {
    return self->sizeEdges();
  }
  bool emptyEdges() const {
    return self->emptyEdges();
  }
  void eraseNeighbours(const gum::NodeId n) {
    self->eraseNeighbours(n);
  }
}
%enddef
ADD_EDGEGRAPHPART_API(gum::UndiGraph)
ADD_EDGEGRAPHPART_API(gum::MixedGraph)
ADD_EDGEGRAPHPART_API(gum::PDAG)

%define ADD_ARCGRAPHPART_API(classname)
%extend classname {
  void addArc(const gum::NodeId n1,const gum::NodeId n2) {
    self->addArc(n1,n2);
  }
  void eraseArc(const gum::NodeId n1,const gum::NodeId n2) {
    self->eraseArc(gum::Arc(n1,n2));
  }
  bool existsArc(const gum::NodeId n1,const gum::NodeId n2) const {
    return self->existsArc(n1,n2);
  }
  void eraseParents(const gum::NodeId n) {
    self->eraseParents(n);
  }
  void eraseChildren(const gum::NodeId n) {
    self->eraseChildren(n);
  }
  gum::Size sizeArcs() const {
    return self->sizeArcs();
  }
  bool emptyArcs() const {
    return self->emptyArcs();
  }
}
%enddef
ADD_ARCGRAPHPART_API(gum::DiGraph)
ADD_ARCGRAPHPART_API(gum::DAG)
ADD_ARCGRAPHPART_API(gum::MixedGraph)
ADD_ARCGRAPHPART_API(gum::PDAG)

%define ADD_MIXEDGRAPHPART_API(classname)
%extend classname {
  NodeSet boundary(NodeId id) const {
    return self->boundary(id);
  }
  std::vector< NodeId > mixedOrientedPath(NodeId node1, NodeId node2) const {
    return self->mixedOrientedPath(node1,node2);
  }
  std::vector< NodeId > mixedUnorientedPath(NodeId node1, NodeId node2) const {
    return self->mixedUnorientedPath(node1,node2);
  }
}
%enddef
ADD_MIXEDGRAPHPART_API(gum::MixedGraph)
ADD_MIXEDGRAPHPART_API(gum::PDAG)

#####################################
%define ADD_MULTIDIMDECORATOR_API(classname)
%extend classname {
/* wrapping the minimal interface from MultiDimDecorator */
  double get(const gum::Instantiation& i) const {
    return self->gum::MultiDimDecorator<double>::get(i);
  }

  void set ( const Instantiation& i, const double& value ) const {
    self->gum::MultiDimDecorator<double>::set(i,value);
  }

  bool empty() const {
    return self->gum::MultiDimDecorator<double>::empty();
  }


  gum::Idx pos ( const gum::DiscreteVariable& v) const {
    return self->gum::MultiDimDecorator<double>::pos(v);
  }

  bool contains ( const gum::DiscreteVariable& v) const {
    return self->gum::MultiDimDecorator<double>::contains(v);
  }

  gum::Idx nbrDim() const {
    return self->gum::MultiDimDecorator<double>::nbrDim();
  }

  const gum::DiscreteVariable& variable ( Idx i) const {
    return self->gum::MultiDimDecorator<double>::variable(i);
  }

  const gum::DiscreteVariable& variable ( const std::string& name) const {
    return self->gum::MultiDimDecorator<double>::variable(name);
  }

  void remove(const gum::DiscreteVariable& var) {
    self->erase(var);
  }

  void add ( const DiscreteVariable& v ) {
    self->gum::MultiDimDecorator<double>::add(v);
  }
}
%enddef
ADD_MULTIDIMDECORATOR_API(gum::Tensor<double>)

#####################################
#####################################
%define ADD_CREDALINFERENCEENGINE_API(classname)
%extend classname  {
  void setRepetitiveInd(const bool flag) {
    self->gum::credal::InferenceEngine<double>::setRepetitiveInd(flag);
  }
 Tensor<double> marginalMax ( const NodeId id ) const {
    return self->gum::credal::InferenceEngine<double>::marginalMax(id);
  }
  Tensor<double> marginalMin ( const NodeId id ) const {
    return self->gum::credal::InferenceEngine<double>::marginalMin(id);
  }
  Tensor<double> marginalMax ( const std::string name ) const {
    return self->gum::credal::InferenceEngine<double>::marginalMax(name);
  }
  Tensor<double> marginalMin ( const std::string name) const {
    return self->gum::credal::InferenceEngine<double>::marginalMin(name);
  }
  void insertModalsFile ( const std::string& path ) {
    self->gum::credal::InferenceEngine<double>::insertModalsFile(path);
  }
  const std::vector< double >& dynamicExpMax ( const std::string& varName ) const {
    return self->gum::credal::InferenceEngine<double>::dynamicExpMax(varName);
  }
  const std::vector< double >& dynamicExpMin ( const std::string& varName ) const {
    return self->gum::credal::InferenceEngine<double>::dynamicExpMin(varName);
  }
  void eraseAllEvidence() {
    self->gum::credal::InferenceEngine<double>::eraseAllEvidence();
  }
  //######## EVIDENCE ##########
  //############################
  void addEvidence( const NodeId id, const Idx val ) {
    self->gum::credal::InferenceEngine<double>::addEvidence(id,val);
  }
  void addEvidence( const std::string& nodeName, const Idx val ) {
    self->gum::credal::InferenceEngine<double>::addEvidence(nodeName,val);
  }
  void addEvidence( const NodeId id, const std::string& val ) {
    self->gum::credal::InferenceEngine<double>::addEvidence(id,val);
  }

  void addEvidence( const std::string& nodeName, const std::string& val ) {
    self->gum::credal::InferenceEngine<double>::addEvidence(nodeName,val);
  }

  void addEvidence( const NodeId id,const std::vector<double>& vals ) {
    self->gum::credal::InferenceEngine<double>::addEvidence(id,vals);
  }
  void addEvidence( const std::string& nodeName,
                   const std::vector<double>& vals ) {
    self->gum::credal::InferenceEngine<double>::addEvidence(nodeName,vals);
  }
  void addEvidence(const gum::Tensor<double>& p) {
    self->gum::credal::InferenceEngine<double>::addEvidence(p);
  }
}
%enddef
ADD_CREDALINFERENCEENGINE_API(gum::credal::CNMonteCarloSampling<double>)
ADD_CREDALINFERENCEENGINE_API(gum::credal::CNLoopyPropagation<double>)


#####################################
#####################################
%define ADD_INFERENCE_API(baseclassname,classname...)
%extend classname  {
  void makeInference(void) {
    self->baseclassname::makeInference();
  }
  const Tensor<double> posterior( const NodeId var ) {
    return self->baseclassname::posterior(var);
  }
  const Tensor<double> posterior( const std::string& nodeName ) {
    return self->baseclassname::posterior(nodeName);
  }

//######## EVIDENCE ##########
//############################
  void addEvidence( const NodeId id, const Idx val ) {
    self->baseclassname::addEvidence(id,val);
  }
  void addEvidence( const std::string& nodeName, const Idx val ) {
    self->baseclassname::addEvidence(nodeName,val);
  }
  void addEvidence( const NodeId id, const std::string& val ) {
    self->baseclassname::addEvidence(id,val);
  }

  void addEvidence( const std::string& nodeName, const std::string& val ) {
    self->baseclassname::addEvidence(nodeName,val);
  }

  void addEvidence( const NodeId id,const std::vector<double>& vals ) {
   self->baseclassname::addEvidence(id,vals);
  }
  void addEvidence( const std::string& nodeName,
                                const std::vector<double>& vals ) {
    self->baseclassname::addEvidence(nodeName,vals);
  }
  void addEvidence(const gum::Tensor<double>& p) {
    self->baseclassname::addEvidence(p);
  }


  void chgEvidence( const NodeId id, const Idx val ) {
    self->baseclassname::chgEvidence(id,val);
  }
  void chgEvidence( const std::string& nodeName, const Idx val ) {
    self->baseclassname::chgEvidence(nodeName,val);
  }

  void chgEvidence( const NodeId id, const std::string& val ) {
    self->baseclassname::chgEvidence(id,val);
  }
  void chgEvidence( const std::string& nodeName, const std::string& val ) {
    self->baseclassname::chgEvidence(nodeName,val);
  }
  void chgEvidence(const gum::Tensor<double>& p) {
    self->baseclassname::chgEvidence(p);
  }


  void chgEvidence( const NodeId id,const std::vector<double>& vals ) {
   self->baseclassname::chgEvidence(id,vals);
  }
  void chgEvidence( const std::string& nodeName,
                                const std::vector<double>& vals ) {
    self->baseclassname::chgEvidence(nodeName,vals);
  }

  bool hasEvidence( const NodeId id ) const {
    return self->baseclassname::hasEvidence(id);
  }
  bool hasEvidence( const std::string& nodeName) const {
    return self->baseclassname::hasEvidence(nodeName);
  }

  void eraseAllEvidence() {
    self->baseclassname::eraseAllEvidence();
  }

  void eraseEvidence( const NodeId id ) {
    self->baseclassname::eraseEvidence(id);
  }
  void eraseEvidence( const std::string& nodeName  ) {
    self->baseclassname::eraseEvidence(nodeName);
  }

  bool hasSoftEvidence( const NodeId id ) const {
    return self->baseclassname::hasSoftEvidence(id);
  }

  bool hasHardEvidence( const std::string& nodeName ) const {
    return self->baseclassname::hasHardEvidence(nodeName);
  }

  bool hasSoftEvidence( const std::string& nodeName ) const {
    return self->baseclassname::hasSoftEvidence(nodeName);
  }

  gum::Size nbrEvidence() const {
    return self->baseclassname::nbrEvidence();
  }

  gum::Size nbrHardEvidence() const {
    return self->baseclassname::nbrHardEvidence();
  }

  gum::Size nbrSoftEvidence() const {
    return self->baseclassname::nbrSoftEvidence();
  }
}
%enddef

%define ADD_MONOTARGET_INFERENCE_API(baseclassname,classname...)
  ADD_INFERENCE_API(baseclassname,classname)
%extend classname  {
//######## TARGETS ##########
//############################

    void eraseAllTargets() {
      self->baseclassname::eraseAllTargets();
    }

    void addAllTargets() {
      self->baseclassname::addAllTargets();
    }

    void addTarget( const NodeId target ) {
      self->baseclassname::addTarget(target);
    }

    void addTarget( const std::string& nodeName ) {
      self->baseclassname::addTarget(nodeName);
    }

    void eraseTarget( const NodeId target ) {
      self->baseclassname::eraseTarget(target);
    }

    void eraseTarget( const std::string& nodeName ) {
      self->baseclassname::eraseTarget(nodeName);
    }

    bool isTarget( const NodeId variable ) const {
      return self->baseclassname::isTarget(variable);
    }

    bool isTarget( const std::string& nodeName ) const {
      return self->baseclassname::isTarget(nodeName);
    }

    gum::Size nbrTargets( ) const {
      return self->baseclassname::nbrTargets();
    }

   double H( const NodeId X ) {
     return self->baseclassname::H(X);
   }
   double H( const std::string& nodeName ) {
     return self->baseclassname::H(nodeName);
   }

  Tensor<double> evidenceImpact(NodeId target,const NodeSet& evs){
    return self->baseclassname::evidenceImpact(target,evs);
  }
  Tensor<double> evidenceImpact(const std::string& target,const std::vector<std::string>& evs){
   return self->baseclassname::evidenceImpact(target,evs);
  }
}
%enddef

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
ADD_PARALLELIZED_INFERENCE_API(gum::ShaferShenoyMRFInference<double>)

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

%extend gum::Tensor<double> {
  gum::Size domainSize() {return self->gum::MultiDimDecorator<double>::domainSize();}
  gum::Size nbrDim() {return self->gum::MultiDimDecorator<double>::nbrDim();}
}

#################
%define ADD_MRF_INFERENCE_API(classname...)
ADD_MONOTARGET_INFERENCE_API (gum::MarginalTargetedMRFInference<double>,classname)
%extend classname {
   const IMarkovRandomField<double>& MRF() const { return self->gum::MarginalTargetedMRFInference<double>::MRF(); }
}
%enddef

%define ADD_JOINT_MRF_INFERENCE_API(classname)
ADD_MRF_INFERENCE_API(classname)
%extend classname {
  const Tensor<double> posterior( const NodeId var ) {
    return self->JointTargetedMRFInference<double>::posterior(var);
  }
  const Tensor<double> posterior( const std::string nodeName ) {
    return self->JointTargetedMRFInference<double>::posterior(nodeName);
  }
  void eraseAllTargets() {
    self->gum::JointTargetedMRFInference<double>::eraseAllTargets();
  }
  void eraseAllJointTargets() {
    self->gum::JointTargetedMRFInference<double>::eraseAllJointTargets();
  }
  void eraseAllMarginalTargets() {
    self->gum::JointTargetedMRFInference<double>::eraseAllMarginalTargets();
  }

  gum::Size nbrJointTargets() {
    return self->gum::JointTargetedMRFInference<double>::nbrJointTargets();
  }

  Tensor<double> evidenceJointImpact(const NodeSet& targets,const NodeSet& evs){
    return self->gum::JointTargetedMRFInference<double>::evidenceJointImpact(targets,evs);
  }
  Tensor<double> evidenceJointImpact(const std::vector<std::string>& targets,const std::vector<std::string>& evs){
   return self->gum::JointTargetedMRFInference<double>::evidenceJointImpact(targets,evs);
  }
}
%enddef
ADD_JOINT_MRF_INFERENCE_API(gum::ShaferShenoyMRFInference<double>)


#################
%define ADD_ID_INFERENCE_API(classname...)
ADD_INFERENCE_API(classname,classname)
%extend classname {
   void makeInference(void) {
     self->gum::InfluenceDiagramInference<double>::makeInference();
   }

   const InfluenceDiagram<double>& influenceDiagram() const { return self->gum::InfluenceDiagramInference<double>::influenceDiagram(); }
}
%enddef
ADD_ID_INFERENCE_API(gum::ShaferShenoyLIMIDInference<double >)



###################
%define ADD_CN_INFERENCE_API(classname...)
%extend classname {
  const CredalNet< double >& CN() const { return self->gum::credal::InferenceEngine<double>::credalNet(); }
}
%enddef
ADD_CN_INFERENCE_API(gum::credal::CNMonteCarloSampling<double>)
ADD_CN_INFERENCE_API(gum::credal::CNLoopyPropagation<double>)

extend gum::DiscreteVariable {
  std::string toFast() const {return "virtual method";}
}
