############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#                                                                          #
#   The aGrUM/pyAgrum library is free software; you can redistribute it    #
#   and/or modify it under the terms of either :                           #
#                                                                          #
#    - the GNU Lesser General Public License as published by               #
#      the Free Software Foundation, either version 3 of the License,      #
#      or (at your option) any later version,                              #
#    - the MIT license (MIT),                                              #
#    - or both in dual license, as here.                                   #
#                                                                          #
#   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    #
#                                                                          #
#   This aGrUM/pyAgrum library is distributed in the hope that it will be  #
#   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          #
#   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS #
#   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   #
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER #
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        #
#   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  #
#   OTHER DEALINGS IN THE SOFTWARE.                                        #
#                                                                          #
#   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   #
#   licence (LICENSE.MIT) for more details.                                #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

"""
The purpose of this module is to provide basic tools for dealing with dynamic Bayesian Network (and inference) : modeling, visualisation, inference.
"""

import numpy as np
import pydot as dot

import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle

import pyagrum as gum

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
  return f"{name[:-1]}{ts}" if not _isInNoTimeSlice(name) else name


def getTimeSlicesRange(dbn):
  """
  get the range and (name,radical) of each variables

  Parameters
  ----------
  dbn: gum.BayesNet
    a 2TBN or an unrolled BN


  Returns
  ------
  Dict[str,List[T[str,str]]]
    all the timeslice of a dbn : ['0','t'] for a classic 2TBN, range(T) for a classic unrolled BN
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
  """
  Check if bn is a 2 TimeSlice Bayesian network

  Parameters
  ----------
  bn: pyagrum.BayesNet
    the Bayesian network

  Returns
  -------
  bool
    True if the BN is syntaxically correct to be a 2TBN
  """
  ts = getTimeSlicesRange(bn)
  if not set(ts.keys()) <= {noTimeCluster, "0", "t"}:
    return False, "Some variables are not correctly suffixed."

  domainSizes = dict()
  for name, radical in ts['t']:
    domainSizes[radical] = bn.variable(name).domainSize()

  res = ""
  for name, radical in ts['0']:
    if radical in domainSizes:
      if domainSizes[radical] != bn.variable(name).domainSize():
        res = f"\n - for variables {name}/{radical}t"

  if res != "":
    return False, "Domain size mismatch : " + res

  return True, ""


def _TimeSlicesToDot(dbn):
  """
  Try to correctly represent dBN and 2TBN in dot format

  Parameters
  ----------
  dbn: pyagrum.BayesNet
    a 2TBN or an unrolled BN
  """
  timeslices = getTimeSlicesRange(dbn)
  kts = sorted(timeslices.keys(), key=lambda x: -1 if x == noTimeCluster else 1e8 if x == 't' else int(x))

  # dynamic member makes pylink unhappy
  # pylint: disable=no-member
  g = dot.Dot(graph_type='digraph')
  g.set_rankdir("LR")
  g.set_splines("ortho")
  g.set_node_defaults(color="#000000", fillcolor="white", style="filled")

  for k in kts:
    if k != noTimeCluster:
      cluster = dot.Cluster(k, label=f"Time slice {k}", bgcolor="#DDDDDD", rankdir="same")
      g.add_subgraph(cluster)
    else:
      cluster = g  # small trick to add in graph variable in no timeslice
    for (n, label) in sorted(timeslices[k]):
      cluster.add_node(dot.Node('"' + n + '"', label='"' + label + '"'))

  g.set_edge_defaults(color="blue", constraint="False")
  for tail, head in dbn.arcs():
    g.add_edge(dot.Edge('"' + dbn.variable(tail).name() + '"',
                        '"' + dbn.variable(head).name() + '"'))

  g.set_edge_defaults(style="invis", constraint="True")
  for x in timeslices["0"]:
    name = x[1]
    prec = None
    for k in kts:
      if k == noTimeCluster:
        continue
      if prec is not None:
        g.add_edge(dot.Edge(f'"{name}{prec}"', f'"{name}{k}"'))
      prec = k

  return g


def showTimeSlices(dbn, size=None):
  """
  Try to correctly display dBN and 2TBN

  Parameters
  ----------
  dbn: pyagrum.BayesNet
    a 2TBN or an unrolled BN
  size: int or str
    size of the fig
  """

  # jupyter notebooks is optional
  # pylint: disable=import-outside-toplevel
  from pyagrum.lib.notebook import showGraph
  if size is None:
    size = gum.config["dynamicBN", "default_graph_size"]

  showGraph(_TimeSlicesToDot(dbn), size)


def getTimeSlices(dbn, size=None):
  """
  Try to correctly represent dBN and 2TBN as an HTML string

  Parameters
  ----------
  dbn: pyagrum.BayesNet
    a 2TBN or an unrolled BN
  size: int or str
    size of the fig
  """
  # jupyter notebooks is optional
  # pylint: disable=import-outside-toplevel
  from pyagrum.lib.notebook import getGraph
  if size is None:
    size = gum.config["dynamicBN", "default_graph_size"]

  return getGraph(_TimeSlicesToDot(dbn), size)


def unroll2TBN(dbn, nbr):
  """
  unroll a 2TBN given the nbr of timeslices

  Parameters
  ----------
  dbn: pyagrum.BayesNet
    a 2TBN or an unrolled BN
  nbr: int
   the number of timeslice

  Returns
  -------
  pyagrum.BayesNet
    unrolled BN from a 2TBN and the nbr of timeslices
  """
  ts = getTimeSlicesRange(dbn)
  if not {noTimeCluster, "0", "t"}.issuperset(ts.keys()) and {"0", "t"}.issubset(ts.keys()):
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
          name, ts
        )
                              )  # create the true name

  # add parents
  # the main pb : to have the same order for parents w.r.t the order in 2TBN
  for dbn_id in dbn.nodes():
    name = dbn.variable(dbn_id).name()
    # right order for parents
    lvarnames = list(reversed(dbn.cpt(dbn_id).names))
    lvarnames.pop()  # remove the node itself, parents remain
    lvarnames.reverse()

    for name_parent in lvarnames:
      if not _isInSecondTimeSlice(name):
        if not _isInSecondTimeSlice(name_parent):
          bn.addArc(bn.idFromName(name_parent), bn.idFromName(name))
        else:
          if _isInFirstTimeSlice(name):
            raise TypeError("An arc from timeslice t to timeslice is impossible in dBN")
          for ts in range(1, nbr):
            new_name_parent = realNameFrom2TBNname(name_parent, ts)  # current TimeSlice
            bn.addArc(bn.idFromName(new_name_parent), bn.idFromName(name))
      else:
        for ts in range(1, nbr):
          if _isInFirstTimeSlice(name_parent):
            new_name_parent = realNameFrom2TBNname(name_parent, ts - 1)  # last TimeSlice
          else:
            new_name_parent = realNameFrom2TBNname(name_parent, ts)  # current TimeSlice
          new_name = realNameFrom2TBNname(name, ts)  # necessary current TimeSlice
          bn.addArc(bn.idFromName(new_name_parent), bn.idFromName(new_name))

  # tensor creation
  for dbn_id in dbn.nodes():
    name = dbn.variable(dbn_id).name()
    if not _isInSecondTimeSlice(name):
      bn.cpt(bn.idFromName(name))[:] = dbn.cpt(dbn_id)[:]
    else:
      for ts in range(1, nbr):
        bn.cpt(bn.idFromName(realNameFrom2TBNname(name, ts)))[:] = dbn.cpt(dbn_id)[:]

  return bn


def plotFollowUnrolled(lovars, dbn, T, evs, vars_title=None):
  """
  plot the dynamic evolution of a list of vars with a dBN

  :param lovars: list of variables to follow
  :param dbn: the unrolled dbn
  :param T: the time range
  :param evs: observations
  :param vars_title: string for default or a dictionary with the variable name as key and the respective title as value.
  """
  ie = gum.LazyPropagation(dbn)
  ie.setEvidence(evs)
  ie.makeInference()

  x = np.arange(T)

  for var in lovars:
    v0 = dbn.variableFromName(var + "0")
    lpots = []
    for i in range(v0.domainSize()):
      serie = []
      for t in range(T):
        serie.append(ie.posterior(dbn.idFromName(var + str(t)))[i])
      lpots.append(serie)

    _, ax = plt.subplots()
    plt.xlim(left=0, right=T - 1)
    plt.ylim(top=1, bottom=0)
    ax.xaxis.grid()

    # Setting a customized title
    if vars_title is None:
      plt.title(f"Following variable {var}", fontsize=20)
    elif len(vars_title) != 0:
      plt.title(vars_title[var], fontsize=20)
    else:
      raise TypeError("Incorrect format of the plots title dictionary")

    plt.xlabel('time')

    stack = ax.stackplot(x, lpots)

    proxy_rects = [Rectangle((0, 0), 1, 1, fc=pc.get_facecolor()[0])
                   for pc in stack]
    labels = [v0.label(i) for i in range(v0.domainSize())]
    plt.legend(proxy_rects, labels, loc='center left',
               bbox_to_anchor=(1, 0.5), ncol=1, fancybox=True, shadow=True
               )

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
