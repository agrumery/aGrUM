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
"""
Created on Thu Jun 11 14:08:14 2020

"""
import numpy
import math
import warnings

import pyAgrum as gum

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


def _fitTAN(X, y, bn, learner, variableList, target):
  """
  Uses Tree-Augmented NaiveBayes to learn the network structure and its parameters.

  Parameters
  ---------
      X: {array-like, sparse matrix} of shape (n_samples, n_features)
      training data
      y: array-like of shape (n_samples)
      Target values

  """

  # a list of all the variables in our Bayesian network sorted by their index

  # the number of columns in our data
  d = X.shape[1]

  # If there is only one input column, TAN works exactly the same as NaiveBayes
  if (d < 2):
    _fitNaiveBayes(X, y, bn, learner, variableList, target, None)
    return

  probabilityY = learner.pseudoCount([target]).normalize().tolist()
  mutualInformation = dict()
  undirectedGraph = gum.UndiGraph()

  # we calculate the mutual information of all pairs of variables
  for i in range(d):
    undirectedGraph.addNodeWithId(i)
    for j in range(i):
      probabilityList = learner.pseudoCount([variableList[i], variableList[j], target]).normalize().tolist()
      probabilityXi = learner.pseudoCount([variableList[i], target]).normalize().tolist()
      probabilityXj = learner.pseudoCount([variableList[j], target]).normalize().tolist()
      temp = 0
      for yIndex in range(len(probabilityList)):
        for xjIndex in range(len(probabilityList[yIndex])):
          for xiIndex in range(len(probabilityList[yIndex][xjIndex])):
            if probabilityList[yIndex][xjIndex][xiIndex] > 0:
              temp = temp + probabilityList[yIndex][xjIndex][xiIndex] * math.log(
                probabilityList[yIndex][xjIndex][xiIndex] * probabilityY[yIndex] / (
                   probabilityXi[yIndex][xiIndex] * probabilityXj[yIndex][xjIndex]))
      mutualInformation[(i, j)] = temp
  # if the mutual information between two variables is bigger than this threshold, we add an edge between them
  threshold = 0
  for var in mutualInformation:
    threshold = threshold + mutualInformation[var]
  threshold = float(threshold) / (d * (d - 1))

  mutualInformation = {k: v for k, v in sorted(mutualInformation.items(), key=(lambda item: item[1]), reverse=True)}

  for var in mutualInformation:
    (i, j) = var
    # since it's sorted in descending order we know that if this value is under the threshold all the other following values will also be under the threshold
    if mutualInformation[var] < threshold:
      break
    # if the mutual information between xi and xj we add an edge between the two nodes
    undirectedGraph.addEdge(i, j)

    # if the edge causes a cycle, we delete the edge and pass on to the following pair of variables
    if (undirectedGraph.hasUndirectedCycle()):
      undirectedGraph.eraseEdge(i, j)
    # dict(int:set(int)): each key is a node from every connected part of the graph. The set associated is a set of all nodes that are part of the same connected part of the graph
  connectedParts = undirectedGraph.connectedComponents()

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
              probabilityList[yIndex][xIndex] / (probabilityY[yIndex] * probabilityX[xIndex]))
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
  for i in range(d):
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
  undirectedGraph = gum.UndiGraph()

  # we calculate the mutual information of all pairs of variables
  for i in range(d):
    undirectedGraph.addNodeWithId(i)
    if (i > 0):
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
              probabilityList[xjIndex][xiIndex] / (probabilityXi[xiIndex] * probabilityXj[xjIndex]))
      mutualInformation[(i, j)] = mutual
  # sorting the dictionary of mutualInformation in descending order by the values associated
  mutualInformation = {k: v for k, v in sorted(mutualInformation.items(), key=(lambda item: item[1]), reverse=True)}

  for (i, j) in mutualInformation:

    # if the mutual information between xi and xj we add an edge between the two nodes
    undirectedGraph.addEdge(i, j)

    # if the edge causes a cycle, we delete the edge and pass on to the following pair of variables
    if (undirectedGraph.hasUndirectedCycle()):
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
