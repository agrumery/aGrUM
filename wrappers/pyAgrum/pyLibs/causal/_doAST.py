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
This file defines the needed class for the representation of an abstract syntax tree for causal formula
"""

from collections import defaultdict
from typing import Union, Dict, Optional, Iterable, List

import pyAgrum
from pyAgrum.causal._types import NameSet

# pylint: disable=unused-import
import pyAgrum.causal  # for annotations


class ASTtree:
  """
  Represents a generic node for the CausalFormula. The type of the node will be registered in a string.

  Parameters
  ----------
  typ: str
    the type of the node (will be specified in concrete children classes.
  verbose: bool
    if True, add some messages
  """

  def __init__(self, typ: str, verbose=False):
    """
    Represents a generic node for the CausalFormula. The type of the node will be registered in a string.

    Parameters
    ----------
    typ: str
      the type of the node (will be specified in concrete children classes.
    verbose: bool
      if True, add some messages
    """
    self._type = typ
    self.__continueNextLine = "| "
    self._verbose = verbose

  @property
  def _continueNextLine(self):
    return self.__continueNextLine

  @property
  def type(self) -> str:
    """
    Returns
    -------
    str
      the type of the node
    """
    return self._type

  def __str__(self, prefix: str = "") -> str:
    """
    stringify a CausalFormula tree

    Parameters
    ----------
    prefix: str
      a prefix for each line of the string representation

    Returns
    -------
    str
      the string version of the tree
    """
    raise NotImplementedError

  def protectToLatex(self, nameOccur: Dict[str, int]) -> str:
    """
    Create a protected LaTeX representation of a ASTtree

    Parameters
    ----------
    nameOccur: Dict[str,int]
      the number of occurrence for each variable

    Returns
    -------
    str
      a protected version of LaTeX representation of the tree
    """
    raise NotImplementedError

  def fastToLatex(self, nameOccur: Dict[str, int]) -> str:
    """
    Internal virtual function to create a LaTeX representation of the ASTtree

    Parameters
    ----------
    nameOccur: Dict[str,int]
      the number of occurrence for each variable

    Returns
    -------
    str
      LaTeX representation of the tree
    """
    raise NotImplementedError

  def toLatex(self, nameOccur: Optional[Dict[str, int]] = None) -> str:
    """
    Create a LaTeX representation of a ASTtree

    Parameters
    ----------
    nameOccur: Dict[str,int] default=None
      the number of occurrence for each variable

    Returns
    -------
    str
      LaTeX representation of the tree
    """
    if nameOccur is None:
      nameOccur = defaultdict(int)
    return self.fastToLatex(nameOccur)

  @staticmethod
  def _latexCorrect(srcName: Union[str, Iterable[str]], nameOccur: Dict[str, int]) -> Union[str, Iterable[str]]:
    """
    Change the latex presentation of variable w.r.t the number of occurrence of this variable : for instance,
    add primes when necessary

    Parameters
    ----------
    srcName: str
      the name or an iterable containing a collection of names
    nameOccur: Dict[str,int]
      the dict that gives the number of occurrence for each variable (default value 0 if the variable
    is not a key in this dict)

    Returns
    -------
    str | Iterable[str]
      the corrected name or the list of corrected names
    """

    def __transform(v: str) -> str:
      nbr = max(0, nameOccur[v] - 1)
      return v + ("'" * nbr)

    if isinstance(srcName, str):
      return __transform(srcName)

    return sorted([__transform(v) for v in srcName])

  def copy(self) -> "ASTtree":
    """
    Copy an CausalFormula tree

    Returns
    -------
    ASTtree
      the new causal tree
    """
    raise NotImplementedError

  def eval(self, contextual_bn: "pyAgrum.BayesNet") -> "pyAgrum.Potential":
    """
    Evaluation of a AST tree from inside a BN

    Parameters
    ----------
    contextual_bn: pyAgrum.BayesNet
      the observational Bayesian network in which will be done the computations

    Returns
    -------
    pyAgrum.Potential
      the resulting Potential
    """
    raise NotImplementedError


class ASTBinaryOp(ASTtree):
  """
  Represents a generic binary node for the CausalFormula. The op1 and op2 are the two operands of the class.

  Parameters
  ----------
  typ: str
    the type of the node (will be specified in concrete children classes
  op1: ASTtree
    left operand
  op2: ASTtree
    right operand
  """

  def __init__(self, typ: str, op1: ASTtree, op2: ASTtree):
    """
    Represents a generic binary node for the CausalFormula. The op1 and op2 are the two operands of the class.

    Parameters
    ----------
    typ: str
      the type of the node (will be specified in concrete children classes
    op1: ASTtree
      left operand
    op2: ASTtree
      right operand
    """
    super().__init__(typ)
    self._op1: ASTtree = op1
    self._op2: ASTtree = op2

  def protectToLatex(self, nameOccur: Dict[str, int]) -> str:
    raise NotImplementedError

  def fastToLatex(self, nameOccur: Dict[str, int]) -> str:
    raise NotImplementedError

  def copy(self) -> "ASTtree":
    raise NotImplementedError

  def eval(self, contextual_bn: "pyAgrum.BayesNet") -> "pyAgrum.Potential":
    raise NotImplementedError

  @property
  def op1(self) -> ASTtree:
    """
    Returns
    -------
    ASTtree
      the left operand
    """
    return self._op1

  @property
  def op2(self) -> ASTtree:
    """
    Returns
    -------
    ASTtree
      the right operand
    """
    return self._op2

  def __str__(self, prefix: str = "") -> str:
    return f"""{prefix}{self.type}\n{self.op1.__str__(prefix + self._continueNextLine)}
{self.op2.__str__(prefix + self._continueNextLine)}"""


class ASTplus(ASTBinaryOp):
  """
  Represents the sum of 2 :class:`causal.ASTtree`

  Parameters
  ----------
  op1: ASTtree
    left operand
  op2: ASTtree
    right operand
  """

  def __init__(self, op1: ASTtree, op2: ASTtree):
    """
    Represents the sum of 2 :class:`causal.ASTtree`

    Parameters
    ----------
    op1: ASTtree
      left operand
    op2: ASTtree
      right operand
    """
    super().__init__('+', op1, op2)

  def copy(self) -> "ASTtree":
    return ASTplus(self.op1.copy(), self.op2.copy())

  def protectToLatex(self, nameOccur: Dict[str, int]) -> str:
    return f"\\left({self.fastToLatex(nameOccur)}\\right)"

  def fastToLatex(self, nameOccur: Dict[str, int]) -> str:
    return self.op1.fastToLatex(nameOccur) + '+' + self.op2.fastToLatex(nameOccur)

  def eval(self, contextual_bn: "pyAgrum.BayesNet") -> "pyAgrum.Potential":
    if self._verbose:
      print("EVAL operation + ", flush=True)
    res = self.op1.eval(contextual_bn) + self.op2.eval(contextual_bn)

    if self._verbose:
      print(f"END OF EVAL operation  : {res}", flush=True)

    return res


class ASTminus(ASTBinaryOp):
  """
  Represents the substraction of 2 :class:`causal.ASTtree`

  Parameters
  ----------
  op1: ASTtree
    left operand
  op2: ASTtree
    right operand
  """

  def __init__(self, op1: ASTtree, op2: ASTtree):
    """
    Represents the substraction of 2 :class:`causal.ASTtree`

    Parameters
    ----------
    op1: ASTtree
      left operand
    op2: ASTtree
      right operand
    """
    super().__init__('-', op1, op2)

  def copy(self) -> "ASTtree":
    return ASTminus(self.op1.copy(), self.op2.copy())

  def protectToLatex(self, nameOccur: Dict[str, int]) -> str:
    return "\\left(" + self.fastToLatex(nameOccur) + "\\right)"

  def fastToLatex(self, nameOccur: Dict[str, int]) -> str:
    return self.op1.fastToLatex(nameOccur) + '-' + self.op2.fastToLatex(nameOccur)

  def eval(self, contextual_bn: "pyAgrum.BayesNet") -> "pyAgrum.Potential":
    if self._verbose:
      print("EVAL operation", flush=True)
    res = self.op1.eval(contextual_bn) - self.op2.eval(contextual_bn)

    if self._verbose:
      print(f"END OF EVAL operation : {res}", flush=True)

    return res


class ASTmult(ASTBinaryOp):
  """
  Represents the multiplication of 2 :class:`causal.ASTtree`

  Parameters
  ----------
  op1: ASTtree
    left operand
  op2: ASTtree
    right operand
  """

  def __init__(self, op1: ASTtree, op2: ASTtree):
    """
    Represents the multiplication of 2 :class:`causal.ASTtree`

    Parameters
    ----------
    op1: ASTtree
      left operand
    op2: ASTtree
      right operand
    """
    super().__init__('*', op1, op2)

  def copy(self) -> "ASTtree":
    return ASTmult(self.op1.copy(), self.op2.copy())

  def protectToLatex(self, nameOccur: Dict[str, int]) -> str:
    return self.fastToLatex(nameOccur)

  def fastToLatex(self, nameOccur: Dict[str, int]) -> str:
    return self.op1.protectToLatex(nameOccur) + ' \\cdot ' + self.op2.protectToLatex(nameOccur)

  def eval(self, contextual_bn: "pyAgrum.BayesNet") -> "pyAgrum.Potential":
    if self._verbose:
      print("EVAL operation * in context", flush=True)
    res = self.op1.eval(contextual_bn) * self.op2.eval(contextual_bn)

    if self._verbose:
      print(f"END OF EVAL operation * : {res}", flush=True)

    return res


class ASTdiv(ASTBinaryOp):
  """
  Represents the division of 2 :class:`causal.ASTtree`

  Parameters
  ----------
  op1: ASTtree
    left operand
  op2: ASTtree
    right operand
  """

  def __init__(self, op1: ASTtree, op2: ASTtree):
    """
    Represents the division of 2 :class:`causal.ASTtree`

    Parameters
    ----------
    op1: ASTtree
      left operand
    op2: ASTtree
      right operand
    """
    super().__init__("/", op1, op2)

  def copy(self) -> "ASTtree":
    return ASTdiv(self.op1.copy(), self.copy(self.op2.copy()))

  def protectToLatex(self, nameOccur: Dict[str, int]) -> str:
    return self.fastToLatex(nameOccur)

  def fastToLatex(self, nameOccur: Dict[str, int]) -> str:
    return " \\frac {" + self.op1.fastToLatex(nameOccur) + "}{" + self.op2.fastToLatex(nameOccur) + "}"

  def eval(self, contextual_bn: "pyAgrum.BayesNet") -> "pyAgrum.Potential":
    if self._verbose:
      print("EVAL operation / in context", flush=True)
    res = self.op1.eval(contextual_bn) / self.op2.eval(contextual_bn)

    if self._verbose:
      print(f"END OF EVAL operation / : {res}", flush=True)

    return res


class ASTposteriorProba(ASTtree):
  """
  Represent a conditional probability :math:`P_{bn}(vars|knw)` that can be computed by an inference in a BN.

  Parameters
  ----------
  bn: pyAgrum.BayesNet
    the :class:`pyAgrum:pyAgrum.BayesNet`
  varset: Set[str]
    a set of variable names (in the BN) conditioned in the posterior
  knw: Set[str]
    a set of variable names (in the BN) conditioning in the posterior
  """

  def __init__(self, bn: "pyAgrum.BayesNet", varset: NameSet, knw: NameSet):
    """
    Represent a conditional probability :math:`P_{bn}(vars|knw)` that can be computed by an inference in a BN.

    Parameters
    ----------
    bn: pyAgrum.BayesNet
      the :class:`pyAgrum:pyAgrum.BayesNet`
    varset: Set[str]
      a set of variable names (in the BN) conditioned in the posterior
    knw: Set[str]
      a set of variable names (in the BN) conditioning in the posterior
    """
    super().__init__("_posterior_")
    if not isinstance(varset, set):
      raise ValueError("'varset' must be a set")
    if not isinstance(knw, set):
      raise ValueError("'knw' must be a set")

    self._vars = varset
    self._bn = bn
    minKnames = {bn.variable(i).name() for i in bn.minimalCondSet(varset, knw)}
    self._knw = minKnames

  @property
  def vars(self) -> NameSet:
    """
    Returns
    -------
    Set[str]
      (Conditioned) vars in :math:`P_{bn}(vars|knw)`
    """
    return self._vars

  @property
  def knw(self) -> NameSet:
    """
    Returns
    -------
    Set[str]
      (Conditioning) knw in :math:`P_{bn}(vars|knw)`
    """
    return self._knw

  @property
  def bn(self) -> "pyAgrum.BayesNet":
    """
    Returns
    -------
    pyAgrum.BayesNet
      the observationnal BayesNet in :math:`P_{bn}(vars|knw)`
    """
    return self._bn

  def __str__(self, prefix: str = "") -> str:
    s = "P("
    s += ','.join(sorted(self.vars))
    if self.knw is not None:
      s += "|"
      s += ','.join(sorted(self.knw))
    s += ")"
    return f"{prefix}{s}"

  def protectToLatex(self, nameOccur: Dict[str, int]) -> str:
    return self.fastToLatex(nameOccur)

  def fastToLatex(self, nameOccur: Dict[str, int]) -> str:
    s = "P\\left(" + ",".join(self._latexCorrect(self.vars, nameOccur))
    if self.knw is not None and len(self.knw) > 0:
      s += "\\mid "
      s += ",".join(self._latexCorrect(self.knw, nameOccur))

    s += "\\right)"

    return s

  def copy(self) -> "ASTtree":
    return ASTposteriorProba(self.bn, self.vars, self.knw)

  def eval(self, contextual_bn: "pyAgrum.BayesNet") -> "pyAgrum.Potential":
    if self._verbose:
      print(f"EVAL ${self.fastToLatex(defaultdict(int))} in context", flush=True)
    ie = pyAgrum.LazyPropagation(contextual_bn)
    p = None

    # simple case : we just need a CPT from the BN
    if len(self.vars) == 1:
      for x in self.vars:
        break  # we keep the first one and only one
      ix = contextual_bn.idFromName(x)
      if {contextual_bn.variable(i).name() for i in contextual_bn.parents(ix)} == self.knw:
        p = contextual_bn.cpt(ix)

    if p is None:
      if len(self.knw) == 0:
        ie.addJointTarget(self.vars)
        ie.makeInference()
        p = ie.jointPosterior(self.vars)
      else:
        ie.addJointTarget(self.vars | self.knw)
        ie.makeInference()
        p = ie.jointPosterior(self.vars | self.knw) / ie.jointPosterior(self.knw)

    #
    # res = p.extract({k: v for k, v in context.todict().items() if k in self.vars + self.knw})

    if self._verbose:
      print(f"END OF EVAL ${self.fastToLatex(defaultdict(int))}$ : {p}", flush=True)

    return p


class ASTjointProba(ASTtree):
  """
  Represent a joint probability in the base observational part of the :class:`causal.CausalModel`

  Parameters
  ----------
  varNames: Set[str]
    a set of variable names
  """

  def __init__(self, varNames: NameSet):
    """
    Represent a joint probability in the base observational part of the :class:`causal.CausalModel`

    Parameters
    ----------
    varNames: Set[str]
      a set of variable names
    """
    super().__init__("_joint_")
    self._varNames = varNames

  @property
  def varNames(self) -> NameSet:
    """
    Returns
    -------
    Set[str]
      the set of names of var
    """
    return self._varNames

  def __str__(self, prefix: str = "") -> str:
    s = "P("
    s += ",".join(sorted(self._varNames))
    s += ")"
    return f"{prefix}joint {s}"

  def copy(self) -> "ASTtree":
    return ASTjointProba(self.varNames)

  def protectToLatex(self, nameOccur: Dict[str, int]) -> str:
    return self.fastToLatex(nameOccur)

  def fastToLatex(self, nameOccur: Dict[str, int]) -> str:
    return "P\\left(" + ",".join(self._latexCorrect(self.varNames, nameOccur)) + "\\right)"

  def eval(self, contextual_bn: "pyAgrum.BayesNet") -> "pyAgrum.Potential":
    if self._verbose:
      print(f"EVAL ${self.fastToLatex(defaultdict(int))}$ in context", flush=True)
    ie = pyAgrum.LazyPropagation(contextual_bn)
    if len(self.varNames) > 1:
      svars = set(self.varNames)
      ie.addJointTarget(svars)
      ie.makeInference()
      res = ie.jointPosterior(svars)
    else:
      for name in self.varNames:
        break  # take the first and only one name in varNames
      ie.makeInference()
      res = ie.posterior(name)

    if self._verbose:
      print(f"END OF EVAL ${self.fastToLatex(defaultdict(int))}$ : {res}", flush=True)

    return res


class ASTsum(ASTtree):
  """
  Represents a sum over a variable of a :class:`causal.ASTtree`.

  Parameters
  ----------
  var: str
    name of the variable on which to sum
  term: ASTtree
    the tree to be evaluated
  """

  def __init__(self, var: str, term: ASTtree):
    """
    Represents a sum over a variable of a :class:`causal.ASTtree`.

    Parameters
    ----------
    var: str
      name of the variable on which to sum
    term: ASTtree
      the tree to be evaluated
    """
    super().__init__("_sum_")

    va = var if isinstance(var, list) else [var]
    self.var = va[0]

    if len(va) > 1:
      self._term = ASTsum(va[1:], term)
    else:
      self._term = term

  @property
  def term(self) -> ASTtree:
    """
    Returns
    -------
    ASTtree
      the term to sum
    """
    return self._term

  def __str__(self, prefix: str = "") -> str:
    l = []
    a = self
    while a.type == "_sum_":
      l.append(a.var)
      a = a.term
    return f'{prefix}sum on {",".join(sorted(l))} for\n{a.__str__(prefix + self._continueNextLine)}'

  def copy(self) -> "ASTtree":
    return ASTsum(self.var, self.term.copy())

  def protectToLatex(self, nameOccur: Dict[str, int]) -> str:
    return "\\left(" + self.fastToLatex(nameOccur) + "\\right)"

  def fastToLatex(self, nameOccur: Dict[str, int]) -> str:
    la = []
    a = self
    while a.type == "_sum_":
      la.append(a.var)
      nameOccur[a.var] += 1
      a = a.term

    res = "\\sum_{" + (",".join(self._latexCorrect(la, nameOccur))) + "}{" + a.fastToLatex(nameOccur) + "}"
    for v in la:
      nameOccur[v] -= 1

    return res

  def eval(self, contextual_bn: "pyAgrum.BayesNet") -> "pyAgrum.Potential":
    if self._verbose:
      print(f"EVAL ${self.fastToLatex(defaultdict(int))}$", flush=True)

    res = self.term.eval(contextual_bn).margSumOut([self.var])

    if self._verbose:
      print(f"END OF EVAL ${self.fastToLatex(defaultdict(int))}$ : {res}", flush=True)

    return res


def productOfTrees(lterms: List[ASTtree]) -> ASTtree:
  """
  create an ASTtree for a sequence of multiplications of ASTtree

  Parameters
  ----------
  lterms: List[ASTtree]
    the trees (as ASTtree) to multiply

  Returns
  -------
  ASTtree
    the ASTtree representing the tree of multiplications

  """
  if len(lterms) == 1:
    return lterms[0]
  return ASTmult(lterms[0], productOfTrees(lterms[1:]))
