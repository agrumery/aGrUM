
%define IMPROVE_INFERENCE_API(classname)
%feature("shadow") classname::setEvidence %{
def setEvidence(self, evidces):
    if not isinstance(evidces, dict):
        raise TypeError("setEvidence parameter must be a dict, not %s"%(type(evidces)))
    self.eraseAllEvidence()
    for k,v in evidces.items():
        self.addEvidence(k,v)
%}

%feature("shadow") classname::updateEvidence %{
def updateEvidence(self, evidces):
    if not isinstance(evidces, dict):
        raise TypeError("setEvidence parameter must be a dict, not %s"%(type(evidces)))

    for k,v in evidces.items():
        if self.hasEvidence(k):
            self.chgEvidence(k,v)
        else:
            self.addEvidence(k,v)
%}

%feature("shadow") classname::setTargets %{
def setTargets(self, targets):
    if not isinstance(targets, set):
        raise TypeError("setTargets parameter must be a set, not %s"%(type(targets)))

    self.eraseAllTargets()
    for k in targets:
        self.addTarget(k)
%}


// these void class extensions are rewritten by "shadow" declarations
%extend classname {
    void setEvidence(PyObject *evidces) {}
    void updateEvidence(PyObject *evidces) {}
    void setTargets(PyObject* targets) {}

    PyObject* hardEvidenceList() {
      return PyAgrumHelper::PyListFromNodeSet(self->hardEvidenceNodes() ) ;
    }
    PyObject* softEvidenceList() {
      return PyAgrumHelper::PyListFromNodeSet(self->softEvidenceNodes() ) ;
    }
    PyObject* targetList() {
      return PyAgrumHelper::PyListFromNodeSet(self->targets() );
    }
}
%enddef
IMPROVE_INFERENCE_API(gum::LazyPropagation<double>)
IMPROVE_INFERENCE_API(gum::GibbsInference<double>)


%define IMPROVE_JOINT_INFERENCE_API(classname)
%extend classname {
    Potential<double> jointPosterior(PyObject *list) {
      gum::NodeSet nodeset;
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(nodeset,list,self->BayesNet());
      return self->jointPosterior(nodeset);
    };

    void addJointTarget( PyObject* list ) {
      gum::NodeSet nodeset;
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(nodeset,list,self->BayesNet());
      self->gum::JointTargetedInference<double>::addJointTarget(nodeset);
    }

    void eraseJointTarget( PyObject* list ) {
      gum::NodeSet nodeset;
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(nodeset,list,self->BayesNet());
      self->gum::JointTargetedInference<double>::eraseJointTarget(nodeset);
    }

    bool isJointTarget( PyObject* list ) {
      gum::NodeSet nodeset;
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(nodeset,list,self->BayesNet());
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
