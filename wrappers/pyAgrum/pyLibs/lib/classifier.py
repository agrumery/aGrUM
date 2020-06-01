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
from pyAgrum.lib.bn2roc import __computepoints, _computeROC
from sklearn.metrics import accuracy_score


def get_threshold(bn, csv_name, target, label):
  """
  Get threshold by computing ROC

  :param bn: a bayesian network
  :type bn: gum.BayesNet
  :param csv_name: a csv filename
  :type csv_name: str
  :param target: the target
  :type target: str
  :param label: the target label
  :type label: str

  :return: the threshold

  """
  (res, totalP, totalN, idTarget) = __computepoints(bn, csv_name, target,
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
    self._bn = gum.BayesNet() if bn is None else bn
    self.learning_method = learning_method
    self.prior = prior
    self.prior_weight = 1
    self.class_name = None
    self.positif_label = None
    self.threshold = None
    self.nb_classes = 2
    self.bins = bins

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

    template = gum.BayesNet()
    if 'int' in str(df[variable].dtype):
      self.positif_label = 1
    elif 'bool' in str(df[variable].dtype):
      self.positif_label = True
    else:
      self.positif_label = 'true'

    # template creation (with discretization if needed)
    for varname in df:
      if varname != variable:
        if 'float' in str(df[varname].dtype) and len(df[varname].unique()) / len(df[varname]) > 0.95:
                                                                              # less than 5% duplicate
          nb = min(len(df[varname]), self.bins)
          # , duplicates = 'drop')
          _, bins = pd.qcut(df[varname], nb, retbins=True)
          amplitude = bins[-1]-bins[0]
          bins[0] -= 100*amplitude
          bins[-1] += 100*amplitude
          template.add(gum.DiscretizedVariable(varname, varname, bins))
        else:
          varname_values = []
          for k in df[varname].unique():
            if pd.isna(k):
              varname_values.append('')
            else:
              varname_values.append(str(k))

          template.add(gum.LabelizedVariable(varname, varname, varname_values))

    variable_values = []
    for k in df[variable].unique():
      if pd.isna(k):
        variable_values.append('')
      else:
        variable_values.append(str(k))

    template.add(gum.LabelizedVariable(
        variable, variable, sorted(variable_values)))

    self.class_name = variable
    self.nb_classes = 2

    learner = gum.BNLearner(csv_name, template)

    if self.learning_method == 'greedy':
      learner.useGreedyHillClimbing()
    else:  # miic for now
      learner.useMIIC()

    if self.prior == "laplace":
      learner.useAprioriSmoothing(self.prior_weight)
    else:
      learner.useNoApriori()

    self._bn = learner.learnBN()

    self.threshold = get_threshold(
        self._bn, csv_name, self.class_name, self.positif_label)

    return self

  def fit(self, Xtrain, Ytrain, nb_bins=10):
    """
    Fit the Bayesian Network model according to the given training data.

    :param Xtrain: dataframe, shape (n_samples, n_features) - training dataframe
    :param Ytrain: dataframe, shape (n_samples,) - target values/class labels

    :return: self
    """
    if type(Ytrain) is np.ndarray:
      Ytrain = pd.DataFrame(Ytrain, columns=["Y"])
      Xtrain = pd.DataFrame(
          Xtrain, columns=[f"X{i}" for i in range(Xtrain.shape[1])])
    if hasattr(Ytrain, "name"):
      self.class_name = Ytrain.name
    else:
      self.class_name = "Y"

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
    if type(Xtest) is np.ndarray:
      Xtest = pd.DataFrame(
          Xtest, columns=[f"X{i}" for i in range(Xtest.shape[1])])

    Yscores = np.empty([Xtest.shape[0], self.nb_classes])
    Yscores[:] = np.nan

    mbnames = [self._bn.variable(i).name()
               for i in gum.MarkovBlanket(self._bn, self.class_name).nodes()
               if self._bn.variable(i).name() != self.class_name]
    ie = gum.LazyPropagation(self._bn)
    for var in ie.BN().names():  # ici que var de mb
      if var != self.class_name:
        ie.addEvidence(var, 0)
    ie.addTarget(self.class_name)

    Xtest = Xtest.reset_index(drop=True)

    for line in Xtest.itertuples():
      for var in mbnames:
          try:
            idx = self._bn.variable(var).index(str(getattr(line, var)))
            ie.chgEvidence(var, idx)

          except gum.GumException:
            # this can happend when value is missing is the test base.
            logging.warning(
                f"[pyAgrum] ** pyAgrum.lib.classifier : The value {getattr(line, var)} for the variable {var} is missing in the training set.")
            pass

      ie.makeInference()

      marginal = ie.posterior(self.class_name)
      Yscores[line[0]] = marginal.toarray()

    return Yscores

  def predict(self, Xtest):
    """
    Perform classification on samples in Xtest.

    :param Xtest: dataframe, shape(n_samples, n_features) - testing dataframe.

    :return: Ypred - array, shape (n_samples,) - class labels for samples in Xtest.

    """
    if type(Xtest) is np.ndarray:
      Xtest = pd.DataFrame(
          X_test, columns=[f"X{i}" for i in range(Xtest.shape[1])])

    Yscores = self.predict_proba(Xtest)

    if self.positif_label == True:
      Ypred = np.where(Yscores[:, 1] >= self.threshold, True, False)
    if self.positif_label == 'true':
      Ypred = np.where(Yscores[:, 1] >= self.threshold, 'true', 'false')
    else:
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

  def score(self, X_test, Y_test):
    if type(Y_test) is np.ndarray:
      Y_test = pd.DataFrame(Y_test, columns=["Y"])
      X_test = pd.DataFrame(
          X_test, columns=[f"X{i}" for i in range(X_test.shape[1])])

    return accuracy_score(Y_test, self.predict(X_test))
