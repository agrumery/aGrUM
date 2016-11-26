// this file is for giving access to methods defined in template ancestror.
// SWIG does not allow to use "using" for this kind of methods.

#####################################

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

  void populate ( const std::vector< double >& v ) const {
    self->gum::MultiDimDecorator<double>::populate(v);
  }

  void fill ( const double& d ) const {
    self->gum::MultiDimDecorator<double>::fill(d);
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
ADD_MULTIDIMDECORATOR_API(gum::UtilityTable<double>)


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
ADD_CREDALINFERENCEENGINCE_API(%arg(gum::credal::CNMonteCarloSampling<double, gum::LazyPropagation<double> >))
ADD_CREDALINFERENCEENGINCE_API(gum::credal::CNLoopyPropagation<double>)


#####################################
#####################################
%extend gum::learning::BNLearner<double> {
  void setInitialDAG( const gum::DAG& g) {
    self->gum::learning::genericBNLearner::setInitialDAG(g);
  }
}



#####################################
#####################################
%define ADD_INFERENCE_API(classname)
%extend classname  {
  void makeInference(void) {
    self->gum::BayesNetInference<double>::makeInference();
  }
  const Potential<double> posterior( const NodeId var ) {
    return self->MarginalTargetedInference<double>::posterior(var);
  }
  const Potential<double> posterior( const std::string nodeName ) {
    return self->MarginalTargetedInference<double>::posterior(nodeName);
  }
  const IBayesNet<double>& BayesNet() const {
    return self->BayesNetInference<double>::BayesNet();
  }

//######## EVIDENCE ##########
//############################
  void addEvidence( const NodeId id, const Idx val ) {
    self->gum::BayesNetInference<double>::addEvidence(id,val);
  }
  void addEvidence( const std::string& nodeName, const Idx val ) {
    self->gum::BayesNetInference<double>::addEvidence(nodeName,val);
  }
  void addEvidence( const NodeId id, const std::string& val ) {
    self->gum::BayesNetInference<double>::addEvidence(id,val);
  }

  void addEvidence( const std::string& nodeName, const std::string& val ) {
    self->gum::BayesNetInference<double>::addEvidence(nodeName,val);
  }

  void addEvidence( const NodeId id,const std::vector<double>& vals ) {
   self->gum::BayesNetInference<double>::addEvidence(id,vals);
  }
  void addEvidence( const std::string& nodeName,
                                const std::vector<double>& vals ) {
    self->gum::BayesNetInference<double>::addEvidence(nodeName,vals);
  }


  void chgEvidence( const NodeId id, const Idx val ) {
    self->gum::BayesNetInference<double>::chgEvidence(id,val);
  }
  void chgEvidence( const std::string& nodeName, const Idx val ) {
    self->gum::BayesNetInference<double>::chgEvidence(nodeName,val);
  }

  void chgEvidence( const NodeId id, const std::string& val ) {
    self->gum::BayesNetInference<double>::chgEvidence(id,val);
  }
  void chgEvidence( const std::string& nodeName, const std::string& val ) {
    self->gum::BayesNetInference<double>::chgEvidence(nodeName,val);
  }


  void chgEvidence( const NodeId id,const std::vector<double>& vals ) {
   self->gum::BayesNetInference<double>::chgEvidence(id,vals);
  }
  void chgEvidence( const std::string& nodeName,
                                const std::vector<double>& vals ) {
    self->gum::BayesNetInference<double>::chgEvidence(nodeName,vals);
  }

  bool hasEvidence( const NodeId id ) const {
    return self->gum::BayesNetInference<double>::hasEvidence(id);
  }
  bool hasEvidence( const std::string& nodeName) const {
    return self->gum::BayesNetInference<double>::hasEvidence(nodeName);
  }

  void eraseAllEvidence() {
    self->gum::BayesNetInference<double>::eraseAllEvidence();
  }

  void eraseEvidence( const NodeId id ) {
    self->gum::BayesNetInference<double>::eraseEvidence(id);
  }
  void eraseEvidence( const std::string& nodeName  ) {
    self->gum::BayesNetInference<double>::eraseEvidence(nodeName);
  }

  bool hasSoftEvidence( const NodeId id ) const {
    return self->gum::BayesNetInference<double>::hasSoftEvidence(id);
  }

  bool hasHardEvidence( const std::string& nodeName ) const {
    return self->gum::BayesNetInference<double>::hasHardEvidence(nodeName);
  }

  bool hasSoftEvidence( const std::string& nodeName ) const {
    return self->gum::BayesNetInference<double>::hasSoftEvidence(nodeName);
  }

  gum::Size nbrEvidence() const {
    return self->gum::BayesNetInference<double>::nbrEvidence();
  }

  gum::Size nbrHardEvidence() const {
    return self->gum::BayesNetInference<double>::nbrHardEvidence();
  }

  gum::Size nbrSoftEvidence() const {
    return self->gum::BayesNetInference<double>::nbrSoftEvidence();
  }


//######## TARGETS ##########
//############################
    void eraseAllTargets() {
      self->gum::MarginalTargetedInference<double>::eraseAllTargets();
    }

    void addAllTargets() {
      self->gum::MarginalTargetedInference<double>::addAllTargets();
    }

    void addTarget( const NodeId target ) {
      self->gum::MarginalTargetedInference<double>::addTarget(target);
    }

    void addTarget( const std::string& nodeName ) {
      self->gum::MarginalTargetedInference<double>::addTarget(nodeName);
    }

    void eraseTarget( const NodeId target ) {
      self->gum::MarginalTargetedInference<double>::eraseTarget(target);
    }

    void eraseTarget( const std::string& nodeName ) {
      self->gum::MarginalTargetedInference<double>::eraseTarget(nodeName);
    }

    bool isTarget( const NodeId variable ) const {
      return self->gum::MarginalTargetedInference<double>::isTarget(variable);
    }

    bool isTarget( const std::string& nodeName ) const {
      return self->gum::MarginalTargetedInference<double>::isTarget(nodeName);
    }

    gum::Size nbrTargets( ) const {
      return self->gum::MarginalTargetedInference<double>::nbrTargets();
    }

   double H( const NodeId X ) {
     return self->gum::MarginalTargetedInference<double>::H(X);
   }
   double H( const std::string& nodeName ) {
     return self->gum::MarginalTargetedInference<double>::H(nodeName);
   }
}
%enddef
ADD_INFERENCE_API(gum::LazyPropagation<double>)
ADD_INFERENCE_API(gum::GibbsInference<double>)

%define ADD_JOINT_INFERENCE_API(classname)
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
}
%enddef
ADD_JOINT_INFERENCE_API(gum::LazyPropagation<double>)


