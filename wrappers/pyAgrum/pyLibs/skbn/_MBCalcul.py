# (c) Copyright 2020-2024 by Pierre-Henri Wuillemin(@LIP6)  (pierre-henri.wuillemin@lip6.fr)
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

import pyAgrum as gum
from ._utils import _listIdtoName as listIdtoName


def compileMarkovBlanket(bn, target):
  """
  Create a Bayesian network with the children, their parents and the parents of the node target

  Parameters
  ----------
      bn: pyAgrum.BayesNet
          Bayesian network to work on
      target: str or int
          Name or id of the target
  Returns
  -------
      MarkovBlanket: pyAgrum.BayesNet
          Markov Blanket from bn
  """
  mb = gum.BayesNet('MarkovBlanket')

  # add target to Markov Blanket
  mb.add(bn.variable(target))

  # list of target's children
  children = listIdtoName(bn, list(bn.children(target)))

  # list of target's parents
  parents = listIdtoName(bn, list(bn.parents(target)))

  for c in children:
    # list of c's parents
    parents_child = listIdtoName(bn, list(bn.parents(c)))

    # if c is not already in Markov Blanket
    if (c not in mb.names()):
      # add c in Markov Blanket
      mb.add(bn.variable(c))

      # create arc between target and his child c
      mb.addArc(target, c)

    # add c's parents in Markov Blanket
    for pc in parents_child:
      # if pc is a target's parent
      if (pc in mb.names()):
        if (pc != target):
          mb.addArc(pc, c)
        continue

      # add pc in Markov Blanket
      mb.add(bn.variable(pc))

      # if pc is not a children, his cpt doesn't matter (use for predict)
      if pc not in children:
        mb.cpt(pc).fillWith(1).normalize()
      else:
        mb.addArc(target, pc)

      # create arc between c and his parent pc
      mb.addArc(pc, c)

  for p in parents:
    # if p is not already in Markov Blanket
    if (p in mb.names()):
      # create arc between target and his parent p
      mb.addArc(p, target)
      continue

    # add p in Markov Blanket
    mb.add(bn.variable(p))

    # cpt doesn't matter for parents
    mb.cpt(p).fillWith(1).normalize()

    # create arc between target and his parent p
    mb.addArc(p, target)

  # update cpt for target and his children
  mb.cpt(target).fillWith(bn.cpt(target))
  for i in children:
    mb.cpt(i).fillWith(bn.cpt(i))

  return mb


def _calcul_proba_for_nary_class(row, local_inst, dictName, MarkovBlanket, target):
  """
  Calculate the posterior distribution of variable target (given its Markov blanket)

  Parameters
  ----------
      row: numpyArray shape: (n features)
          test data
      local_inst: pyAgrum.Potential
          Instantiation of the Markov Blanket EXCEPT the target
      dictName: Dict[str : int]
          dictionary of the name of a variable and his column in the data base
      MarkovBlanket: pyAgrum.BayesNet
          Markov Blanket to work on
      target: str
          Name of the target
  Returns
  -------
      proba:
          the probability distribution for target
  """
  # create Instantiation with Markov Blanket's variables
  for n in MarkovBlanket.names():
    if n == target:
      continue
    local_inst.chgVal(n, str(row[dictName.get(n)]))

  p = MarkovBlanket.cpt(target).extract(local_inst)
  for i in MarkovBlanket.children(target):
    p *= MarkovBlanket.cpt(i).extract(local_inst)
  p.normalize()

  return p


def _calcul_most_probable_for_nary_class(row, local_inst, dictName, MarkovBlanket, target):
  """
  Calculate the most probable class for variable target

  Parameters
  ----------
      row: numpyArray shape: (n features)
          test data
      local_inst: pyAgrum.Potential
          Instantiation of the Markov Blanket EXCEPT the target
      dictName: Dict[str : int]
          dictionary of the name of a variable and his column in the data base
      MarkovBlanket: pyAgrum.BayesNet
          Markov Blanket to work on
      target: str
          Name of the target
  Returns
  -------
      Tuple[int,proba]:
          the value and the probability of the most probable class
  """
  p = _calcul_proba_for_nary_class(
    row, local_inst, dictName, MarkovBlanket, target)
  return p.argmax(), p.max()


def _calcul_proba_for_binary_class(row, label1, labels, Inst, dictName, MarkovBlanket, target):
  """
  Calculate the probability of having label1 to the binary variable y

  Parameters
  ----------
      row: numpyArray shape: (n features)
          test data
      label1:
          the True value of y
      labels:
          the False values of y
      Inst: Potential
          Instantiation of the Markov Blanket
      dictName: dict[str : int]
          dictionary of the name of a variable and his column in the data base
      MarkovBlanket: pyAgrum.BayesNet
          Markov Blanket to work on
      target: str
          Name of the target
  Returns
  -------
      double
          probability of getting label1 to the variable y
  """

  # create Instantiation with Markov Blanket's variables
  for n in MarkovBlanket.names():
    if n == target:
      continue
    Inst.chgVal(n, str(row[dictName.get(n)]))

  # probability of Positive value
  Inst.chgVal(target, str(label1))
  res1 = MarkovBlanket.cpt(target).get(Inst)

  # probability of Negative value
  res2 = 0.0
  for label0 in labels:
    Inst.chgVal(target, str(label0))
    res2 += MarkovBlanket.cpt(target).get(Inst)

  # probability for all the children
  for n in MarkovBlanket.children(target):
    Inst.chgVal(target, str(label1))
    res1 = res1 * (MarkovBlanket.cpt(n).get(Inst))

    tmp = 0.0
    for label0 in labels:
      Inst.chgVal(target, str(label0))
      tmp += (MarkovBlanket.cpt(n).get(Inst))
    res2 = res2 * tmp

  # normalize to have probabilities

  return res1 / (res1 + res2)
