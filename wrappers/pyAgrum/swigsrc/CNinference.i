/**
 *
 *  Copyright 2005-2023 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
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


%define IMPROVE_CN_INFERENCE_API(classname)

%pythonappend gum::credal::classname<double>::classname %{
  self._model=credalNet
              %}
%extend gum::credal::classname<double> {
  %pythoncode %{
def setEvidence(self, evidces):
  """
  Erase all the evidences and apply addEvidence(key,value) for every pairs in evidces.

  Parameters
  ----------
  evidces : Dict[str,Union[int,str,List[float]]] or List[pyAgrum.Potential]
   a dict of "name:evidence" where name is a string (the name of the variable) and evidence is an integer (an index) or a string (a label) or a list of float (a likelihood).

  Raises
  ------
  pyAgrum.InvalidArgument
     If one value is not a value for the node
   pyAgrum.InvalidArgument
     If the size of a value is different from the domain side of the node
   pyAgrum.FatalError
     If one value is a vector of 0s
   pyAgrum.UndefinedElement
     If one node does not belong to the Bayesian network
  """
  if isinstance(evidces, dict):
    self.eraseAllEvidence()
    for k,v in evidces.items():
      self.addEvidence(k,v)
    return
  elif isinstance(evidces, list): # should be a list of Potential
    self.eraseAllEvidence()
    for p in evidces:
      self.addEvidence(p)
    return
  raise TypeError("Parameter must be a dict or a list, not %s"%(type(evidces)))
%}
}
%enddef

IMPROVE_CN_INFERENCE_API(CNLoopyPropagation)
IMPROVE_CN_INFERENCE_API(CNMonteCarloSampling)
