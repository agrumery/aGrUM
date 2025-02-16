/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
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
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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
