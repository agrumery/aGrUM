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
Created on Thu Jun 11 14:08:14 2020

"""

import math
import warnings

import numpy

import pyagrum

from ._utils import _ImplementConstraints as implementConstraints
from ._utils import _ImplementScore as implementScore


def _fitStandard(X, y, learner, learningMethod, possibleSkeleton, scoringType, constraints):
  """
  The default fit function that uses MIIC, Greedy Hill Climbing or Tabu list sorting depending on the learning method chosen.

  Parameters
  ---------
      X: {array-like, sparse matrix} of shape (n_samples, n_features)
      training data
      y: array-like of shape (n_samples)
      Target values
  """

  implementConstraints(constraints, learner)

  if learningMethod == "MIIC":
    learner.useMIIC()
  elif learningMethod == "MIICMDL":
    learner.useMIIC()
    learner.useMDLCorrection()
  elif learningMethod == "MIICNML":
    learner.useMIIC()
    learner.useNMLCorrection()
  elif learningMethod == "GHC":  # default
    learner.useGreedyHillClimbing()
  elif learningMethod == "Tabu":
    learner.useLocalSearchWithTabuList()
  else:
    raise ValueError("Learning Method not recognized.")

  if possibleSkeleton is not None:
    learner.setPossibleSkeleton(possibleSkeleton)

  implementScore(scoringType, learner)
  bn = learner.learnBN()
  return bn


def _fitTAN(X, y, bn, learner, variableList, target, significance):
  """
  Uses the selective Tree-Augmented NaiveBayes (Blanco et al., 2005) to learn the network structure and its
  parameters: the features dependent on the class, then the edges between them, are selected by G2 tests at the
  level ``significance`` (with ``significance=1``, every feature and every edge is kept: TAN of Friedman et al., 1997).

  Parameters
  ---------
      X: {array-like, sparse matrix} of shape (n_samples, n_features)
      training data
      y: array-like of shape (n_samples)
      Target values
      significance: float
      level of the independence tests, in ]0,1]

  """

  # the number of columns in our data
  d = X.shape[1]

  # the features whose dependence on the class is significant
  selected = [i for i in range(d) if learner.G2(variableList[i], target)[1] <= significance]

  if len(selected) == 0:
    warnings.warn(
      f"TAN: no feature is significantly dependent on the class at level {significance}: the classifier predicts the "
      "prior of the class",
      stacklevel=2,
    )

  # with less than two features, TAN works exactly the same as NaiveBayes
  if len(selected) < 2:
    return _fitNaiveBayes(X, y, bn, learner, [variableList[i] for i in selected], target, None)

  probabilityY = learner.pseudoCount([target]).normalize().tolist()
  mutualInformation = dict()
  undirectedGraph = pyagrum.UndiGraph()

  # we calculate the conditional mutual information of the pairs of features whose dependence given the class is
  # significant
  for i in selected:
    undirectedGraph.addNodeWithId(i)
  for a, i in enumerate(selected):
    for j in selected[:a]:
      if learner.G2(variableList[i], variableList[j], [target])[1] > significance:
        continue
      probabilityList = learner.pseudoCount([variableList[i], variableList[j], target]).normalize().tolist()
      probabilityXi = learner.pseudoCount([variableList[i], target]).normalize().tolist()
      probabilityXj = learner.pseudoCount([variableList[j], target]).normalize().tolist()
      temp = 0
      for yIndex in range(len(probabilityList)):
        for xjIndex in range(len(probabilityList[yIndex])):
          for xiIndex in range(len(probabilityList[yIndex][xjIndex])):
            if probabilityList[yIndex][xjIndex][xiIndex] > 0:
              temp = temp + probabilityList[yIndex][xjIndex][xiIndex] * math.log(
                probabilityList[yIndex][xjIndex][xiIndex]
                * probabilityY[yIndex]
                / (probabilityXi[yIndex][xiIndex] * probabilityXj[yIndex][xjIndex])
              )
      mutualInformation[(i, j)] = temp

  mutualInformation = {k: v for k, v in sorted(mutualInformation.items(), key=(lambda item: item[1]), reverse=True)}

  # maximum weight spanning tree (Kruskal) of each connected component, as in Friedman et al. (1997)
  for var in mutualInformation:
    (i, j) = var
    undirectedGraph.addEdge(i, j)

    # if the edge causes a cycle, we delete the edge and pass on to the following pair of variables
    if undirectedGraph.hasUndirectedCycle():
      undirectedGraph.eraseEdge(i, j)
    # dict(int:set(int)): each key is a node from every connected part of the graph. The set associated is a set of all nodes that are part of the same connected part of the graph
  connectedParts = undirectedGraph.connectedComponentsList()

  for node in connectedParts:
    # int: the id of the node that will be used as a root to orient the undirected graph, initialised as 0
    root = 0
    # we choose the node with the largest mutual information with y as the root. We save the largest mutual information in the following variable
    maxMutualInformation = -99999
    for x0 in connectedParts[node]:
      mutual = 0
      probabilityList = learner.pseudoCount([variableList[x0], target]).normalize().tolist()
      probabilityY = learner.pseudoCount([target]).normalize().tolist()
      probabilityX = learner.pseudoCount([variableList[x0]]).normalize().tolist()
      for yIndex in range(len(probabilityList)):
        for xIndex in range(len(probabilityList[yIndex])):
          if probabilityList[yIndex][xIndex] > 0:
            mutual = mutual + probabilityList[yIndex][xIndex] * math.log(
              probabilityList[yIndex][xIndex] / (probabilityY[yIndex] * probabilityX[xIndex])
            )
      if mutual > maxMutualInformation:
        maxMutualInformation = mutual
        root = x0
    ListOfNodes = [root]
    for tailId in ListOfNodes:
      # for every element in the list of nodes we create an arc between this element and every neighbour of the element in the undirected graph that is not already in the list of nodes.
      # Since the graph contains no cycles we know that if headId is already in the list then the arc (headId,tailId) has already been added, meaning the arc (tailId,headId) shouldn't be added
      neighbours = undirectedGraph.neighbours(tailId)
      for headId in neighbours:
        if headId not in ListOfNodes:
          bn.addArc(variableList[tailId], variableList[headId])
          ListOfNodes.append(headId)
  for i in selected:
    bn.addArc(target, variableList[i])

  bn = learner.learnParameters(bn.dag())
  return bn


def _fitChowLiu(X, y, bn, learner, variableList, target):
  """
  Uses the Chow-Liu algorithm to learn the network structure and its parameters.

  Parameters
  ---------
      X: {array-like, sparse matrix} of shape (n_samples, n_features)
      training data
      y: array-like of shape (n_samples)
      Target values
  """

  # since the chow liu algorithm doesn't differentiate between input and output variables, we construct a matrix that includes them both
  dimension = y.shape
  yColumn = numpy.reshape(y, (dimension[0], 1))
  xAndY = numpy.concatenate((yColumn, X), axis=1)
  d = xAndY.shape[1]

  mutualInformation = dict()
  undirectedGraph = pyagrum.UndiGraph()

  # we calculate the mutual information of all pairs of variables
  for i in range(d):
    undirectedGraph.addNodeWithId(i)
    if i > 0:
      probabilityXi = learner.pseudoCount([variableList[i - 1]]).normalize().tolist()
    for j in range(i):
      if j > 0:
        probabilityList = learner.pseudoCount([variableList[i - 1], variableList[j - 1]]).normalize().tolist()
        probabilityXj = learner.pseudoCount([variableList[j - 1]]).normalize().tolist()
      else:
        probabilityList = learner.pseudoCount([variableList[i - 1], target]).normalize().tolist()
        probabilityXj = learner.pseudoCount([target]).normalize().tolist()
      mutual = 0
      for xjIndex in range(len(probabilityList)):
        for xiIndex in range(len(probabilityList[xjIndex])):
          if probabilityList[xjIndex][xiIndex] > 0:
            mutual = mutual + probabilityList[xjIndex][xiIndex] * math.log(
              probabilityList[xjIndex][xiIndex] / (probabilityXi[xiIndex] * probabilityXj[xjIndex])
            )
      mutualInformation[(i, j)] = mutual
  # sorting the dictionary of mutualInformation in descending order by the values associated
  mutualInformation = {k: v for k, v in sorted(mutualInformation.items(), key=(lambda item: item[1]), reverse=True)}

  for i, j in mutualInformation:
    # if the mutual information between xi and xj we add an edge between the two nodes
    undirectedGraph.addEdge(i, j)

    # if the edge causes a cycle, we delete the edge and pass on to the following pair of variables
    if undirectedGraph.hasUndirectedCycle():
      undirectedGraph.eraseEdge(i, j)

  ListOfNodes = [0]
  for tailId in ListOfNodes:
    # for every element in the list of nodes we create an arc between this element and every neighbour of the element in the undirected graph that is not already in the list of nodes.
    # Since the graph contains no cycles we know that if headId is already in the list then the arc (headId,tailId) has already been added, meaning the arc (tailId,headId) shouldn't be added
    neighbours = undirectedGraph.neighbours(tailId)
    for headId in neighbours:
      if headId not in ListOfNodes:
        if tailId > 0:
          bn.addArc(variableList[tailId - 1], variableList[headId - 1])
        else:
          bn.addArc(target, variableList[headId - 1])
        ListOfNodes.append(headId)

  bn = learner.learnParameters(bn.dag())
  return bn


def _fitNaiveBayes(X, y, bn, learner, variableList, target, constraints):
  """
  Uses the Naive Bayes algorithm to learn the network parameters. The network structure isn't learned since it is fixed.

  Parameters
  ---------
      X: {array-like, sparse matrix} of shape (n_samples, n_features)
      training data
      y: array-like of shape (n_samples)
      Target values
  """

  for variableName in variableList:
    bn.addArc(target, variableName)
  if constraints is not None:
    warnings.warn("The structure of Naive Bayes is fixed, so it is impossible to add any new constraints")

  bn = learner.learnParameters(bn.dag())
  return bn
