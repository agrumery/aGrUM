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

%define IMPROVE_LIMID_INFERENCE_API(classname)
%feature("shadow") gum::classname<double>::setEvidence %{
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
        If one node does not belong to the influence diagram
    """
    if not isinstance(evidces, dict):
        raise TypeError("setEvidence parameter must be a dict, not %s"%(type(evidces)))
    self.eraseAllEvidence()
    for k,v in evidces.items():
        self.addEvidence(k,v)
%}

%feature("shadow") gum::classname<double>::updateEvidence %{
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

// these void class extensions are rewritten by "shadow" declarations
%extend gum::classname<double> {
    void setEvidence(PyObject *evidces) {}
    void updateEvidence(PyObject *evidces) {}

    PyObject* hardEvidenceNodes() {
      return PyAgrumHelper::PySetFromNodeSet(self->hardEvidenceNodes() ) ;
    }
    PyObject* softEvidenceNodes() {
      return PyAgrumHelper::PySetFromNodeSet(self->softEvidenceNodes() ) ;
    }
    PyObject* MEU() {
    std::cout<<"H"<<std::endl;
      return PyAgrumHelper::PyDictFromPairMeanVar(self->MEU() ) ;
    }
    PyObject* meanVar(gum::NodeId node) {
      return PyAgrumHelper::PyDictFromPairMeanVar(self->meanVar(node) ) ;
    }
}

%pythonappend gum::classname<double>::classname %{
   self._model=infDiag
%}

%pythonappend gum::classname<double>::junctionTree %{
   val._engine=self
%}

%enddef

IMPROVE_LIMID_INFERENCE_API(ShaferShenoyLIMIDInference)
