# -*- coding: utf-8 -*-

# (c) Copyright by Pierre-Henri Wuillemin, UPMC, 2017
#   (pierre-henri.wuillemin@lip6.fr)

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
Basic implementation for dynamic Bayesian networks in pyAgrum
"""

import matplotlib.pyplot as plt
import numpy as np
import pyAgrum as gum
import pydotplus as dot
from matplotlib.patches import Rectangle

from .notebook import showGraph, getGraph

noTimeCluster = "void"


def _splitName(name):
  """
  By convention, name of dynamic variable "X" in dBN may be
    - "X0" for timeslice 0 both in unrolled BN and in 2TBN
    - "Xt" for timeslice t in a 2TBN
    - "X"+str(i) for timelice i with integer i>0 in unrolled BN
    - other naes are not in a timeslice
  @argument name : str (name of the dynamic variable)
  @return static_name,timeslice with timeslice =noTimeCluster,"t" or str(i)
  """
  if name[-1] == "t":
    return name[:-1], "t"
  i = len(name) - 1
  if not name[i].isdigit():
    return name, noTimeCluster

  while name[i].isdigit():
    if i == 0:
      return name, noTimeCluster
    i -= 1

  return name[:i + 1], name[i + 1:]


def _isInFirstTimeSlice(name):
  """
  @return true if there is a 0 at the end of name
  """
  return name[-1] == "0"


def _isInSecondTimeSlice(name):
  """
  @return true if there is a t at the end of name
  """
  return name[-1] == "t"


def _isInNoTimeSlice(name):
  return name[-1] not in ["0", "t"]


def realNameFrom2TBNname(name, ts):
  """
  @return dynamic name from static name and timeslice (no check)
  """
  return "{0}{1}".format(name[:-1], ts) if not _isInNoTimeSlice(name) else name


def getTimeSlicesRange(dbn):
  """
  get the range and (name,radical) of each variables

  :param dbn: a 2TBN or an unrolled BN
  :return: all the timeslice of a dbn

  e.g.
  ['0','t'] for a classic 2TBN
  range(T) for a classic unrolled BN
  """
  timeslices = {}

  for i in dbn.nodes():
    n = dbn.variable(i).name()
    label, ts = _splitName(n)
    if ts in timeslices:
      timeslices[ts].append((n, label))
    else:
      timeslices[ts] = [(n, label)]

  return timeslices


def is2TBN(bn):
  ts = getTimeSlicesRange(bn)
  if not (set(ts.keys()) <= set([noTimeCluster, "0", "t"])):
    return False, "Some variables are not correctly suffixed."

  domainSizes = dict()
  for name, radical in ts['t']:
    domainSizes[radical] = bn.variable(name).domainSize()

  res = ""
  for name, radical in ts['0']:
    if radical in domainSizes:
      if domainSizes[radical] != bn.variable(name).domainSize():
        res = "\n - for variables {}/{}t".format(name, radical)

  if res != "":
    return False, "Domain size mismatch : " + res
  else:
    return True, ""


def _TimeSlicesToDot(dbn):
  """
  Try to correctly represent dBN and 2TBN in dot format
  """
  timeslices = getTimeSlicesRange(dbn)

  g = dot.Dot(graph_type='digraph')
  g.set_rankdir("TD")
  g.set_splines("ortho")

  for k in sorted(timeslices.keys(), key=lambda x: -1 if x == noTimeCluster else 1e8 if x == 't' else int(x)):
    if k != noTimeCluster:
      cluster = dot.Cluster(k, label="Time slice {}".format(
          k), bgcolor="#DDDDDD", rankdir="TD")
      g.add_subgraph(cluster)
    else:
      cluster = g  # small trick to add in graph variable in no timeslice
    for (n, label) in sorted(timeslices[k]):
      cluster.add_node(dot.Node('"'+n+'"', label='"'+label+'"', style='filled',
                                color='#000000', fillcolor='white'))

  for tail, head in dbn.arcs():
    g.add_edge(dot.Edge('"'+dbn.variable(tail).name()+'"',
                        '"'+dbn.variable(head).name()+'"',
                        constraint=False, color="blue"))

  for k in sorted(timeslices.keys(), key=lambda x: -1 if x == noTimeCluster else 1e8 if x == 't' else int(x)):
    if k != noTimeCluster:
      prec = None
      for (n, label) in sorted(timeslices[k]):
        if prec is not None:
          g.add_edge(dot.Edge('"'+prec+'"',
                              '"'+n+'"',
                              style="invis"))
        prec = n

  return g


def showTimeSlices(dbn, size=None):
  """
  Try to correctly display dBN and 2TBN

  :param dbn: the dynamic BN
  :param size: size of the figue
  :param format: png/svg
  """
  if size is None:
    size = gum.config["dynamicBN", "default_graph_size"]

  showGraph(_TimeSlicesToDot(dbn), size)


def getTimeSlices(dbn, size=None):
  """
  Try to correctly represent dBN and 2TBN as an HTML string

  :param dbn: the dynamic BN
  :param size: size of the figue
  :param format: png/svg
  """
  if size is None:
    size = gum.config["dynamicBN", "default_graph_size"]

  return getGraph(_TimeSlicesToDot(dbn), size)


def unroll2TBN(dbn, nbr):
  """
  unroll a 2TBN given the nbr of timeslices

  :param dbn: the dBN
  :param nbr: the number of timeslice

  :return: unrolled BN from a 2TBN and the nbr of timeslices
  """
  ts = getTimeSlicesRange(dbn)
  if not (set([noTimeCluster, "0", "t"]).issuperset(ts.keys())
          and set(["0", "t"]).issubset(ts.keys())):
    raise TypeError("unroll2TBN needs a 2-TimeSlice BN")

  bn = gum.BayesNet()

  # variable creation
  for dbn_id in dbn.nodes():
    name = dbn.variable(dbn_id).name()
    if _isInNoTimeSlice(name):
      bn.add(dbn.variable(dbn_id))
    elif _isInFirstTimeSlice(name):
      # create a clone of the variable in the new bn
      bn.add(dbn.variable(dbn_id))
    else:
      for ts in range(1, nbr):
        # create a clone of the variable in the new bn
        nid = bn.add(dbn.variable(dbn_id))
        bn.changeVariableName(nid, realNameFrom2TBNname(
            name, ts))  # create the true name

  # add parents
  # the main pb : to have the same order for parents w.r.t the order in 2TBN
  for dbn_id in dbn.nodes():
    name = dbn.variable(dbn_id).name()
    # right order for parents
    l = dbn.cpt(dbn_id).var_names
    l.pop()
    l.reverse()

    for name_parent in l:
      if not _isInSecondTimeSlice(name):
        if not _isInSecondTimeSlice(name_parent):
          bn.addArc(bn.idFromName(name_parent), bn.idFromName(name))
        else:
          if _isInFirstTimeSlice(name):
            raise TypeError(
                "An arc from timeslice t to timeslice is impossible in dBN")
          else:
            for ts in range(1, nbr):
              new_name_parent = realNameFrom2TBNname(
                  name_parent, ts)  # current TimeSlice
              bn.addArc(bn.idFromName(new_name_parent), bn.idFromName(name))
      else:
        for ts in range(1, nbr):
          if _isInFirstTimeSlice(name_parent):
            new_name_parent = realNameFrom2TBNname(
                name_parent, ts - 1)  # last TimeSlice
          else:
            new_name_parent = realNameFrom2TBNname(
                name_parent, ts)  # current TimeSlice
          new_name = realNameFrom2TBNname(
              name, ts)  # necessary current TimeSlice
          bn.addArc(bn.idFromName(new_name_parent), bn.idFromName(new_name))

  # potential creation
  for dbn_id in dbn.nodes():
    name = dbn.variable(dbn_id).name()
    if not _isInSecondTimeSlice(name):
      bn.cpt(bn.idFromName(name))[:] = dbn.cpt(dbn_id)[:]
    else:
      for ts in range(1, nbr):
        bn.cpt(bn.idFromName(realNameFrom2TBNname(name, ts)))[
            :] = dbn.cpt(dbn_id)[:]

  return bn


def plotFollowUnrolled(lovars, dbn, T, evs):
  """
  plot the dynamic evolution of a list of vars with a dBN

  :param lovars: list of variables to follow
  :param dbn: the unrolled dbn
  :param T: the time range
  :param evs: observations
  """
  ie = gum.LazyPropagation(dbn)
  ie.setEvidence(evs)
  ie.makeInference()

  x = np.arange(T)

  for var in lovars:
    v0 = dbn.variableFromName(var + "0")
    l = []
    for i in range(v0.domainSize()):
      serie = []
      for t in range(T):
        serie.append(ie.posterior(dbn.idFromName(var + str(t)))[i])
      l.append(serie)

    fig, ax = plt.subplots()
    plt.ylim(top=1, bottom=0)
    ax.xaxis.grid()
    plt.title("Following variable {0}".format(var), fontsize=20)
    plt.xlabel('time')

    stack = ax.stackplot(x, l)

    proxy_rects = [Rectangle((0, 0), 1, 1, fc=pc.get_facecolor()[0])
                   for pc in stack]
    labels = [v0.label(i) for i in range(v0.domainSize())]
    plt.legend(proxy_rects, labels, loc='center left',
               bbox_to_anchor=(1, 0.5), ncol=1, fancybox=True, shadow=True)

    plt.show()


def plotFollow(lovars, twoTdbn, T, evs):
  """
  plots modifications of variables in a 2TDN knowing the size of the time window (T) and the evidence on the sequence.

  :param lovars: list of variables to follow
  :param twoTdbn: the two-timeslice dbn
  :param T: the time range
  :param evs: observations
  """
  plotFollowUnrolled(lovars, unroll2TBN(twoTdbn, T), T, evs)


if __name__ == '__main__':
  pyAgrum_header("2015-17")

  dbn = gum.BayesNet()
  a0, b0, c0, d0, at, bt, ct, dt = [dbn.add(gum.LabelizedVariable(s, s, 5)) for s in
                                    ["a0", "b0", "c0", "d0", "at", "bt", "ct", "dt"]]

  dbn.addArc(a0, b0)
  dbn.addArc(c0, d0)
  dbn.addArc(at, bt)

  dbn.addArc(a0, bt)
  dbn.addArc(b0, ct)
  dbn.addArc(c0, dt)
  dbn.addArc(d0, at)

  # needed from within spyder, do not understand why
  import time

  gum.initRandom(int(str(time.time()).split('.')[1]))
  # should not be necessary
  dbn.generateCPTs()

  showTimeSlices(dbn)

  # T=5
  # bn=unroll2TBN(dbn,T)
  # showTimeSlices(bn)

  plotFollow(["a", "b", "c", "d"], dbn, T=51, evs={
             'a15': 2, 'a30': 0, 'c14': 0, 'b40': 0})
