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

%define IMPROVE_MN_INFERENCE_API(classname...)
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
%ignore gum::classname::evidenceImpact(const std::string& target, const std::vector<std::string>& evs);

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
    Potential<double> evidenceImpact(PyObject* target,PyObject *evs) {
      gum::NodeId itarget=PyAgrumHelper::nodeIdFromNameOrIndex(target,self->MN().variableNodeMap());
      gum::NodeSet soe;
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(soe,evs,self->MN().variableNodeMap());
      return self->evidenceImpact(itarget,soe);
    }
}
%enddef
IMPROVE_MN_INFERENCE_API(ShaferShenoyMNInference<double>)

%define IMPROVE_JOINT_MN_INFERENCE_API(classname)
%ignore classname::evidenceJointImpact(const NodeSet& target, const NodeSet& evs);
%ignore classname::jointMutualInformation(const NodeSet &targets);
%extend classname {
    double jointMutualInformation(PyObject* targets) {
      gum::NodeSet sot;
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(sot,targets,self->MN().variableNodeMap());
      return self->jointMutualInformation(sot);
    }
    Potential<double> evidenceJointImpact(PyObject* targets,PyObject *evs) {
      gum::NodeSet sot;
      gum::NodeSet soe;
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(sot,targets,self->MN().variableNodeMap());
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(soe,evs,self->MN().variableNodeMap());
      return self->evidenceJointImpact(sot,soe);
    }
    Potential<double> jointPosterior(PyObject *targets) {
      if (! PyAnySet_Check(targets)) {
        GUM_ERROR(gum::InvalidArgument,"The argument must be a set");
      }
      gum::NodeSet nodeset;
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(nodeset,targets,self->MN().variableNodeMap());
      return self->jointPosterior(nodeset);
    };

    void addJointTarget( PyObject* targets ) {
      if (! PyAnySet_Check(targets)) {
        GUM_ERROR(gum::InvalidArgument,"The argument must be a set");
      }
      gum::NodeSet nodeset;
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(nodeset,targets,self->MN().variableNodeMap());

      self->gum::JointTargetedMNInference<double>::addJointTarget(nodeset);
    }
    void eraseJointTarget( PyObject* targets ) {
      if (! PyAnySet_Check(targets)) {
        GUM_ERROR(gum::InvalidArgument,"The argument must be a set");
      }
      gum::NodeSet nodeset;
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(nodeset,targets,self->MN().variableNodeMap());
      self->gum::JointTargetedMNInference<double>::eraseJointTarget(nodeset);
    }

    bool isJointTarget( PyObject* targets ) {
      if (! PyAnySet_Check(targets)) {
        GUM_ERROR(gum::InvalidArgument,"The argument must be a set");
      }
      gum::NodeSet nodeset;
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(nodeset,targets,self->MN().variableNodeMap());
      return self->gum::JointTargetedMNInference<double>::isJointTarget(nodeset);
    }

    PyObject* jointTargets() const {
      PyObject* q = PyList_New( 0 );

      for ( auto ns : self->JointTargetedMNInference<double>::jointTargets()) {
        PyList_Append( q,PyAgrumHelper::PySetFromNodeSet(ns));
      }
      return q;
    }
}
%enddef
IMPROVE_JOINT_MN_INFERENCE_API(gum::ShaferShenoyMNInference<double>)


// create a reference to python BN into python inference
%define IMPROVE_EXACT_MN_INFERENCE_API(classname)
%pythonappend gum::classname<double>::classname %{
  self._model=MN # first arg of the constructor
%}
%pythonappend gum::classname<double>::junctionTree %{
   val._engine=self
%}
%enddef
IMPROVE_EXACT_MN_INFERENCE_API(ShaferShenoyMNInference)


