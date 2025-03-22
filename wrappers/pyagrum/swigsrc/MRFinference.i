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





%define IMPROVE_MRF_INFERENCE_API(classname...)
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
    pyagrum.InvalidArgument
        If one value is not a value for the node
    pyagrum.InvalidArgument
        If the size of a value is different from the domain side of the node
    pyagrum.FatalError
        If one value is a vector of 0s
    pyagrum.UndefinedElement
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
    pyagrum.InvalidArgument
        If one value is not a value for the node
    pyagrum.InvalidArgument
        If the size of a value is different from the domain side of the node
    pyagrum.FatalError
        If one value is a vector of 0s
    pyagrum.UndefinedElement
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
    pyagrum.UndefinedElement
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
    Tensor<double> evidenceImpact(PyObject* target,PyObject *evs) {
      gum::NodeId itarget=PyAgrumHelper::nodeIdFromNameOrIndex(target,self->MRF().variableNodeMap());
      gum::NodeSet soe;
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(soe,evs,self->MRF().variableNodeMap());
      return self->evidenceImpact(itarget,soe);
    }
}
%enddef
IMPROVE_MRF_INFERENCE_API(ShaferShenoyMRFInference<double>)

%define IMPROVE_JOINT_MRF_INFERENCE_API(classname)
%ignore classname::evidenceJointImpact(const NodeSet& target, const NodeSet& evs);
%ignore classname::jointMutualInformation(const NodeSet &targets);
%extend classname {
    double jointMutualInformation(PyObject* targets) {
      gum::NodeSet sot;
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(sot,targets,self->MRF().variableNodeMap());
      return self->jointMutualInformation(sot);
    }
    Tensor<double> evidenceJointImpact(PyObject* targets,PyObject *evs) {
      gum::NodeSet sot;
      gum::NodeSet soe;
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(sot,targets,self->MRF().variableNodeMap());
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(soe,evs,self->MRF().variableNodeMap());
      return self->evidenceJointImpact(sot,soe);
    }
    Tensor<double> jointPosterior(PyObject *targets) {
      if (! PyAnySet_Check(targets)) {
        GUM_ERROR(gum::InvalidArgument,"The argument must be a set")
      }
      gum::NodeSet nodeset;
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(nodeset,targets,self->MRF().variableNodeMap());
      return self->jointPosterior(nodeset);
    };

    void addJointTarget( PyObject* targets ) {
      if (! PyAnySet_Check(targets)) {
        GUM_ERROR(gum::InvalidArgument,"The argument must be a set")
      }
      gum::NodeSet nodeset;
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(nodeset,targets,self->MRF().variableNodeMap());

      self->gum::JointTargetedMRFInference<double>::addJointTarget(nodeset);
    }
    void eraseJointTarget( PyObject* targets ) {
      if (! PyAnySet_Check(targets)) {
        GUM_ERROR(gum::InvalidArgument,"The argument must be a set")
      }
      gum::NodeSet nodeset;
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(nodeset,targets,self->MRF().variableNodeMap());
      self->gum::JointTargetedMRFInference<double>::eraseJointTarget(nodeset);
    }

    bool isJointTarget( PyObject* targets ) {
      if (! PyAnySet_Check(targets)) {
        GUM_ERROR(gum::InvalidArgument,"The argument must be a set")
      }
      gum::NodeSet nodeset;
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(nodeset,targets,self->MRF().variableNodeMap());
      return self->gum::JointTargetedMRFInference<double>::isJointTarget(nodeset);
    }

    PyObject* jointTargets() const {
      PyObject* q = PyList_New( 0 );

      PyObject* pyval;
      for ( auto ns : self->JointTargetedMRFInference<double>::jointTargets()) {
        pyval=PyAgrumHelper::PySetFromNodeSet(ns);
        PyList_Append( q, pyval );
        Py_DecRef( pyval );
      }
      return q;
    }
}
%enddef
IMPROVE_JOINT_MRF_INFERENCE_API(gum::ShaferShenoyMRFInference<double>)


// create a reference to python BN into python inference
%define IMPROVE_EXACT_MRF_INFERENCE_API(classname)
%pythonappend gum::classname<double>::classname %{
  self._model=MN # first arg of the constructor
%}
%pythonappend gum::classname<double>::junctionTree %{
   val._engine=self
%}
%enddef
IMPROVE_EXACT_MRF_INFERENCE_API(ShaferShenoyMRFInference)
