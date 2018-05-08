#!/usr/bin/python
# -*- coding: utf-8 -*-

# (c) Copyright by Pierre-Henri Wuillemin, UPMC, 2017
# (pierre-henri.wuillemin@lip6.fr)

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

from __future__ import print_function

import os
import sys

import pyAgrum as gum

from ._utils.pyAgrum_header import pyAgrum_header


def parents_name(bn, n):
  """
  Parameters
  ----------
  bn : pyAgrum.BayesNet
  	a Bayesian network
  n :
  	(str) the name of the node
  n :
	(int) the id of the node

  Returns
  -------
  map
  	a list of name of parents of node n

  Raises
  ------
  gum.IndexError
    If the node is not in the Bayesian network	
  """
  return map(lambda n: bn.variable(n).name(), bn.parents(nodeId(bn, n)))
  return l


def nodeId(bn, n):
  """
  Parameters
  ----------
  bn : pyAgrum.BayesNet
  	a Bayesian network
  n : str
  	the name of the node

  Returns
  -------
  int
  	the id of the node

  Raises
  ------
  gum.IndexError
    If the node is not in the Bayesian network	
  """
  if type(n) == str:
    return bn.idFromName(n)
  else:
    return n


DELTA_ERROR = 1e-6


def compareBNVariables(b1, b2):
  """
  Parameters
  ----------
  bn1 : pyAgrum.BayesNet
  	a Bayesian network
  bn2 : pyAgrum.BayesNet
  	another Bayesian network

  Returns
  -------
  str
  	'OK' if BN are composed of the same variables, indicates a non-existing variable otherwise

  """
  for i in range(b1.size()):
    try:
      v1 = b1.variable(i)
      v2 = b2.variable(b2.idFromName(v1.name()))
      if (v2.domainSize() != v1.domainSize()):
        return v1.name() + " has not the same domain size in the two bns"
    except IndexError:
      return b1.variable(i).name() + " does not exist in " + b2.property('name')

    for i in range(b2.size()):
      try:
        v2 = b2.variable(i)
        v1 = b1.variable(b1.idFromName(v2.name()))
        if (v2.domainSize() != v1.domainSize()):
          return v2.name() + " has not the same domain size in the two bns"
      except IndexError:
        return b2.variable(i).name() + " does not exist in " + b1.property('name')

  return "OK"


def compareBNParents(b1, b2):
  """
  Parameters
  ----------
  b1 : pyAgrum.BayesNet
  	a Bayesian network
  b2 : pyAgrum.BayesNet
  	another Bayesian network

  Returns
  -------
  str
  	'OK' if b2 have (at least) the same variable as b1 and their parents are the same.

  """
  for i in range(b1.size()):
    id1 = i
    id2 = b2.idFromName(b1.variable(id1).name())

    if not set(parents_name(b1, id1)) == set(parents_name(b2, id2)):
      return b1.variable(id1).name() + " has not the same parents in the two bns"

  return "OK"


def compareCPT(b1, cpt1, b2, cpt2):
  """
  Parameters
  ----------
  b1 : pyAgrum.BayesNet
  	a Bayesian network
  cpt1 : pyAgrum.Potential
  	one of b1's cpts
  b2 : pyAgrum.BayesNet
  	another Bayesian network
  cpt2 : pyAgrum.Potential
  	one of b2's cpts

  Returns
  -------
  str
  	'OK' if CPTs are the same

  Raises
  ------
  gum.KeyError
    If cpts are not from the same variable
  """

  dico2 = {}
  for i in range(cpt2.nbrDim()):
    dico2[cpt2.variable(i).name()] = i

  I1 = gum.Instantiation(cpt1)
  I2 = gum.Instantiation(cpt2)
  I1.setFirst()
  while not I1.end():
    for i in range(I1.nbrDim()):
      I2.chgVal(dico2[I1.variable(i).name()], I1.val(i))

    if abs(cpt1.get(I1) - cpt2.get(I2)) > DELTA_ERROR:
      return "For " + cpt1.variable(0).name() + ", cpt[" + str(I1) + "] (=" + str(cpt1.get(I1)) + ", in " + b1.property(
        'name') + ") !=cpt2[" + str(I2) + "] (=" + str(cpt2.get(I2)) + ", in " + b2.property('name') + ")"
    I1 += 1
  return "OK"


def compareBNCPT(b1, b2):
  """
  Parameters
  ----------
  b1 : pyAgrum.BayesNet
  	a Bayesian network
  b2 : pyAgrum.BayesNet
  	another Bayesian network

  Returns
  -------
  str
  	'OK' if b2 have (at least) the same variable as b1 and their cpts are the same
  """
  for i in range(b1.size()):
    res = compareCPT(b1, b1.cpt(i), b2, b2.cpt(b2.idFromName(b1.variable(i).name())))
    if res != "OK":
      return res
  return "OK"


def compareBN(name1, name2):
  """
  Parameters
  ----------
  name1 : str
  	a BN filename
  name2 : str
  	another BN filename

  Returns
  -------
  str
  	"OK" if bn are the same, a description of the error otherwise

  """
  if isinstance(name1, str):
    b1 = gum.loadBN(name1)
    b1.setProperty('name', '"' + os.path.basename(b1.property('name') + '"'))
  else:
    b1 = name1

  if isinstance(name2, str):
    b2 = gum.loadBN(name2)
    b2.setProperty('name', '"' + os.path.basename(b2.property('name') + '"'))
  else:
    b2 = name2

  ret = compareBNVariables(b1, b2)
  if ret != "OK":
    return ret

  ret = compareBNParents(b1, b2)
  if ret != "OK":
    return ret

  ret = compareBNCPT(b1, b2)
  return ret


def module_help(exit_value=1):
  """
  defines help viewed if args are not OK on command line, and exit with exit_value
  """
  print(os.path.basename(sys.argv[0]), "bn1.{" + gum.availableBNExts() + "} bn2.{" + gum.availableBNExts() + "}")
  sys.exit(exit_value)


if __name__ == "__main__":
  pyAgrum_header("2011-17")

  if len(sys.argv) != 3:
    module_help()

  print(compareBN(sys.argv[1], sys.argv[2]))
