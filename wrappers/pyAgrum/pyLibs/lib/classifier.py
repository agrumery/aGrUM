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
import logging

import pyAgrum as gum
import pyAgrum.skbn as skbn
from pyAgrum.lib.bn2roc import _computepoints, _computeROC
from sklearn.metrics import accuracy_score


def get_threshold(bn, csv_name, target, label):
  """
  Get threshold by computing ROC

  :param bn: a Bayesian network
  :type bn: gum.BayesNet
  :param csv_name: a csv filename
  :type csv_name: str
  :param target: the target
  :type target: str
  :param label: the target label
  :type label: str

  :return: the threshold

  """
  (res, totalP, totalN, idTarget) = _computepoints(bn, csv_name, target,
                                                   label, visible=False, with_labels=True)

  points, opt, threshold = _computeROC(
    bn, res, totalP, totalN, idTarget, label)

  return threshold


class BNClassifier:
  """
  'scikit-learn-like' methods for BN classification in jupyter notebook
  bi_classe classification only
  label of the target must be True or 1
  """

  def __init__(self, bn=None, learning_method='greedy', prior='likelihood', prior_weight=1, bins=7):
    """
    :param bn: using a BN as classifier 
    :type bn: gum.BayesNet
    :param learning_method: greedy|miic
    :type learning_method: str
    :param prior: likelihood|laplace
    :type variable: str
    :param prior_weight: weight of prior
    :type prior_weight: float
    :param bins: nb of bins when discretization
    :type bins: int

    :return: self

    """

    print("** pyAgrum.lib.classifier.BNClassifier is deprecated in pyAgrum>0.18.1.")
    print("** A pyAgrum.skbn.BNClassifier will be used.")

    self.learning_method = learning_method
    self.prior = prior

    self.prior_weight = 1
    self.class_name = None
    self.positif_label = None
    self.threshold = None
    self.nb_classes = 2
    self.bins = bins

    self.bnclassifier = None

  def fit_from_csv(self, csv_name, variable):
    """
    Fit the Bayesian network model according to the given training data from a csv file.

    :param csv_name: filename of the training data
    :type csv_name: str
    :param variable: name of the target variable
    :type variable: str

    :return: self

    """
    if self.learning_method == 'greedy':
      learning_method = 'GHC'
    elif self.learning_method == "miic":
      learning_method = "MIIC"
    else:
      learning_method = self.learning_method

    if self.prior == 'laplace':
      prior = 'Laplace'
    elif self.prior == 'likelihood':
      prior = None
    else:
      prior = self.prior

    if self.bnclassifier is None:
      self.bnclassifier = skbn.BNClassifier(learning_method, prior, aPrioriWeight=self.prior_weight,
                                            discretizationStrategy="quantile", discretizationNbBins=self.bins,
                                            discretizationThreshold=0.95, usePR=False, significant_digit=13)
    else:
      self.bnclassifier.aPriori = prior
      self.bnclassifier.learningMethod = learning_method
      self.bnclassifier.aPrioriWeight = self.prior_weight
      self.bnclassifier.discretizer.setDiscretizationParameters(None, None, self.bins)

    self.bnclassifier.fit(filename=csv_name, targetName=variable)

    self.class_name = self.bnclassifier.target
    self.positif_label = self.bnclassifier.label
    self.threshold = self.bnclassifier.threshold

    return self

  def fit(self, Xtrain, Ytrain, nb_bins=10):
    """
    Fit the Bayesian network model according to the given training data.

    :param Xtrain: dataframe, shape (n_samples, n_features) - training dataframe
    :param Ytrain: dataframe, shape (n_samples,) - target values/class labels

    :return: self
    """
    if self.learning_method == 'greedy':
      learning_method = 'GHC'
    elif self.learning_method == "miic":
      learning_method = "MIIC"
    else:
      learning_method = self.learning_method

    if self.prior == 'laplace':
      prior = 'Laplace'
    elif self.prior == 'likelihood':
      prior = None
    else:
      prior = self.prior
    if self.bnclassifier is None:
      self.bnclassifier = skbn.BNClassifier(learning_method,
                                            prior,
                                            aPrioriWeight=self.prior_weight,
                                            discretizationStrategy="quantile",
                                            discretizationNbBins=self.bins,
                                            discretizationThreshold=0.95,
                                            usePR=False)

    else:
      self.bnclassifier.aPriori = prior
      self.bnclassifier.learningMethod = learning_method
      self.bnclassifier.aPrioriWeight = self.prior_weight
      self.bnclassifier.discretizer.setDiscretizationParameters(None, None, self.bins)

    self.bnclassifier.fit(Xtrain, Ytrain)

    self.class_name = self.bnclassifier.target
    self.positif_label = self.bnclassifier.label
    self.threshold = self.bnclassifier.threshold

    return self

  def predict_proba(self, Xtest):
    """
    Compute probabilities of possible outcomes for samples in Xtest.

    :param Xtest: dataframe, shape(n_samples, n_features) - testing dataframe.

    :return: Yscores - array, shape (n_samples, n_classes) - returns the probability of the sample for each class in the model.

    """
    if self.bnclassifier is None:
      raise ValueError(
        "This model has not been fitted and can therefore not be used to predict. Please fit the model to your training data before attempting to use the predict method")

    return self.bnclassifier.predict_proba(Xtest)

  def predict(self, Xtest):
    """
    Perform classification on samples in Xtest.

    :param Xtest: dataframe, shape(n_samples, n_features) - testing dataframe.

    :return: Ypred - array, shape (n_samples,) - class labels for samples in Xtest.

    """
    if self.bnclassifier is None:
      raise ValueError(
        "This model has not been fitted and can therefore not be used to predict. Please fit the model to your training data before attempting to use the predict method")

    return self.bnclassifier.predict(Xtest)

  def bn(self):
    """
    Show the Bayesian network.

    :return: BayesNet model
    """
    return self.bnclassifier.bn

  def MarkovBlanket(self):
    """
    Compute the Markov Blanket for the target.

    :return: the Markov Blanket of the model
    """
    return self.bnclassifier.MarkovBlanket

  def score(self, X_test, Y_test):

    if self.bnclassifier is None:
      raise ValueError(
        "This model has not been fitted and can therefore not be used to predict. Please fit the model to your training data before attempting to use the predict method")
    return self.bnclassifier.score(X_test, Y_test)
