############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2026 by                                             #
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
#   See LICENCES for more details.                                         #
#                                                                          #
#   SPDX-FileCopyrightText: Copyright 2005-2026                            #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

"""
The purpose of this module is to provide tools for mapping Bayesian Network (and inference) in dot language in order to
be displayed/saved as image.
"""

import time

import matplotlib.colors
import matplotlib.pyplot as plt
import pydot as dot

import pyagrum
import pyagrum.lib.dotstyle as gds
from pyagrum.lib import proba_histogram


def BN2dot(
  bn: pyagrum.BayesNet,
  size: float | str | None = None,
  nodeColor: dict[tuple[int, int], float] | None = None,
  arcWidth: dict[tuple[int, int], float] | None = None,
  arcLabel: dict[tuple[int, int], str] | None = None,
  arcColor: dict[tuple[int, int], float] | None = None,
  cmapNode: matplotlib.colors.Colormap | None = None,
  cmapArc: matplotlib.colors.Colormap | None = None,
  showMsg: dict | None = None,
) -> dot.Dot:
  """
  create a pydot representation of the BN

  Parameters
  ----------
    bn : pyagrum.BayesNet
      the Bayesian network
    size: str
      size of the rendered graph
    nodeColor: dict[Tuple(int,int),float]
      a nodeMap of values to be shown as color nodes (with special color for 0 and 1)
    arcWidth: dict[Tuple(int,int),float]
      an arcMap of values to be shown as bold arcs
    arcLabel: dict[Tuple(int,int),str]
        an arcMap of labels to be shown next to arcs
    arcColor: dict[Tuple(int,int),float]
      an arcMap of values (between 0 and 1) to be shown as color of arcs
    cmapNode: ColorMap
      color map to show the vals of Nodes
    cmapArc: ColorMap
      color map to show the vals of Arcs
    showMsg: dict
      a nodeMap of values to be shown as tooltip

  Returns
  -------
  pydot.Dot
    the desired representation of the Bayesian network
  """
  if cmapNode is None:
    cmapNode = plt.get_cmap(pyagrum.config["notebook", "nodeCmap"])

  if cmapArc is None:
    cmapArc = plt.get_cmap(pyagrum.config["notebook", "arcCmap"])

  # the look comes from the configuration (written by the toDot of the BN), the values by the options
  options = pyagrum.DotOptions()
  options.setSize(str(pyagrum.config["dot", "graphSize"] if size is None else size))
  gds.setNodesOptions(options, bn, nodeColor, cmapNode, showMsg)
  gds.setLinksOptions(options, bn.arcs(), arcWidth, arcLabel, arcColor, cmapArc, "&rarr;")

  return gds.dotFromAgrum(bn.toDot(options))


def BNinference2dot(
  bn: pyagrum.BayesNet,
  size: float | str | None = None,
  engine: pyagrum.BNInference | None = None,
  evs: dict | None = None,
  targets: set | None = None,
  nodeColor: dict | None = None,
  arcWidth: dict | None = None,
  arcColor: dict | None = None,
  cmapNode: matplotlib.colors.Colormap | None = None,
  cmapArc: matplotlib.colors.Colormap | None = None,
  dag: pyagrum.DAG | None = None,
) -> dot.Dot:
  """
  create a pydot representation of an inference in a BN

  Parameters
  ----------
    bn : pyagrum.BayesNet
      the Bayesian network
    size: str
      size of the rendered graph
    engine:  pyagrum.BNInference
      inference algorithm used. If None, LazyPropagation will be used
    evs: dict
      map of evidence
    targets: set
      set of targets. If targets={} then each node is a target
    nodeColor: dict
      a nodeMap of values to be shown as color nodes (with special color for 0 and 1)
    arcWidth: dict
      a arcMap of values to be shown as bold arcs
    arcColor: dict
      a arcMap of values (between 0 and 1) to be shown as color of arcs
    cmapNode: ColorMap
      color map to show the vals of Nodes
    cmapArc: ColorMap
      color map to show the vals of Arcs
    dag : pyagrum.DAG
      only shows nodes that have their id in the dag (and not in the whole BN)

  Returns
  -------
    the desired representation of the inference
  """
  if evs is None:
    evs = {}
  if targets is None:
    targets = set()
  if cmapNode is None:
    cmapNode = plt.get_cmap(pyagrum.config["notebook", "nodeCmap"])

  if cmapArc is None:
    cmapArc = plt.get_cmap(pyagrum.config["notebook", "arcCmap"])

  startTime = time.time()
  if engine is None:
    ie = pyagrum.LazyPropagation(bn)
  else:
    ie = engine
  ie.setEvidence(evs)
  ie.makeInference()
  stopTime = time.time()

  temp_dir = gds.inferenceTemporaryDirectory()

  dotstr = gds.inferenceDotHeader("digraph", stopTime - startTime)

  showdag = bn.dag() if dag is None else dag

  evidence = ie.hardEvidenceNodes() | ie.softEvidenceNodes()
  for nid in showdag.nodes():
    name = bn.variable(nid).name()
    dotstr += gds.inferenceNodeDot(
      name,
      nid,
      targets,
      nodeColor,
      cmapNode,
      nid in evidence,
      temp_dir,
      lambda filename, bgcol: proba_histogram.saveFigProba(ie.posterior(name), filename, bgcolor=bgcol),
    )

  dotstr += gds.inferenceArcsDot(showdag.arcs(), lambda n: bn.variable(n).name(), arcWidth, arcColor, cmapArc)
  dotstr += "}"

  return gds.prepareDot(gds.inferenceDotGraph(dotstr, size, temp_dir))
