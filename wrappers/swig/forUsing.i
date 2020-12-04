/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
ADD_APPROXIMATIONSCHEME_API(gum::learning::genericBNLearner,gum::learning::BNLearner<double>)

%extend gum::learning::BNLearner<double> {
  using gum::learning::genericBNLearner::setMaxTime;
  using gum::learning::genericBNLearner::maxTime;
  using gum::learning::genericBNLearner::currentTime;

  using gum::learning::genericBNLearner::learnDAG;
  using gum::learning::genericBNLearner::names;
  using gum::learning::genericBNLearner::modalities;
  using gum::learning::genericBNLearner::idFromName;
  using gum::learning::genericBNLearner::nameFromId;
  using gum::learning::genericBNLearner::useScoreAIC;
  using gum::learning::genericBNLearner::useScoreBD;
  using gum::learning::genericBNLearner::useScoreBDeu;
  using gum::learning::genericBNLearner::useScoreBIC;
  using gum::learning::genericBNLearner::useScoreK2;
  using gum::learning::genericBNLearner::useScoreLog2Likelihood;
  using gum::learning::genericBNLearner::setDatabaseWeight;
  using gum::learning::genericBNLearner::setRecordWeight;
  using gum::learning::genericBNLearner::databaseWeight;
  using gum::learning::genericBNLearner::recordWeight;
  using gum::learning::genericBNLearner::useNoApriori;
  using gum::learning::genericBNLearner::useAprioriSmoothing;
  using gum::learning::genericBNLearner::useAprioriDirichlet;
  using gum::learning::genericBNLearner::useAprioriBDeu;
  using gum::learning::genericBNLearner::useGreedyHillClimbing;
  using gum::learning::genericBNLearner::useLocalSearchWithTabuList;
  using gum::learning::genericBNLearner::useK2;
  using gum::learning::genericBNLearner::setMaxIndegree;
  using gum::learning::genericBNLearner::setSliceOrder;
  using gum::learning::genericBNLearner::setPossibleSkeleton;
  using gum::learning::genericBNLearner::addPossibleEdge;
  using gum::learning::genericBNLearner::erasePossibleEdge;
  using gum::learning::genericBNLearner::addForbiddenArc;
  using gum::learning::genericBNLearner::eraseForbiddenArc;
  using gum::learning::genericBNLearner::addMandatoryArc;
  using gum::learning::genericBNLearner::addMandatoryArc;
  using gum::learning::genericBNLearner::eraseMandatoryArc;
  using gum::learning::genericBNLearner::useEM;
  using gum::learning::genericBNLearner::hasMissingValues;
  using gum::learning::genericBNLearner::logLikelihood;
  using gum::learning::genericBNLearner::rawPseudoCount;
  using gum::learning::genericBNLearner::nbRows;
  using gum::learning::genericBNLearner::nbCols;
  using gum::learning::genericBNLearner::domainSize;
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
ADD_ARCGRAPHPART_API(gum::DiGraph);
ADD_ARCGRAPHPART_API(gum::DAG);
ADD_ARCGRAPHPART_API(gum::MixedGraph);


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
ADD_MULTIDIMDECORATOR_API(gum::Potential<double>)

#####################################
#####################################
%define ADD_CREDALINFERENCEENGINCE_API(classname)
%extend classname  {
  void setRepetitiveInd(const bool flag) {
    self->gum::credal::InferenceEngine<double>::setRepetitiveInd(flag);
  }
 const std::vector< double >& marginalMax ( const NodeId id ) const {
    return self->gum::credal::InferenceEngine<double>::marginalMax(id);
  }
 const std::vector< double >& marginalMin ( const NodeId id ) const {
    return self->gum::credal::InferenceEngine<double>::marginalMin(id);
  }
 const std::vector< double >& marginalMax ( const std::string name ) const {
    return self->gum::credal::InferenceEngine<double>::marginalMax(name);
  }
 const std::vector< double >& marginalMin ( const std::string name) const {
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
}
%enddef
ADD_CREDALINFERENCEENGINCE_API(gum::credal::CNMonteCarloSampling<double>)
ADD_CREDALINFERENCEENGINCE_API(gum::credal::CNLoopyPropagation<double>)


#####################################
#####################################
%extend gum::learning::BNLearner<double> {
  void setInitialDAG( const gum::DAG& g) {
    self->gum::learning::genericBNLearner::setInitialDAG(g);
  }
  void use3off2() {
    self->gum::learning::genericBNLearner::use3off2();
  }
  void useMIIC() {
    self->gum::learning::genericBNLearner::useMIIC();
  }
  void useNML(){
    self->gum::learning::genericBNLearner::useNML();
  }
  void useMDL(){
    self->gum::learning::genericBNLearner::useMDL();
  }
  void useNoCorr(){
    self->gum::learning::genericBNLearner::useNoCorr();
  }
  const std::vector<Arc> latentVariables() {
    return self->gum::learning::genericBNLearner::latentVariables();
  }
  gum::MixedGraph learnMixedStructure() {
    return self->gum::learning::genericBNLearner::learnMixedStructure();
  }
}



#####################################
#####################################
%define ADD_INFERENCE_API(baseclassname,classname...)
%extend classname  {
  void makeInference(void) {
    self->baseclassname::makeInference();
  }
  const Potential<double> posterior( const NodeId var ) {
    return self->baseclassname::posterior(var);
  }
  const Potential<double> posterior( const std::string& nodeName ) {
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

  Potential<double> evidenceImpact(NodeId target,const NodeSet& evs){
    return self->baseclassname::evidenceImpact(target,evs);
  }
  Potential<double> evidenceImpact(const std::string& target,const std::vector<std::string>& evs){
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
    const gum::Potential<double>& currentPosterior(const NodeId id)
        {return self->gum::SamplingInference<double>::currentPosterior(id);};
    const gum::Potential<double>& currentPosterior(const std::string& name)
        {return self->gum::SamplingInference<double>::currentPosterior(name);};
}
%enddef
ADD_SAMPLING_INFERENCE_API(gum::GibbsSampling<double>)
ADD_SAMPLING_INFERENCE_API(gum::MonteCarloSampling<double>)
ADD_SAMPLING_INFERENCE_API(gum::WeightedSampling<double>)
ADD_SAMPLING_INFERENCE_API(gum::LoopySamplingInference<double,gum::ImportanceSampling>)
ADD_SAMPLING_INFERENCE_API(gum::LoopySamplingInference<double,gum::GibbsSampling>)
ADD_SAMPLING_INFERENCE_API(gum::LoopySamplingInference<double,gum::WeightedSampling>)
ADD_SAMPLING_INFERENCE_API(gum::LoopySamplingInference<double,gum::MonteCarloSampling>)

%define ADD_JOINT_INFERENCE_API(classname)
ADD_BN_MONOTARGET_INFERENCE_API(gum::MarginalTargetedInference<double>,classname)
%extend classname {
  const Potential<double> posterior( const NodeId var ) {
    return self->JointTargetedInference<double>::posterior(var);
  }
  const Potential<double> posterior( const std::string nodeName ) {
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

  double I( const NodeId X, const NodeId Y ) {
    return self->gum::JointTargetedInference<double>::I(X,Y);
  }
  double VI( const NodeId X, const NodeId Y ) {
    return self->gum::JointTargetedInference<double>::VI(X,Y);
  }

  double I( const std::string& X, const std::string& Y ) {
    return self->gum::JointTargetedInference<double>::I(X,Y);
  }
  double VI( const std::string& X, const std::string& Y ) {
    return self->gum::JointTargetedInference<double>::VI(X,Y);
  }

  Potential<double> evidenceJointImpact(const NodeSet& targets,const NodeSet& evs){
    return self->gum::JointTargetedInference<double>::evidenceJointImpact(targets,evs);
  }
  Potential<double> evidenceJointImpact(const std::vector<std::string>& targets,const std::vector<std::string>& evs){
   return self->gum::JointTargetedInference<double>::evidenceJointImpact(targets,evs);
  }
}
%enddef
ADD_JOINT_INFERENCE_API(gum::LazyPropagation<double>)
ADD_JOINT_INFERENCE_API(gum::ShaferShenoyInference<double>)


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

%extend gum::Potential<double> {
  gum::Size domainSize() {return self->gum::MultiDimDecorator<double>::domainSize();}
  gum::Size nbrDim() {return self->gum::MultiDimDecorator<double>::nbrDim();}
}

#################
%define ADD_MN_INFERENCE_API(classname...)
ADD_MONOTARGET_INFERENCE_API (gum::MarginalTargetedMNInference<double>,classname)
%extend classname {
   const IMarkovNet<double>& MN() const { return self->gum::MarginalTargetedMNInference<double>::MN(); }
}
%enddef

%define ADD_JOINT_MN_INFERENCE_API(classname)
ADD_MN_INFERENCE_API(classname)
%extend classname {
  const Potential<double> posterior( const NodeId var ) {
    return self->JointTargetedMNInference<double>::posterior(var);
  }
  const Potential<double> posterior( const std::string nodeName ) {
    return self->JointTargetedMNInference<double>::posterior(nodeName);
  }
  void eraseAllTargets() {
    self->gum::JointTargetedMNInference<double>::eraseAllTargets();
  }
  void eraseAllJointTargets() {
    self->gum::JointTargetedMNInference<double>::eraseAllJointTargets();
  }
  void eraseAllMarginalTargets() {
    self->gum::JointTargetedMNInference<double>::eraseAllMarginalTargets();
  }

  gum::Size nbrJointTargets() {
    return self->gum::JointTargetedMNInference<double>::nbrJointTargets();
  }

  double I( const NodeId X, const NodeId Y ) {
    return self->gum::JointTargetedMNInference<double>::I(X,Y);
  }
  double VI( const NodeId X, const NodeId Y ) {
    return self->gum::JointTargetedMNInference<double>::VI(X,Y);
  }

  Potential<double> evidenceJointImpact(const NodeSet& targets,const NodeSet& evs){
    return self->gum::JointTargetedMNInference<double>::evidenceJointImpact(targets,evs);
  }
  Potential<double> evidenceJointImpact(const std::vector<std::string>& targets,const std::vector<std::string>& evs){
   return self->gum::JointTargetedMNInference<double>::evidenceJointImpact(targets,evs);
  }
}
%enddef
ADD_JOINT_MN_INFERENCE_API(gum::ShaferShenoyMNInference<double>)


#################
%define ADD_ID_INFERENCE_API(classname...)
%extend classname {
   void makeInference(void) {
     self->gum::InfluenceDiagramInference<double>::makeInference();
   }

   const InfluenceDiagram<double>& influenceDiagram() const { return self->gum::InfluenceDiagramInference<double>::influenceDiagram(); }
}
%enddef
ADD_ID_INFERENCE_API(gum::ShaferShenoyLIMIDInference<double >)
