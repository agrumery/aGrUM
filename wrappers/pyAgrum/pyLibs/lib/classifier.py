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
import os
import numpy as np
import pandas as pd

import pyAgrum as gum
from pyAgrum.lib.bn2roc import __computeROCpoints, _computeROC


def get_threshold(bn, csv_name, target, label):
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
    'scikit-learn-like' methods for BN classification in jupyter notebook
    bi_classe classification only
    label of the target must be True or 1
  """

  def __init__(self, learning_method='greedy', prior='laplace'):
    self._bn = gum.BayesNet()
    self.learning_method = learning_method
    self.prior = prior
    self.class_name = None
    self.positif_label = None
    self.threshold = None
    self.nb_classes = 2

  def fit_from_csv(self, csv_name, variable):
    """
    Fit the Bayesian Network model according to the given training data from a csv file.

    :param csv_name: filename of the training data
    :type csv_name: str
    :param variable: name of the target variable
    :type variable: str

    :return: self

    """
    df = pd.read_csv(csv_name)

    if 'int' in str(df[variable].dtype):
      self.positif_label = 1
    elif 'bool' in str(df[variable].dtype):
      self.positif_label = True
    else:
      self.positif_label = 'true'

    self.class_name = variable
    self.nb_classes = 2

    learner = gum.BNLearner(csv_name)

    if self.learning_method == 'greedy':
      learner.useGreedyHillClimbing()

    self._bn = learner.learnBN()

    self.threshold = get_threshold(self._bn, csv_name, self.class_name, self.positif_label)

    return self

  def fit(self, Xtrain, Ytrain):
    """
    Fit the Bayesian Network model according to the given training data.

    :param Xtrain: dataframe, shape (n_samples, n_features) - training dataframe
    :param Ytrain: dataframe, shape (n_samples,) - target values/class labels

    :return: self

    """
    self.class_name = Ytrain.name
    self.nb_classes = 2

    train_file = pd.concat([Xtrain, Ytrain], axis=1)
    train_file.to_csv('temp_trainFile.csv', index=False)

    self.fit_from_csv('temp_trainFile.csv', self.class_name)

    os.remove("temp_trainFile.csv")

    return self

  def predict_proba(self, Xtest):

    """
    Compute probabilities of possible outcomes for samples in Xtest.

    :param Xtest: dataframe, shape(n_samples, n_features) - testing dataframe.

    :return: Yscores - array, shape (n_samples, n_classes) - returns the probability of the sample for each class in the model.

    """

    Yscores = np.empty([Xtest.shape[0], self.nb_classes])
    Yscores[:] = np.nan

    ie = gum.LazyPropagation(self._bn)
    for var in ie.BN().names():  # ici que var de mb
      if var != self.class_name:
        ie.addEvidence(var, 0)
    ie.addTarget(self.class_name)

    Xtest = Xtest.reset_index(drop=True)

    for i in range(len(Xtest)):
      for var in ie.BN().names():
        if var != self.class_name:

          try:
            idx = self._bn.variable(var).index(str(Xtest.loc[[i]].to_dict('index')[i][var]))
            ie.chgEvidence(var, idx)
          except gum.GumException:
            # this can happend when value is missing is the test base.
            print('The value ' + str(idx) + ' for the variable ' + str(var) + ' is missing in the training set.')
            pass
      ie.makeInference()

      marginal = ie.posterior(self.class_name)
      Yscores[i] = marginal.toarray()

    return Yscores

  def predict(self, Xtest):
    """
    Perform classification on samples in Xtest.

    :param Xtest: dataframe, shape(n_samples, n_features) - testing dataframe.

    :return: Ypred - array, shape (n_samples,) - class labels for samples in Xtest.

    """

    Yscores = self.predict_proba(Xtest)
    Ypred = np.where(Yscores[:, 1] >= self.threshold, 1, 0)

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
    return gum.MarkovBlanket(self._bn, self.class_name)
