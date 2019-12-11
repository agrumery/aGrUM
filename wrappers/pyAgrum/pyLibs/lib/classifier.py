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

# @authors Clara CHARON and Pierre-henri WUILLEMIN
import pyAgrum as gum
import pandas as pd
from pyAgrum.lib.bn2roc import __computeROCpoints, _computeROC
import pyAgrum.lib.notebook as gnb
import numpy as np


def get_seuil(bn, csv_name, target, label):
  """
  Get seuil by computing ROC

  :param bn: a bayesian network
  :type bn: gum.BayesNet
  :param csv_name: a csv filename
  :type csv_name: str
  :param target: the target
  :type target: str
  :param label: the target label
  :type label: str

  :return: the seuil

  """

  (res, totalP, totalN, idTarget) = __computeROCpoints(bn, csv_name, target, label, visible=False, with_labels=True)
  points, opt, seuil = _computeROC(bn, res, totalP, totalN, idTarget, label)

  return seuil


class BNClassifier:
  """
  'scikit-learn-like' API for classification using BayesNet in pyAgrum
  """

  def __init__(self, learning_method='greedy', prior='laplace'):
    self._bn = gum.BayesNet()
    self.learning_method = learning_method
    self.prior = prior

  def fit(self, Xtrain, Ytrain):
    """
    Fit the Bayesian Network model according to the given training data.

    :param Xtrain: dataframe, shape (n_samples, n_features) - training dataframe
    :param Ytrain: dataframe, shape (n_samples,) - target values/class labels

    :return: self

    """
    self.nom_classe = Ytrain.name
    self.nb_classe = 2

    trainFile = pd.concat([Xtrain, Ytrain], axis=1)
    trainFile.to_csv('trainFile.csv', index=False)

    learner = gum.BNLearner('trainFile.csv')

    if self.learning_method == 'greedy':
      learner.useGreedyHillClimbing()

    self._bn = learner.learnBN()

    self.seuil = get_seuil(self._bn, 'trainFile.csv', self.nom_classe, True)

    return self

  def predict_proba(self, Xtest):

    """
    Compute probabilities of possible outcomes for samples in Xtest.

    :param Xtest: dataframe, shape(n_samples, n_features) - testing dataframe.

    :return: Yscores - array, shape (n_samples, n_classes) - returns the probability of the sample for each class in the model.

    """

    Yscores = np.empty([Xtest.shape[0], self.nb_classe])
    Yscores[:] = np.nan

    ie = gum.LazyPropagation(self._bn)
    for var in ie.BN().names():  # ici que var de mb
      if var != self.nom_classe:
        ie.addEvidence(var, 0)
    ie.addTarget(self.nom_classe)

    for i in range(len(Xtest)):
      for var in ie.BN().names():
        if var != self.nom_classe:

          try:
            idx = self._bn.variable(var).index(str(Xtest.loc[[i]].to_dict('index')[i][var]))
            ie.chgEvidence(var, idx)
          except gum.GumException:
            # this can happend when value is missing is the test base.
            print('The value ' + str(idx) + ' for the variable ' + str(var) + ' is missing in the training set.')
            pass
      ie.makeInference()

      marginal = ie.posterior(self.nom_classe)
      Yscores[i] = marginal.toarray()

    return Yscores

  def predict(self, Xtest):
    """
    Perform classification on samples in Xtest.

    :param Xtest: dataframe, shape(n_samples, n_features) - testing dataframe.

    :return: Ypred - array, shape (n_samples,) - class labels for samples in Xtest.

    """

    Yscores = self.predict_proba(Xtest)
    Ypred = np.where(Yscores[:, 1] >= self.seuil, True, False)

    return Ypred

  def bn(self):
    """
    Show the Bayesian Network.

    :return: BayesNet model
    """
    return self._bn

  def MarkovBlanket(self):
    """
    Compute the Markov Blanket for the target.

    :return: the Markov Blanket of the model
    """
    return gum.MarkovBlanket(self._bn, self.nom_classe)
