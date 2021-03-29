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

import pyAgrum as gum
from ._utils import _listIdtoName as listIdtoName


def compileMarkovBlanket(bn, target):
  """
  parameters:
      bn: gum.BayesNet
          Bayesian network to work on
      target: str or int
          Name or id of the target
  returns:
      MarkovBlanket: gum.BayesNet
          Markov Blanket from bn

  Create a Bayesian network with the children, their parents and the parents of the node target
  """
  mb = gum.BayesNetFragment(bn)
  mb.installNode(target)
  for i in bn.children(target):
    mb.installNode(i)
    for j in bn.parents(i):
      mb.installNode(j)

  return mb


def _calcul_most_probable_for_nary_class(row,Inst,dictName,MarkovBlanket, target):
  """
  parameters:
      row: numpyArray shape: (n features)
          test data
      Inst: Potential
          Instanciation of the Markov Blanket
      dictName: dict[str : int]
          dictionnary of the name of a variable and his column in the data base
      MarkovBlanket: gum.BayesNet
          Markov Blanket to work on
      target: str
          Name of the target
  returns:
      probable_class,proba:
          the index of probable class (from the variable target) and its probability

  Calculate the most probable class for variable target
  """
  # create Instantiation with Markov Blanket's variables
  for n in MarkovBlanket.names():
    if n == target:
      continue
    Inst.chgVal(n, str(row[dictName.get(n)]))

  p = self.bn.cpt(target).extract(Inst)
  for i in MarkovBlanket.children(target):
    p *= MarkovBlanket.cpt(i).extract(Inst)
  p.normalize()

  return p.argmax(),p.max()

def _calcul_proba_for_binary_class(row, label1, labels, Inst, dictName, MarkovBlanket, target):
  """
  parameters:
      row: numpyArray shape: (n features)
          test data
      label1:
          the True value of y
      labels:
          the False values of y
      Inst: Potential
          Instanciation of the Markov Blanket
      dictName: dict[str : int]
          dictionnary of the name of a variable and his column in the data base
      MarkovBlanket: gum.BayesNet
          Markov Blanket to work on
      target: str
          Name of the target
  returns:
      res: double
          probability of getting label1 to the variable y

  Calculate the probability of having label1 to the binary variable y
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
