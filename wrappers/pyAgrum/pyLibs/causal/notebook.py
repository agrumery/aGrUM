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
  :param cm: the causal model
  :param size: passd
  :param vals:
  :return:
  """
  return gnb.getDot(cm.toDot())


def showCausalModel(cm: csl.CausalModel, size: str = "4"):
  """
  Shows a graphviz svg representation of the causal DAG ``d``
  """
  gnb.showDot(cm.toDot())


def getCausalImpact(model: csl.CausalModel, on: Union[str, NameSet], doing: Union[str, NameSet],
                    knowing: Optional[NameSet] = None, values: Optional[Dict[str, int]] = None) -> Tuple[
        str, "pyAgrum.Potential", str]:
  """
  return a HTML representing of the three values defining a causal impact : formula, value, explanation
  :param model: the causal model
  :param on: the impacted variable(s)
  :param doing: the variable(s) of intervention
  :param knowing: the variable(s) of evidence
  :param values : values for certain variables

  :return: a triplet (CausalFormula representation (string), pyAgrum.Potential, explanation)
  """
  formula, impact, explanation = csl.causalImpact(
      model, on, doing, knowing, values)
  return gnb.getSideBySide(getCausalModel(model),
                           "?" if formula is None else (
                               '$$\\begin{equation*}' + formula.toLatex() + '\\end{equation*}$$'),
                           "No result" if formula is None else impact,
                           captions=["Causal Model", "Explanation : " + explanation,
                                     "Impact : $" + ("?" if formula is None else formula.latexQuery(values)) + "$"])


def showCausalImpact(model: csl.CausalModel, on: Union[str, NameSet], doing: Union[str, NameSet],
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


csl.CausalModel._repr_html_ = lambda self: gnb.getDot(
    self.toDot(), size=pyAgrum.config['causal', 'default_graph_size'])
csl.CausalFormula._repr_html_ = lambda self: f"$${self.toLatex()}$$"
