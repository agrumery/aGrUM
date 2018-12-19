%define IMPROVE_INFERENCE_API(classname...)
%feature("shadow") gum::classname::setEvidence %{
def setEvidence(self, evidces):
    """
    Erase all the evidences and apply addEvidence(key,value) for every pairs in evidces.

    Parameters
    ----------
    evidces : dict
      a dict of evidences

    Raises
    ------
    gum.InvalidArgument
        If one value is not a value for the node
    gum.InvalidArgument
        If the size of a value is different from the domain side of the node
    gum.FatalError
        If one value is a vector of 0s
    gum.UndefinedElement
        If one node does not belong to the Bayesian network
    """
    if not isinstance(evidces, dict):
        raise TypeError("setEvidence parameter must be a dict, not %s"%(type(evidces)))
    self.eraseAllEvidence()
    for k,v in evidces.items():
        self.addEvidence(k,v)
%}

%feature("shadow") gum::classname::updateEvidence %{
def updateEvidence(self, evidces):
    """
    Apply chgEvidence(key,value) for every pairs in evidces (or addEvidence).

    Parameters
    ----------
    evidces : dict
      a dict of evidences

    Raises
    ------
    gum.InvalidArgument
        If one value is not a value for the node
    gum.InvalidArgument
        If the size of a value is different from the domain side of the node
    gum.FatalError
        If one value is a vector of 0s
    gum.UndefinedElement
        If one node does not belong to the Bayesian network
    """
    if not isinstance(evidces, dict):
        raise TypeError("setEvidence parameter must be a dict, not %s"%(type(evidces)))

    for k,v in evidces.items():
        if self.hasEvidence(k):
            self.chgEvidence(k,v)
        else:
            self.addEvidence(k,v)
%}

%feature("shadow") gum::classname::setTargets %{
def setTargets(self, targets):
    """
    Remove all the targets and add the ones in parameter.

    Parameters
    ----------
    targets : set
      a set of targets

    Raises
    ------
    gum.UndefinedElement
        If one target is not in the Bayes net
    """
    if not isinstance(targets, set):
        raise TypeError("setTargets parameter must be a set, not %s"%(type(targets)))

    self.eraseAllTargets()
    for k in targets:
        self.addTarget(k)
%}

%ignore gum::classname::evidenceImpact(NodeId target, const NodeSet& evs);

// these void class extensions are rewritten by "shadow" declarations
%extend gum::classname {
    void setEvidence(PyObject *evidces) {}
    void updateEvidence(PyObject *evidces) {}
    void setTargets(PyObject* targets) {}

    PyObject* hardEvidenceNodes() {
      return PyAgrumHelper::PySetFromNodeSet(self->hardEvidenceNodes() ) ;
    }
    PyObject* softEvidenceNodes() {
      return PyAgrumHelper::PySetFromNodeSet(self->softEvidenceNodes() ) ;
    }
    PyObject* targets() {
      return PyAgrumHelper::PySetFromNodeSet(self->targets() );
    }
    Potential<double> evidenceImpact(NodeId target,PyObject *evs) {
      gum::NodeSet soe;
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(soe,evs,self->BN());
      return self->evidenceImpact(target,soe);
    }
}
%enddef

IMPROVE_INFERENCE_API(LazyPropagation<double>)
IMPROVE_INFERENCE_API(ShaferShenoyInference<double>)
IMPROVE_INFERENCE_API(VariableElimination<double>)
IMPROVE_INFERENCE_API(GibbsSampling<double>)
IMPROVE_INFERENCE_API(ImportanceSampling<double>)
IMPROVE_INFERENCE_API(WeightedSampling<double>)
IMPROVE_INFERENCE_API(MonteCarloSampling<double>)
IMPROVE_INFERENCE_API(LoopyBeliefPropagation<double>)
IMPROVE_INFERENCE_API(LoopySamplingInference<double,gum::GibbsSampling>)
IMPROVE_INFERENCE_API(LoopySamplingInference<double,gum::ImportanceSampling>)
IMPROVE_INFERENCE_API(LoopySamplingInference<double,gum::WeightedSampling>)
IMPROVE_INFERENCE_API(LoopySamplingInference<double,gum::MonteCarloSampling>)

%define IMPROVE_JOINT_INFERENCE_API(classname)
%ignore classname::evidenceJointImpact(const NodeSet& target, const NodeSet& evs);
%ignore classname::jointMutualInformation(const NodeSet &targets);
%extend classname {
    double jointMutualInformation(PyObject* targets) {
      gum::NodeSet sot;
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(sot,targets,self->BN());
      return self->jointMutualInformation(sot);
    }
    Potential<double> evidenceJointImpact(PyObject* targets,PyObject *evs) {
      gum::NodeSet sot;
      gum::NodeSet soe;
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(sot,targets,self->BN());
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(soe,evs,self->BN());
      return self->evidenceJointImpact(sot,soe);
    }
    Potential<double> jointPosterior(PyObject *list) {
      gum::NodeSet nodeset;
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(nodeset,list,self->BN());
      return self->jointPosterior(nodeset);
    };

    void addJointTarget( PyObject* list ) {
      gum::NodeSet nodeset;
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(nodeset,list,self->BN());
      self->gum::JointTargetedInference<double>::addJointTarget(nodeset);
    }

    void eraseJointTarget( PyObject* list ) {
      gum::NodeSet nodeset;
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(nodeset,list,self->BN());
      self->gum::JointTargetedInference<double>::eraseJointTarget(nodeset);
    }

    bool isJointTarget( PyObject* list ) {
      gum::NodeSet nodeset;
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(nodeset,list,self->BN());
      return self->gum::JointTargetedInference<double>::isJointTarget(nodeset);
    }

    PyObject* jointTargets() const {
      PyObject* q = PyList_New( 0 );

      for ( auto ns : self->JointTargetedInference<double>::jointTargets()) {
        PyList_Append( q,PyAgrumHelper::PyListFromNodeSet(ns));
      }
      return q;
    }
}
%enddef
IMPROVE_JOINT_INFERENCE_API(gum::LazyPropagation<double>)
IMPROVE_JOINT_INFERENCE_API(gum::ShaferShenoyInference<double>)
IMPROVE_JOINT_INFERENCE_API(gum::VariableElimination<double>)


// create a reference to python BN into python inference
%define IMPROVE_EXACT_INFERENCE_API(classname)
%pythonappend gum::classname<double>::classname %{
  self._bn=args[0] #BN
%}
%enddef
IMPROVE_EXACT_INFERENCE_API(LazyPropagation)
IMPROVE_EXACT_INFERENCE_API(ShaferShenoyInference)
IMPROVE_EXACT_INFERENCE_API(VariableElimination)


%define IMPROVE_APPROX_INFERENCE_API(constructor,classname...)
%pythonappend gum::classname::constructor %{
  self._bn=bn  #BN
%}
%enddef
IMPROVE_APPROX_INFERENCE_API(GibbsSampling,GibbsSampling<double>)
IMPROVE_APPROX_INFERENCE_API(ImportanceSampling,ImportanceSampling<double>)
IMPROVE_APPROX_INFERENCE_API(WeightedSampling,WeightedSampling<double>)
IMPROVE_APPROX_INFERENCE_API(MonteCarloSampling,MonteCarloSampling<double>)
IMPROVE_APPROX_INFERENCE_API(LoopyBeliefPropagation,LoopyBeliefPropagation<double>)
IMPROVE_APPROX_INFERENCE_API(LoopySamplingInference,LoopySamplingInference<double,gum::GibbsSampling>)
IMPROVE_APPROX_INFERENCE_API(LoopySamplingInference,LoopySamplingInference<double,gum::ImportanceSampling>)
IMPROVE_APPROX_INFERENCE_API(LoopySamplingInference,LoopySamplingInference<double,gum::WeightedSampling>)
IMPROVE_APPROX_INFERENCE_API(LoopySamplingInference,LoopySamplingInference<double,gum::MonteCarloSampling>)
