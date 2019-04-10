# -*- coding: utf-8 -*-
#(c) Copyright by Pierre-Henri Wuillemin, UPMC, 2011  (pierre-henri.wuillemin@lip6.fr)

#Permission to use, copy, modify, and distribute this
#software and its documentation for any purpose and
#without fee or royalty is hereby granted, provided
#that the above copyright notice appear in all copies
#and that both that copyright notice and this permission
#notice appear in supporting documentation or portions
#thereof, including modifications, that you make.

#THE AUTHOR P.H. WUILLEMIN  DISCLAIMS ALL WARRANTIES
#WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
#WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
#SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT
#OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
#RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
#IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
#ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
#OR PERFORMANCE OF THIS SOFTWARE!

"""
This file defines some helpers for handling causal concepts in notebooks
"""
from ._types import *

import IPython
import pyAgrum.lib.notebook as gnb
import pydotplus as dot

from ._CausalModel import CausalModel
from ._causalImpact import causalImpact


def getCausalModel(cm: CausalModel, size: str = "4") -> str:
  """
  return a HTML representing the causal model
  :param cm: the causal model
  :param size: passd
  :param vals:
  :return:
  """
  graph = dot.Dot(graph_type='digraph')
  for n in cm.nodes():
    if n not in cm.latentVariablesIds():
      bgcol = "#444444"
      fgcol = "#FFFFFF"
      st = "filled"
      shap = "ellipse"
    else:
      bgcol = "#FF0000"
      fgcol = "#000000"
      st = "dashed"
      shap="point"

    graph.add_node(dot.Node(cm.names()[n],
                            shape=shap,
                            style=st,
                            fillcolor=bgcol,
                            fontcolor=fgcol))

  for a, b in cm.arcs():
    graph.add_edge(dot.Edge(cm.names()[a], cm.names()[b],
                            style="dashed" if a in cm.latentVariablesIds() else "filled"))

  graph.set_size(size)

  return IPython.display.SVG(graph.create_svg()).data


def showCausalModel(cm: CausalModel, size: str = "4"):
  """
  Shows a graphviz svg representation of the causal DAG ``d``
  """
  html = getCausalModel(cm, size)
  IPython.display.display(IPython.display.HTML("<div align='center'>" + html + "</div>"))


def getCausalImpact(model: CausalModel, on: Union[str, NameSet], doing: Union[str, NameSet],
                    knowing: Optional[NameSet] = None, values: Optional[Dict[str, int]] = None) -> Tuple[
  str, gum.Potential, str]:
  """
  return a HTML representing of the three values defining a causal impact : formula, value, explanation
  :param model: the causal model
  :param on: the impacted variable(s)
  :param doing: the variable(s) of intervention
  :param knowing: the variable(s) of evidence
  :param values : values for certain variables

  :return: a triplet (CausalFormula, gum.Potential, explanation)
  """
  formula, impact, explanation = causalImpact(model, on, doing, knowing, values)
  return gnb.getSideBySide(getCausalModel(model),
                           "?" if formula is None else (
                               '$$\\begin{equation}' + formula.toLatex() + '\\end{equation}$$'),
                           "No result" if formula is None else impact,
                           captions=["Causal Model", "Explanation : " + explanation,
                                     "Impact : $" + ("?" if formula is None else formula.latexQuery(values)) + "$"])


def showCausalImpact(model: CausalModel, on: Union[str, NameSet], doing: Union[str, NameSet],
                     knowing: Optional[NameSet] = None, values: Optional[Dict[str, int]] = None):
  """
  display a HTML representing of the three values defining a causal impact :  formula, value, explanation
  :param model: the causal model
  :param on: the impacted variable(s)
  :param doing: the variable(s) of intervention
  :param knowing: the variable(s) of evidence
  :param values : values for certain variables
  """
  html = getCausalImpact(model, on, doing, knowing, values)
  IPython.display.display(IPython.display.HTML(html))
