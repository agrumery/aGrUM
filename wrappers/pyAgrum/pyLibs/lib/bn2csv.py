# (c) Copyright by Pierre-Henri Wuillemin, UPMC,
# 2017  (pierre-henri.wuillemin@lip6.fr)

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
Samples generation w.r.t to a probability distribution
represented by a Bayesian network.
"""

from __future__ import print_function

import csv
import math
import os
import random
import sys

import pyAgrum as gum

from ._utils.progress_bar import ProgressBar
from ._utils.pyAgrum_header import pyAgrum_header


class CSVGenerator:
  """
  Class for samples generation w.r.t to a probability distribution
  represented by a Bayesian network.
  """

  def __init__(self):
    print("** Class 'CSVGenerator' is is deprecated in pyAgrum>0.12.0. Please use gum.generateCSV instead.")
    self._parents = {}
    self._probas = {}

  @staticmethod
  def draw(tab):
    """
    draw a value using tab as probability table.

    Parameters
    ----------
    tab : list
      a probability table

    Returns
    -------
    tuple
      the couple (i,proba)
    """
    res = len(tab) - 1

    t = random.random()
    for i in range(len(tab)):
      if t < tab[i]:
        res = i
        break
      t -= tab[i]

    return (res, tab[res])

  @staticmethod
  def nameAndParents(bn, n):
    """
    Compute a list of parents for node n in BN bn.

    Parameters
    ----------
    bn : pyAgrum.BayesNet
      a Bayesian network
    n :
      (int) a node id
    n :
      (str) a node name

    Returns
    -------
    tuple
      a couple of name of n and list of parents names

    Raises
    ------
    gum.IndexError
      If the node is not in the Bayesian network
    """
    l = bn.cpt(n).var_names
    node = l.pop()
    # l is the list of var names
    return node, l  # map(bn.idFromName,l)

  def cachingnameAndParents(self, bn, n):
    """
    Compute a list of parents for node n in BN bn.

    Parameters
    ----------
    bn : pyAgrum.BayesNet
      a Bayesian network
    n :
      (int) a node id
    n :
      (str) a node name

    Returns
    -------
    tuple
      a couple of name of n and list of parents names

    """
    if not n in self._parents:
      self._parents[n] = CSVGenerator.nameAndParents(bn, n)
    return self._parents[n]

  def caching_probas(self, bn, node_id, n, par):
    """
    Parameters
    ----------
    bn : pyAgrum.BayesNet
      a Bayesian network
    node_id : int
      a node id
    n : int
      a node id
    par : list
      the node's parents

    Returns
    -------
    list
      the node's probabilities

    """
    if not n in self._probas:
      self._probas[n] = {}
    current = self._probas[n]

    for k in par:
      val_k = self._sample[k]
      if not val_k in current:
        current[val_k] = {}
      current = current[val_k]

    if not 'p' in current:
      current['p'] = list(bn.cpt(node_id)[dict([(p, self._sample[p]) for p in par])])

    return current['p']

  def newSample(self, bn, seq):
    """
    Generate a sample w.r.t to the bn using the variable sequence seq (topological order)

    Parameters
    ----------
    bn : pyAgrum.BayesNet
      a Bayesian network
    seq : list
      a variable sequence

    Returns
    -------
    tuple
      the coule (sample,log2-likelihood)
    """
    self._sample = {}
    LL = 0
    for node_id in seq:
      (nod, par) = self.cachingnameAndParents(bn, node_id)
      (self._sample[nod], p) = CSVGenerator.draw(self.caching_probas(bn, node_id, nod, par))
      LL += math.log(p, 2)

    return (self._sample, LL)

  def proceed(self, name_in, name_out, n, visible, with_labels):
    """
    From the file name_in (BN file), generate n samples and save them in name_out

    Parameters
    ----------
    name_in : str
      a file name
    name_out : str
      the output file
    n : int
      the number of samples
    visible : bool
      indicate if a progress bar should be displayed
    with_labels : bool
      indicate if values should be labelled or not

    Returns
    -------
    double
      the log2-likelihood of the n samples database
    """

    if isinstance(name_in, str):
      bn = gum.loadBN(name_in)
    else:
      bn = name_in

    seq = bn.topologicalOrder()
    writer = csv.writer(open(name_out, 'w',newline=''))

    if visible:
      sys.stdout.flush()

    titles = [bn.variable(item).name() for item in seq]
    writer.writerow(titles)

    if visible:
      prog = ProgressBar(name_out + ' : ', 0, n, 77, mode='static', char='#')
      prog.display()

    LL = 0
    for i in range(n):
      (cas, ll) = self.newSample(bn, seq)
      LL += ll

      if with_labels:
        row = [bn.variable(item).label(cas[bn.variable(item).name()]) for item in seq]
      else:
        row = [cas[bn.variable(item).name()] for item in seq]

      writer.writerow(row)

      if visible:
        prog.increment_amount()
        prog.display()

    if visible:
      print()
      print("Log2-Likelihood : {0}".format(LL))
      print()

    return LL

def generateCSV(name_in, name_out, n, visible=False, with_labels=True):
  """
  From the file name_in (BN file), generate n samples and save them in name_out

  Parameters
  ----------
  name_in : str
    a file name
  name_out : str
    the output file
  n : int
    the number of samples
  visible : bool
    indicate if a progress bar should be displayed
  with_labels : bool
    indicate if values should be labelled or not

  Returns
  -------
  double
    the log2-likelihood of the n samples database

  """
  print("** The function 'gum.lib.bn2csv.generateCSV' is deprecated in pyAgrum>0.12.0. Please use gum.generateCSV instead.")
  g = CSVGenerator()
  return g.proceed(name_in, name_out, n, visible, with_labels)

def module_help(exit_value=1):
  """
  defines help viewed if args are not OK on command line, and exit with exit_value
  """
  print(os.path.basename(sys.argv[0]), "src.{" + gum.availableBNExts() + "} nbr [dst.csv]")
  sys.exit(exit_value)
print("** The library 'gum.lib.bn2csv' is deprecated in pyAgrum>0.12.0. Please use gum.generateCSV instead.")

if __name__ == "__main__":
  pyAgrum_header("2011-2019")

  if len(sys.argv) < 3:
    module_help()

  bn_name = sys.argv[1]
  try:
    n = int(sys.argv[2])
  except ValueError:
    module_help()

  if len(sys.argv) < 4:
    base, ext = os.path.splitext(bn_name)
    csv_name = base + '.csv'
  else:
    csv_name = sys.argv[3]
    base, ext = os.path.splitext(csv_name)
    if ext != '.csv':
      csv_name += '.csv'

  generateCSV(bn_name, csv_name, n, True)
