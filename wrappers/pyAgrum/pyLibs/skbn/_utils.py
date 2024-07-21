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
from typing import Any

import pandas
import sklearn
import numpy
import pandas as pd

import pyAgrum
import pyAgrum.lib.bn2roc as bn2roc


def _CalculateThreshold(bn: pyAgrum.BayesNet, targetName: str, csvfilename: str, usePR: bool, beta: float,
                        significant_digits: int):
  """
  The Bayesian network gives us the probability of the target knowing the values of the other variables.
  The value above which the probability needs to be for the input to be classified as that class is called the threshold.
  This method calculates the optimal threshold using the roc or precision-recall curve.

  Parameters
  ----------
      bn: gum.BayesNet
          Bayesian network to work on
      targetName: str
          Name of the target
      csvfilename: str
          Name of the csv file
      usePR: bool
          indicates if the threshold to choose is Prevision-Recall curve's threhsold or ROC's threshold by default.
          ROC curves should be used when there are roughly equal numbers of observations for each class.
          Precision-Recall curves should be used when there is a moderate to large class imbalance especially for the target's class.
      beta: float
          the beta value to use when computing the F score. Only used when usePR is True.
      significant_digits: int
          number of significant digits when computing probabilities
  Returns
  -------
      float
          optimal threshold for predictions
  """
  target = bn.variableFromName(targetName)

  if usePR:
    _, _, _, threshold = bn2roc.showROC_PR(bn, csvfilename, targetName,
                                           target.labels()[1], beta=beta,
                                           show_fig=False, show_ROC=False, show_PR=False,
                                           significant_digits=significant_digits,
                                           show_progress=False)
  else:
    _, threshold, _, _ = bn2roc.showROC_PR(bn, csvfilename, targetName,
                                           target.labels()[1], beta=beta,
                                           show_fig=False, show_ROC=False, show_PR=False,
                                           significant_digits=significant_digits,
                                           show_progress=False)

  return threshold


def _ImplementScore(scoringType: str, learner: pyAgrum.BNLearner):
  """
  Tells the Bayesian network which scoring type to use.

  Parameters
  ----------
      scoringType: str
          A string designating the type of scoring we want to use. Since scoring is used
          while constructing the network and not when learning its Parameters,
          the scoring will be ignored if using a learning algorithm
          with a fixed network structure such as Chow-Liu, TAN or NaiveBayes.
          possible values are:  AIC, BIC, BD, BDeu, K2, Log2
          AIC means Akaike information criterion
          BIC means Bayesian Information criterion
          BD means Bayesian-Dirichlet scoring
          BDeu means Bayesian-Dirichlet equivalent uniform
          Log2 means log2 likelihood ratio test
      learner:
          learner object from pyAgrum to apply the score
  """
  if scoringType is None:
    return
  elif scoringType == 'AIC':
    learner.useScoreAIC()
  elif scoringType == 'BD':
    learner.useScoreBD()
  elif scoringType == 'BDeu':  # default
    learner.useScoreBDeu()
  elif scoringType == 'BIC':
    learner.useScoreBIC()
  elif scoringType == 'K2':
    learner.useScoreK2()
  elif scoringType == 'Log2':
    learner.useScoreLog2Likelihood()
  else:
    raise ValueError("Invalid scoringType! Possible values are : \
                          AIC , BD , BDeu, BIC , K2 and Log2")


def _ImplementPrior(prior: str, learner: pyAgrum.BNLearner, priorWeight: float, DirichletCsv: str):
  """
  Tells the Bayesian network which prior to use

  Parameters
  ----------
      prior: str
          A string designating the type of prior we want to use.
          Possible values are Smoothing, BDeu , Dirichlet and NoPrior.
          Note: if using Dirichlet smoothing DirichletCsv cannot be set to none
      learner:
          learner object from pyAgrum to apply the score
      priorWeight: float
          The weight used for the prior.
      DirichletCsv: str
          the file name of the csv file we want to use for the dirichlet prior.
          Will be ignored if prior is not set to Dirichlet.
  """
  if prior == 'Smoothing':
    learner.useSmoothingPrior(priorWeight)
  elif prior == 'Dirichlet':
    if DirichletCsv is None:
      raise ValueError(
        "Must specify file for dirichlet prior as a parameter to the classifier if using a dirichlet prior. DirichletCsv cannot be set to none if prior is set to Dirichlet")
    learner.useDirichletPrior(DirichletCsv, priorWeight)
  elif prior == 'BDeu':
    learner.useBDeuPrior(priorWeight)
  elif prior == 'NoPrior':
    learner.useNoPrior()
  elif prior is None:  # default : (small) Laplace's adjustment
    learner.useSmoothingPrior(0.01)
  else:
    raise ValueError(
      "Invalid prior! Possible values are : Smoothing , Dirichlet , BDeu and NoPrior")


def _ImplementConstraints(constraints: dict[str, Any], learner: pyAgrum.BNLearner):
  """
  Tells the Bayesian network which constraints should be put on the structure of the network.
  More details on the nature of these constraints can be found in the documentation for the constructor of this class

  Parameters
  ----------
      constraints: dict()
          A dictionary designating the constraints that we want to put on the structure of the Bayesian network.
          Ignored if using a learning algorithm where the structure is fixed such as TAN or NaiveBayes.
          the keys of the dictionary should be the strings "PossibleEdges" , "MandatoryArcs" and  "ForbiddenArcs".
          The format of the values should be a tuple of strings (tail,head) which designates the string arc from tail to head.
          For example if we put the value ("x0"."y") in MandatoryArcs the network will surely have an arc going from x0 to y.
          Note: PossibleEdge between nodes x and y allows for either (x,y) or (y,x) (or none of them) to be added to the Bayesian network, while the others are not symmetric.
      learner:
          learner object from pyAgrum to apply the score
  """
  if constraints is None:  # default
    return
  if type(constraints) is not dict:
    raise ValueError(
      "Invalid syntax for constraints. Constraints should be passed as a dictionary")
  for key in constraints:
    if key == 'MandatoryArcs':
      for (tail, head) in constraints[key]:
        learner.addMandatoryArc(tail, head)
    elif key == 'ForbiddenArcs':
      for (tail, head) in constraints[key]:
        learner.addForbiddenArc(tail, head)
    elif key == 'PossibleEdges':
      for (tail, head) in constraints[key]:
        learner.addPossibleEdge(tail, head)
    else:
      raise ValueError("Invalid syntax: the only keys in the constraints dictionary should be \
                             MandatoryArcs, PossibleEdges and ForbiddenArcs")


def _DFNames(X: pandas.DataFrame):
  """
  Return a dictionary of variable's name and his index from a DataFrame

  Parameters
  ----------
      X: pandas.DataFrame
          DataFrame to read

  Returns
  -------
      Dict[str,int]
          Dictionary of variable's name and his index
  """
  res = dict()
  i = 0
  for var in X.columns.tolist():
    res[var] = i
    i = i + 1

  return res


def _listIdtoName(bn: pyAgrum.BayesNet, liste: list[int]):
  """
  Return a list of names of the variable which have their id in list.

  Parameters
  ----------
      bn: gum.BayesNet
          Bayesian network to work on
      liste: list[int]
          List of id

  Returns
  -------
      List[str]
          List of names
  """
  return [bn.variable(i).name() for i in liste]


def _createCSVfromNDArrays(X, y, target: str, variableNameIndexDictionary: dict[str, int], csvfilename: str):
  """
  Creates a csv file from the matrices passed as Parameters.
  csvfilename  is used by the fit function to learn the network structure and its Parameters

  Parameters
  ----------
      X: {array-like, sparse matrix} of shape (n_samples, n_features)
          training data
      y: array-like of shape (n_samples)
          Target values
      target: str
          Name of the target
      variableNameIndexDictionary: dict[str , int]
          dictionnary of the csvfilename of a variable and his column in the data base
      csvfilename: str
          csv's title
  """

  # verifies if the shape of
  X, y = sklearn.utils.check_X_y(X, y, dtype=None, accept_sparse=True)
  y = pd.DataFrame(y, columns=[target])
  variableList = [k for k, v in sorted(
    variableNameIndexDictionary.items(), key=(lambda item: item[1]), reverse=False)]
  X = pd.DataFrame(X, columns=variableList)

  # We construct the list of variable names.
  # This will serve as the first line in the csv file since it is needed by pyAgrum to function properly
  training_file = pd.concat([y, X], axis=1)
  training_file.to_csv(csvfilename, index=False)
