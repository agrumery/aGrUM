############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#                                                                          #
#   The aGrUM/pyAgrum library is free software; you can redistribute it    #
#   and/or modify it under the terms of either :                           #
#                                                                          #
#    - the GNU Lesser General Public License as published by               #
#      the Free Software Foundation, either version 3 of the License,      #
#      or (at your option) any later version,                              #
#    - the MIT license (MIT),                                              #
#    - or both in dual license, as here.                                   #
#                                                                          #
#   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    #
#                                                                          #
#   This aGrUM/pyAgrum library is distributed in the hope that it will be  #
#   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          #
#   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS #
#   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   #
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER #
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        #
#   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  #
#   OTHER DEALINGS IN THE SOFTWARE.                                        #
#                                                                          #
#   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   #
#   licence (LICENSE.MIT) for more details.                                #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

"""
This file defines some helpers for handling causal concepts in notebooks
"""
from typing import Union, Optional, Dict, Tuple
import IPython

import pyagrum
import pyagrum.lib.notebook as gnb
import pyagrum.causal as csl

from pyagrum.causal._types import NameSet


def getCausalModel(cm: csl.CausalModel, size=None) -> str:
  """
  return a HTML representing the causal model

  Parameters
  ----------
  cm: CausalModel
    the causal model
  size: int|str
    the size of the rendered graph

  Returns
  -------
  pydot.Dot
    the dot representation
  """
  if size is None:
    size = pyagrum.config['causal', 'default_graph_size']
  return gnb.getDot(cm.toDot(), size)


def showCausalModel(cm: csl.CausalModel, size=None):
  """
  Shows a pydot svg representation of the causal DAG

  Parameters
  ----------
  cm: CausalModel
    the causal model
  size: int|str
    the size of the rendered graph
  """
  if size is None:
    size = pyagrum.config['causal', 'default_graph_size']
  gnb.showDot(cm.toDot(), size=size)


def getCausalImpact(model: csl.CausalModel, on: Union[str, NameSet], doing: Union[str, NameSet],
                    knowing: Optional[NameSet] = None, values: Optional[Dict[str, int]] = None):
  """
  return a HTML representing of the three values defining a causal impact : formula, value, explanation

  Parameters
  ----------
  model: CausalModel
    the causal model
  on: str | Set[str]
    the impacted variable(s)
  doing: str | Set[str]
    the interventions
  knowing: str | Set[str]
    the observations
  values: Dict[str,int] default=None
    value for certain variables

  Returns
  -------
  HTML
  """
  formula, impact, explanation = csl.causalImpact(model, on, doing, knowing, values)

  gnb.flow.clear()
  gnb.flow.add(getCausalModel(model), caption="Causal Model")

  if formula is None:
    gnb.flow.add(explanation, caption="Impossible")
  else:
    gnb.flow.add('$$\\begin{equation*}' + formula.toLatex() + '\\end{equation*}$$',
                 caption="Explanation : " + explanation)

  if formula is None:
    res="No result"
  else:
    if impact.variable(0).domainSize()<5:
      res=impact
    else:
      res=gnb.getProba(impact)
  gnb.flow.add(res,caption="Impact")

  return gnb.flow.html()


def showCausalImpact(model: csl.CausalModel, on: Union[str, NameSet], doing: Union[str, NameSet],
                     knowing: Optional[NameSet] = None, values: Optional[Dict[str, int]] = None):
  """
  display a HTML representing of the three values defining a causal impact :  formula, value, explanation

  Parameters
  ----------
  model: CausalModel
    the causal model
  on: str | Set[str]
    the impacted variable(s)
  doing: str | Set[str]
    the interventions
  knowing: str | Set[str]
    the observations
  values: Dict[str,int] default=None
    value for certain variables
  """
  html = getCausalImpact(model, on, doing, knowing, values)
  IPython.display.display(html)


csl.CausalModel._repr_html_ = lambda self: gnb.getDot(
  self.toDot(), size=pyagrum.config['causal', 'default_graph_size'])
csl.CausalFormula._repr_html_ = lambda self: f"$${self.toLatex()}$$"
