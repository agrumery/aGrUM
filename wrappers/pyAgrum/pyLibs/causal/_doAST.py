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
This file defines the needed class for the representation of an abstract syntax tree for causal formula
"""

from collections import defaultdict

from ._exceptions import *


class ASTtree:
  """
  Represents a generic node for the CausalFormula. The type of the node will be registered in a string.

  :param type: the type of the node (will be specified in concrete children classes.
  """

  def __init__(self, type: str, verbose=False):
    self._type = type
    self.__continueNextLine = "| "
    self._verbose = verbose

  @property
  def _continueNextLine(self):
    return self.__continueNextLine

  @property
  def type(self) -> str:
    """
    :return: the type of the node
    """
    return self._type

  def __str__(self, prefix: str = "") -> str:
    """
    stringify a CausalFormula tree

    :param prefix: a prefix for each line of the string representation
    :return:  the string version of the tree
    """
    raise NotImplementedError

  def _protectToLatex(self, nameOccur: Dict[str, int]) -> str:
    """
    Create a protected LaTeX representation of a ASTtree

    :return: the LaTeX string
    """
    raise NotImplementedError

  def _toLatex(self, nameOccur: Dict[str, int]) -> str:
    """
    Internal virtual function to create a LaTeX representation of a ASTtree

    :return: the LaTeX string
    """
    raise NotImplementedError

  def toLatex(self, nameOccur: Optional[Dict[str, int]] = None) -> str:
    """
    Create a LaTeX representation of a ASTtree

    :return: the LaTeX string
    """
    if nameOccur is None:
      nameOccur = defaultdict(int)
    return self._toLatex(nameOccur)

  def _latexCorrect(self, srcName: Union[str, Iterable[str]], nameOccur: Dict[str, int]) -> Union[str, Iterable[str]]:
    """
    Change the latex presentation of variable w.r.t the number of occurrence of this variable : for instance,
    add primes when necessary

    :param srcName: the name or an iterable containing a collection of names
    :param nameOccur: the dict that gives the number of occurrence for each variable (default value 0 if the variable
    is not a key in this dict)
    :return: the corrected name or the list of corrected names
    """

    def __transform(v: str) -> str:
      nbr = max(0, nameOccur[v] - 1)
      return v + ("'" * nbr)

    if isinstance(srcName, str):
      return __transform(srcName)
    else:
      return sorted([__transform(v) for v in srcName])

  def copy(self) -> "ASTtree":
    """
    Copy an CausalFormula tree

    :return: the new causal tree
    """
    raise NotImplementedError

  def eval(self, contextual_bn: gum.BayesNet) -> gum.Potential:
    raise NotImplementedError


class ASTBinaryOp(ASTtree):
  """
  Represents a generic binary node for the CausalFormula. The op1 and op2 are the two operands of the class.

  :param type: the type of the node (will be specified in concrete children classes
  :param op1: left operand
  :param op2: right operand
  """

  def __init__(self, type: str, op1: ASTtree, op2: ASTtree):
    super().__init__(type)
    self._op1: ASTtree = op1
    self._op2: ASTtree = op2

  @property
  def op1(self) -> ASTtree:
    """
    :return: the left operand
    """
    return self._op1

  @property
  def op2(self) -> ASTtree:
    """
    :return: the right operand
    """
    return self._op2

  def __str__(self, prefix: str = "") -> str:
    """
    stringify a CausalFormula tree

    :param prefix: a prefix for each line of the string representation
    :return:  the string version of the tree
    """
    return "{}{}\n{}\n{}".format(prefix, self.type, self.op1.__str__(prefix + self._continueNextLine),
                                 self.op2.__str__(prefix + self._continueNextLine))


class ASTplus(ASTBinaryOp):
  """
  Represents the sum of 2 :class:`causal.ASTtree`

  :param op1: first operand
  :param op2: second operand
  """

  def __init__(self, op1: ASTtree, op2: ASTtree):
    super().__init__('+', op1, op2)

  def copy(self) -> "ASTtree":
    """
    Copy an CausalFormula tree

    :return: the new CausalFormula tree
    """
    return ASTplus(self.op1.copy(), self.op2.copy())

  def _protectToLatex(self, nameOccur: Dict[str, int]) -> str:
    """
    Create a protected LaTeX representation of a ASTtree

    :return: the LaTeX string
    """
    return "\\left(" + self._toLatex(nameOccur) + "\\right)"

  def _toLatex(self, nameOccur: Dict[str, int]) -> str:
    """
    Create a LaTeX representation of a ASTtree

    :return: the LaTeX string
    """
    return self.op1._toLatex(nameOccur) + '+' + self.op2._toLatex(nameOccur)

  def eval(self, contextual_bn: gum.BayesNet) -> gum.Potential:
    if self._verbose:
      print(f"EVAL operation + ", flush=True)
    res = self.op1.eval(contextual_bn) + self.op2.eval(contextual_bn)

    if self._verbose:
      print(f"END OF EVAL operation  : {res}", flush=True)

    return res


class ASTminus(ASTBinaryOp):
  """
  Represents the substraction of 2 :class:`causal.ASTtree`

  :param op1: first operand
  :param op2: second operand
  """

  def __init__(self, op1: ASTtree, op2: ASTtree):
    super().__init__('-', op1, op2)

  def copy(self) -> "ASTtree":
    """
    Copy an CausalFormula tree

    :return: the new CausalFormula tree
    """
    return ASTminus(self.op1.copy(), self.op2.copy())

  def _protectToLatex(self, nameOccur: Dict[str, int]) -> str:
    """
    Create a protected LaTeX representation of a ASTtree

    :return: the LaTeX string
    """
    return "\\left(" + self._toLatex(nameOccur) + "\\right)"

  def _toLatex(self, nameOccur: Dict[str, int]) -> str:
    """
    Create a LaTeX representation of a ASTtree

    :return: the LaTeX string
    """
    return self.op1._toLatex(nameOccur) + '-' + self.op2._toLatex(nameOccur)

  def eval(self, contextual_bn: gum.BayesNet) -> gum.Potential:
    if self._verbose:
      print(f"EVAL operation", flush=True)
    res = self.op1.eval(contextual_bn) - self.op2.eval(contextual_bn)

    if self._verbose:
      print(f"END OF EVAL operation : {res}", flush=True)

    return res


class ASTmult(ASTBinaryOp):
  """
  Represents the multiplication of 2 :class:`causal.ASTtree`

  :param op1: first operand
  :param op2: second operand
  """

  def __init__(self, op1: ASTtree, op2: ASTtree):
    super().__init__('*', op1, op2)

  def copy(self) -> "ASTtree":
    """
    Copy an CausalFormula tree

    :return: the new CausalFormula tree
    """
    return ASTmult(self.op1.copy(), self.op2.copy())

  def _protectToLatex(self, nameOccur: Dict[str, int]) -> str:
    """
    Create a protected LaTeX representation of a ASTtree

    :return: the LaTeX string
    """
    return self._toLatex(nameOccur)

  def _toLatex(self, nameOccur: Dict[str, int]) -> str:
    """
    Create a LaTeX representation of a ASTtree

    :return: the LaTeX string
    """
    return self.op1._protectToLatex(nameOccur) + ' \cdot ' + self.op2._protectToLatex(nameOccur)

  def eval(self, contextual_bn: gum.BayesNet) -> gum.Potential:
    if self._verbose:
      print(f"EVAL operation * in context {context}", flush=True)
    res = self.op1.eval(contextual_bn) * self.op2.eval(contextual_bn)

    if self._verbose:
      print(f"END OF EVAL operation * : {res}", flush=True)

    return res


class ASTdiv(ASTBinaryOp):
  """
  Represents the division of 2 :class:`causal.ASTtree`

  :param op1: first operand
  :param op2: second operand
  """

  def __init__(self, op1: ASTtree, op2: ASTtree):
    super().__init__("/", op1, op2)

  def copy(self) -> "ASTtree":
    """
    Copy an CausalFormula tree

    :return: the new CausalFormula tree
    """
    return ASTdiv(self.op1.copy(), self.copy(self.op2.copy()))

  def _protectToLatex(self, nameOccur: Dict[str, int]) -> str:
    """
    Create a protected LaTeX representation of a ASTtree

    :return: the LaTeX string
    """
    return self._toLatex(nameOccur)

  def _toLatex(self, nameOccur: Dict[str, int]) -> str:
    """
    Create a LaTeX representation of a ASTtree

    :return: the LaTeX string
    """
    return " \\frac {" + self.op1._toLatex(nameOccur) + "}{" + self.op2._toLatex(nameOccur) + "}"

  def eval(self, contextual_bn: gum.BayesNet) -> gum.Potential:
    if self._verbose:
      print(f"EVAL operation / in context {context}", flush=True)
    res = self.op1.eval(contextual_bn) / self.op2.eval(contextual_bn)

    if self._verbose:
      print(f"END OF EVAL operation / : {res}", flush=True)

    return res


class ASTposteriorProba(ASTtree):
  """
  Represent a conditional probability :math:`P_{bn}(vars|knw)` that can be computed by an inference in a BN.

  :param bn: the :class:`pyAgrum:pyAgrum.BayesNet`
  :param vars: a set of variable names (in the BN)
  :param knw: a set of variable names (in the BN)
  """

  def __init__(self, bn: gum.BayesNet, vars: NameSet, knw: NameSet):
    super().__init__("_posterior_")
    if type(vars) is not set:
      raise (ValueError, "'vars' must be a set")
    if type(knw) is not set:
      raise (ValueError, "'knw' must be a set")

    self._vars = vars
    self._bn = bn
    minKnames = {bn.variable(i).name() for i in
                 bn.minimalCondSet(vars, knw)}
    self._knw = minKnames

  @property
  def vars(self) -> NameSet:
    """
    :return: vars in :math:`P_{bn}(vars|knw)`
    """
    return self._vars

  @property
  def knw(self) -> NameSet:
    """
    :return: knw in :math:`P_{bn}(vars|knw)`
    """
    return self._knw

  @property
  def bn(self) -> gum.BayesNet:
    """
    :return: bn in :math:`P_{bn}(vars|knw)`
    """
    return self._bn

  def __str__(self, prefix: str = "") -> str:
    """
    stringify a CausalFormula tree

    :param prefix: a prefix for each line of the string representation
    :return:  the string version of the tree
    """
    s = "P("
    s += ','.join(sorted(self.vars))
    if self.knw is not None:
      s += "|"
      s += ','.join(sorted(self.knw))
    s += ")"
    return "{}{}".format(prefix, s)

  def _protectToLatex(self, nameOccur: Dict[str, int]) -> str:
    """
    Create a protected LaTeX representation of a ASTtree

    :return: the LaTeX string
    """
    return self._toLatex(nameOccur)

  def _toLatex(self, nameOccur: Dict[str, int]) -> str:
    """
    Create a LaTeX representation of a ASTtree

    :return: the LaTeX string
    """
    s = "P\\left(" + ",".join(self._latexCorrect(self.vars, nameOccur))
    if self.knw is not None and len(self.knw) > 0:
      s += "\\mid "
      s += ",".join(self._latexCorrect(self.knw, nameOccur))

    s += "\\right)"

    return s

  def copy(self) -> "ASTtree":
    """
    Copy an CausalFormula tree

    :return: the new CausalFormula tree
    """
    return ASTposteriorProba(self.bn, self.vars, self.knw)

  def eval(self, contextual_bn: gum.BayesNet) -> gum.Potential:
    if self._verbose:
      print(f"EVAL ${self._toLatex(defaultdict(int))} in context {context}", flush=True)
    ie = gum.LazyPropagation(contextual_bn)
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
      print(f"END OF EVAL ${self._toLatex(defaultdict(int))}$ : {p}", flush=True)

    return p


class ASTjointProba(ASTtree):
  """
  Represent a joint probability in the base observational part of the :class:`causal.CausalModel`

  :param varNames: a set of variable names
  """

  def __init__(self, varNames: NameSet):
    super().__init__("_joint_")
    self._varNames = varNames

  @property
  def varNames(self) -> NameSet:
    """
    :return: the set of names of var
    """
    return self._varNames

  def __str__(self, prefix: str = "") -> str:
    """
    stringify a CausalFormula tree

    :param prefix: a prefix for each line of the string representation
    :return:  the string version of the tree
    """
    s = "P("
    s += ",".join(sorted(self._varNames))
    s += ")"
    return "{}joint {}".format(prefix, s)

  def copy(self) -> "ASTtree":
    """
    Copy an CausalFormula tree

    :return: the new CausalFormula tree
    """
    return ASTjointProba(self.varNames)

  def _protectToLatex(self, nameOccur: Dict[str, int]) -> str:
    """
    Create a protected LaTeX representation of a ASTtree

    :return: the LaTeX string
    """
    return self._toLatex(nameOccur)

  def _toLatex(self, nameOccur: Dict[str, int]) -> str:
    """
    Create a LaTeX representation of a ASTtree
    :return: the LaTeX string
    """
    return "P\\left(" + ",".join(self._latexCorrect(self.varNames, nameOccur)) + "\\right)"

  def eval(self, contextual_bn: gum.BayesNet) -> gum.Potential:
    if self._verbose:
      print(f"EVAL ${self._toLatex(defaultdict(int))}$ in context {context}", flush=True)
    ie = gum.LazyPropagation(contextual_bn)
    if len(self.varNames) > 1:
      svars = {name for name in self.varNames}
      ie.addJointTarget(svars)
      ie.makeInference()
      res = ie.jointPosterior(svars)
    else:
      for name in self.varNames:
        break  # take the first and only one name in varNames
      ie.makeInference()
      res = ie.posterior(name)

    if self._verbose:
      print(f"END OF EVAL ${self._toLatex(defaultdict(int))}$ : {res}", flush=True)

    return res


class ASTsum(ASTtree):
  """
  Represents a sum over a variable of a :class:`causal.ASTtree`.

  :param var: name of the variable
  :param term: the tree to be evaluated
  """

  def __init__(self, var: List[str], term: ASTtree):
    super().__init__("_sum_")

    va = (var if type(var) == list else [var])
    self.var = va[0]

    if len(va) > 1:
      self._term = ASTsum(va[1:], term)
    else:
      self._term = term

  @property
  def term(self):
    return self._term

  def __str__(self, prefix: str = "") -> str:
    """
    stringify a CausalFormula tree

    :param prefix: a prefix for each line of the string representation
    :return:  the string version of the tree
    """
    l = []
    a = self
    while a.type == "_sum_":
      l.append(a.var)
      a = a.term
    return "{}sum on {} for \n{}".format(prefix, ",".join(sorted(l)), a.__str__(prefix + self._continueNextLine))

  def copy(self) -> "ASTtree":
    """
    Copy an CausalFormula tree

    :return: the new CausalFormula tree
    """

    return ASTsum(self.var, self.term.copy())

  def _protectToLatex(self, nameOccur: Dict[str, int]) -> str:
    """
    Create a protected LaTeX representation of a ASTtree

    :return: the LaTeX string
    """
    return "\\left(" + self._toLatex(nameOccur) + "\\right)"

  def _toLatex(self, nameOccur: Dict[str, int]) -> str:
    """
    Create a LaTeX representation of a ASTtree

    :return: the LaTeX string
    """
    la = []
    a = self
    while a.type == "_sum_":
      la.append(a.var)
      nameOccur[a.var] += 1
      a = a.term

    res = "\\sum_{" + (",".join(self._latexCorrect(la, nameOccur))) + "}{" + a._toLatex(nameOccur) + "}"
    for v in la:
      nameOccur[v] -= 1

    return res

  def eval(self, contextual_bn: gum.BayesNet) -> gum.Potential:
    """
    Evaluation of the sum

    :param contextual_bn: BN where to infer
    :return: the value of the sum
    """
    if self._verbose:
      print(f"EVAL ${self._toLatex(defaultdict(int))}$", flush=True)

    res = self.term.eval(contextual_bn).margSumOut([self.var])

    if self._verbose:
      print(f"END OF EVAL ${self._toLatex(defaultdict(int))}$ : {res}", flush=True)

    return res


def productOfTrees(lterms: List[ASTtree]) -> ASTtree:
  """
  create an ASTtree for a sequence of multiplications of ASTtree
  :param lterms: the trees (as ASTtree)
  :return: the ASTtree representing the tree of multiplications
  """
  if len(lterms) == 1:
    return lterms[0]
  return ASTmult(lterms[0], productOfTrees(lterms[1:]))
