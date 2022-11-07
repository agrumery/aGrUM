# -*- coding: utf-8 -*-
# (c) Copyright by Pierre-Henri Wuillemin (LIP6), 2020  (pierre-henri.wuillemin@lip6.fr)

# Permission to use, copy, modify, and distribute this
# software and its documentation for any purpose and
# without fee or royalty is hereby granted, provided
# that the above copyright notice appear in all copies
# and that both that copyright notice and this permission
# notice appear in supporting documentation or portions
# thereof, including modifications, that you make.

# THE AUTHOR P.H. WUILLEMIN  DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
# SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT
# OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
# RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
# IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
# ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
# OR PERFORMANCE OF THIS SOFTWARE!

"""
This file defines some helpers for handling causal concepts in notebooks
"""
from typing import Union,Optional,Dict,Tuple
import IPython

import pyAgrum
import pyAgrum.lib.notebook as gnb
import pyAgrum.causal as csl

from pyAgrum.causal._types import NameSet



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
    size=pyAgrum.config['causal', 'default_graph_size']
  return gnb.getDot(cm.toDot(),size)


def showCausalModel(cm: csl.CausalModel, size= None):
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
    size=pyAgrum.config['causal', 'default_graph_size']
  gnb.showDot(cm.toDot(),size=size)


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
  gnb.flow.add(getCausalModel(model),caption="Causal Model")

  if formula is None:
    gnb.flow.add(explanation,caption="Impossible")
  else:
    gnb.flow.add('$$\\begin{equation*}' + formula.toLatex() + '\\end{equation*}$$',caption="Explanation : "+explanation )

  gnb.flow.add("No result" if formula is None else impact,caption="Impact") # : $" + ("?" if formula is None else formula.latexQuery(values)) + "$")

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
    self.toDot(), size=pyAgrum.config['causal', 'default_graph_size'])
csl.CausalFormula._repr_html_ = lambda self: f"$${self.toLatex()}$$"
