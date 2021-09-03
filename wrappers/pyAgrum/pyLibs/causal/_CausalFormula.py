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
This file defines a representation of a causal query in a causal model
"""

from collections import defaultdict
from typing import Union,Optional,Dict

import pyAgrum

from pyAgrum.causal._types import NameSet
from pyAgrum.causal._doAST import ASTtree

# pylint: disable=unused-import
import pyAgrum.causal  # for annotations


class CausalFormula:
  """
  Represents a causal query in a causal model. The query is encoded as an CausalFormula that can be evaluated in the
  causal model : $P(on|knowing, \\overhook (doing))$

  :param cm: the causal model
  :param root: the syntax tree as the root ASTtree
  :param on: the variable or the set of variables of interest
  :param doing: the intervention variables
  :param knowing: the observation variables
  """

  def __init__(self, cm: "pyAgrum.causal.CausalModel", root: ASTtree, on: Union[str, NameSet],
               doing: Union[str, NameSet],
               knowing: Optional[NameSet] = None):
    self._cm = cm
    self._root = root
    if isinstance(on, str):
      self._on = {on}
    else:
      self._on = on
    if isinstance(doing, str):
      self._doing = {doing}
    else:
      self._doing = doing
    if knowing is None:
      self._knowing = set()
    else:
      self._knowing = knowing

  def __str__(self, prefix: str = "") -> str:
    """
    :param prefix: a prefix for each line of the string representation
    :return:  the string version of the CausalFormula
    """
    return self.root.__str__(prefix)

  def latexQuery(self, values: Optional[Dict[str, str]] = None) -> str:
    """
    Returns a string representing the query compiled by this Formula. If values, the query is annotated with the
    values in the dictionary.

    :param values: the values to add in the query representation
    :return: the string representing the causal query for this CausalFormula
    """
    if values is None:
      values = {}

    def _getVarRepresentation(v: str) -> str:
      if v not in values:
        return v

      bn = self.cm.observationalBN()
      label = bn.variable(self.cm.idFromName(v)).label(
        _getLabelIdx(bn, v, values[v]))
      return v + "=" + label

    # adding values when necessary
    on = [_getVarRepresentation(k) for k in self._on]
    doing = [_getVarRepresentation(k) for k in self._doing]
    knowing = [_getVarRepresentation(k) for k in self._knowing]

    latexOn = ",".join(on)

    doOpPref = pyAgrum.config["causal", "latex_do_prefix"]
    doOpSuff = pyAgrum.config["causal", "latex_do_suffix"]
    latexDo = ""
    if len(doing) > 0:
      latexDo = ",".join([doOpPref + d + doOpSuff for d in doing])

    latexKnw = ""
    if len(knowing) > 0:
      if latexDo != "":
        latexKnw = ", "
      latexKnw += ",".join(knowing)

    return "P( " + latexOn + " \\mid " + latexDo + latexKnw + ")"

  def toLatex(self) -> str:
    """
    :return: a LaTeX representation of the CausalFormula
    """

    occur = defaultdict(int)
    for n in self._cm.observationalBN().nodes():
      occur[self._cm.observationalBN().variable(n).name()] = 0
    for n in self._doing:
      occur[n] = 1
    for n in self._knowing:
      occur[n] = 1
    for n in self._on:
      occur[n] = 1

    return self.latexQuery() + " = " + self._root.toLatex(occur)

  def copy(self) -> "CausalFormula":
    """
    Copy theAST. Note that the causal model is just referenced. The tree is copied.

    :return: the new CausalFormula
    """
    return CausalFormula(self.cm, self.root.copy(), self._on, self._doing, self._knowing)

  @property
  def cm(self) -> "pyAgrum.causal.CausalModel":
    """
    :return: the causal model
    """
    return self._cm

  @property
  def root(self) -> ASTtree:
    """
    :return: ASTtree root of the CausalFormula tree
    """
    return self._root

  def eval(self) -> "pyAgrum.Potential":
    """
    Compute the Potential from the CausalFormula over vars using cond as value for others variables

    :return:
    """
    return self.root.eval(self.cm.observationalBN())


def _getLabelIdx(bn: "pyAgrum.BayesNet", varname: str, val: Union[int, str]) -> int:
  """
  Find the index of a label in a discrete variable from a BN.

  If val is an int, we keep is as is. If it is a str, we try to find the correct index in the variable
  :param bn: the BN where to find the variable
  :param varname: the name of the variable
  :param val: the index or the name of the label

  :return: the index of the label
  """
  if not isinstance(val, str):
    return val

  return bn.variableFromName(varname).index(val)
