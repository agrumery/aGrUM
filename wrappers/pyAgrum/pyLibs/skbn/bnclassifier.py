# -*- coding: utf-8 -*-
# (c) Copyright by Pierre-Henri Wuillemin (LIP6), 2020  (pierre-henri.wuillemin@lip6.fr)

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


import pandas
import numpy
import os
import tempfile
import warnings

import sklearn

import pyAgrum as gum

from .discretizer import BNDiscretizer
from ._utils import _ImplementPrior as IPrior
from ._utils import _CalculateThreshold as CThreshold
from ._utils import _DFNames as DFNames
from ._utils import _createCSVfromNDArrays as CSV

from ._MBCalcul import compileMarkovBlanket
from ._MBCalcul import _calcul_proba_for_binary_class, _calcul_most_probable_for_nary_class, \
  _calcul_proba_for_nary_class

from ._learningMethods import _fitStandard as BN_fitStandard
from ._learningMethods import _fitNaiveBayes as BN_fitNaiveBayes
from ._learningMethods import _fitTAN as BN_fitTAN
from ._learningMethods import _fitChowLiu as BN_fitChowLiu


class BNClassifier(sklearn.base.BaseEstimator, sklearn.base.ClassifierMixin):
  """ Represents a (scikit-learn compliant) classifier wich uses a BN to classify. A BNClassifier is build using

   - a Bayesian network,
   - a database and a learning algorithm and parameters
   - the use of BNDiscretizer to discretize with different algorithms some variables.


      parameters:
            learningMethod: str
                A string designating which type of learning we want to use. Possible values are: Chow-Liu, NaiveBayes,
                TAN, MIIC + (MDL ou NML), GHC, 3off2 + (MDL ou NML), Tabu.
                GHC designates Greedy Hill Climbing.
                MIIC designates Multivariate Information based Inductive Causation
                TAN designates Tree-augmented NaiveBayes
                Tabu designated Tabu list searching

            aPriori: str
                A string designating the type of a priori smoothing we want to use. Possible values are Smoothing,
                BDeu, Dirichlet and NoPrior .
                Note: if using Dirichlet smoothing DirichletCsv cannot be set to none
                By default (when aPriori is None) : a smoothing(0.01) is applied.

            scoringType: str
                A string designating the type of scoring we want to use. Since scoring is used while constructing the
                network and not when learning its parameters, the scoring will be ignored if using a learning algorithm
                with a fixed network structure such as Chow-Liu, TAN or NaiveBayes.
                possible values are:  AIC, BIC, BD, BDeu, K2, Log2
                AIC means Akaike information criterion
                BIC means Bayesian Information criterion
                BD means Bayesian-Dirichlet scoring
                BDeu means Bayesian-Dirichlet equivalent uniform
                Log2 means log2 likelihood ratio test

            constraints: dict()
                A dictionary designating the constraints that we want to put on the structure of the Bayesian network.
                Ignored if using a learning algorithm where the structure is fixed such as TAN or NaiveBayes.
                the keys of the dictionary should be the strings "PossibleEdges" , "MandatoryArcs" and  "ForbiddenArcs".
                The format of the values should be a tuple of strings (tail,head) which designates the string arc from
                tail to head. For example if we put the value ("x0"."y") in MandatoryArcs the network will surely have
                an arc going from x0 to y.
                Note: PossibleEdges allows for both (tail,head) and (head,tail) to be added to the Bayesian network,
                while the others are not symmetric.

            aPrioriWeight: double
                The weight used for a priori smoothing.

            possibleSkeleton: pyagrum.undigraph
                An undirected graph that serves as a possible skeleton for the Bayesian network

            DirichletCsv: str
                the file name of the csv file we want to use for the dirichlet prior. Will be ignored if aPriori is not
                set to Dirichlet.

            discretizationStrategy: str
                sets the default method of discretization for this discretizer. This method will be used if the user has
                not specified another method for that specific variable using the setDiscretizationParameters method
                possible values are: 'quantile', 'uniform', 'kmeans', 'NML', 'CAIM' and 'MDLP'

            defaultNumberOfBins: str or int
                sets the number of bins if the method used is quantile, kmeans, uniform. In this case this parameter can
                also be set to the string 'elbowMethod' so that the best number of bins is found automatically.
                If the method used is NML, this parameter sets the the maximum number of bins up to which the NML
                algorithm searches for the optimal number of bins. In this case this parameter must be an int
                If any other discetization method is used, this parameter is ignored.

            discretizationThreshold: int or float
                When using default parameters a variable will be treated as continous only if it has more unique values
                than this number (if the number is an int greater than 1).
                If the number is a float between 0 and 1, we will test if the proportion of unique values is bigger than
                this number.
                For instance, if you have entered 0.95, the variable will be treated as continous only if more than 95%
                of its values are unique.

            usePR: bool
                indicates if the threshold to choose is Prevision-Recall curve's threhsold or ROC's threshold by
                default.
                ROC curves should be used when there are roughly equal numbers of observations for each class.
                Precision-Recall curves should be used when there is a moderate to large class imbalance especially for
                the target's class.

            significant_digit:
                number of significant digits when computing probabilities
    """

  def __init__(self, learningMethod="GHC", aPriori=None, scoringType="BIC", constraints=None, aPrioriWeight=1,
               possibleSkeleton=None, DirichletCsv=None, discretizationStrategy="quantile", discretizationNbBins=5,
               discretizationThreshold=25, usePR=False, significant_digit=10):
    """ parameters:
            learningMethod: str
                A string designating which type of learning we want to use. Possible values are: Chow-Liu, NaiveBayes,
                TAN, MIIC + (MDL ou NML), GHC, 3off2 + (MDL ou NML), Tabu.
                GHC designates Greedy Hill Climbing.
                MIIC designates Multivariate Information based Inductive Causation
                TAN designates Tree-augmented NaiveBayes
                Tabu designated Tabu list searching

            aPriori: str
                A string designating the type of a priori smoothing we want to use. Possible values are Smoothing, BDeu ,
                Dirichlet and NoPrior.
                Note: if using Dirichlet smoothing DirichletCsv cannot be set to none

            scoringType: str
                A string designating the type of scoring we want to use. Since scoring is used while constructing the
                network and not when learning its parameters, the scoring will be ignored if using a learning algorithm
                with a fixed network structure such as Chow-Liu, TAN or NaiveBayes.
                possible values are:  AIC, BIC, BD, BDeu, K2, Log2
                AIC means Akaike information criterion
                BIC means Bayesian Information criterion
                BD means Bayesian-Dirichlet scoring
                BDeu means Bayesian-Dirichlet equivalent uniform
                Log2 means log2 likelihood ratio test

            constraints: dict()
                A dictionary designating the constraints that we want to put on the structure of the Bayesian network.
                Ignored if using a learning algorithm where the structure is fixed such as TAN or NaiveBayes.
                the keys of the dictionary should be the strings "PossibleEdges" , "MandatoryArcs" and  "ForbiddenArcs".
                The format of the values should be a tuple of strings (tail,head) which designates the string arc from
                tail to head. For example if we put the value ("x0"."y") in MandatoryArcs the network will surely have
                an arc going from x0 to y.
                Note: PossibleEdges allows for both (tail,head) and (head,tail) to be added to the Bayesian network,
                while the others are not symmetric.

            aPrioriWeight: double
                The weight used for a priori smoothing.

            possibleSkeleton: pyagrum.undigraph
                An undirected graph that serves as a possible skeleton for the Bayesian network

            DirichletCsv: str
                the file name of the csv file we want to use for the dirichlet prior. Will be ignored if aPriori is not
                set to Dirichlet.

            discretizationStrategy: str
                sets the default method of discretization for this discretizer. This method will be used if the user has
                not specified another method for that specific variable using the setDiscretizationParameters method
                possible values are: 'quantile', 'uniform', 'kmeans', 'NML', 'CAIM' and 'MDLP'

            defaultNumberOfBins: str or int
                sets the number of bins if the method used is quantile, kmeans, uniform. In this case this parameter can
                also be set to the string 'elbowMethod' so that the best number of bins is found automatically.
                If the method used is NML, this parameter sets the the maximum number of bins up to which the NML
                algorithm searches for the optimal number of bins. In this case this parameter must be an int
                If any other discetization method is used, this parameter is ignored.

            discretizationThreshold: int or float
                When using default parameters a variable will be treated as continous only if it has more unique values
                than this number (if the number is an int greater than 1).
                If the number is a float between 0 and 1, we will test if the proportion of unique values is bigger than
                this number.
                For instance, if you have entered 0.95, the variable will be treated as continous only if more than 95%
                of its values are unique.

            usePR: bool
                indicates if the threshold to choose is Prevision-Recall curve's threhsold or ROC's threshold by
                default.
                ROC curves should be used when there are roughly equal numbers of observations for each class.
                Precision-Recall curves should be used when there is a moderate to large class imbalance especially for
                the target's class.

            significant_digit:
                number of significant digits when computing probabilities
    """

    # The method of learning used
    self.learningMethod = learningMethod

    # An object used to stock the learner object from pyAgrum
    self.learner = None

    # Used to stock the Bayesian network learned
    self.bn = None

    # The threshold used for predicting the class. THe algorithm calculates the probability of a certain class, the
    # classifier designates it as that class only if the probability is higher than the threshold.
    # The ROC curve is used to calculate the optimal threshold
    self.threshold = 0.5
    self.usePR = usePR

    # the type of a priori smoothing used
    self.aPriori = aPriori

    # the weight used for the a priori smoothing
    self.aPrioriWeight = aPrioriWeight

    # the type of scoring used
    self.scoringType = scoringType

    # the constraints forced onto the structure of the Bayesian network
    self.constraints = constraints

    self.possibleSkeleton = possibleSkeleton

    self.DirichletCsv = DirichletCsv

    self.MarkovBlanket = None

    self.significant_digit = significant_digit

    self.discretizationNbBins = discretizationNbBins
    self.discretizationStrategy = discretizationStrategy
    self.discretizationThreshold = discretizationThreshold
    self.discretizer = BNDiscretizer(
      discretizationStrategy, discretizationNbBins, discretizationThreshold)

    # AJOUT FROM MODEL

    # boolean that tells us whether this classifier is obtained from an already trained model (using the function
    # fromTrainedModel) or not
    self.fromModel = False

    self.label = '1.0'

    # the name of the target variable
    self.target = 'y'

    # the type of the target variable
    self.targetType = None

    # dict(str:int)
    # The keys of this dictionary are the names of the variables. The value associeted to each name is
    # the index of the variable.
    self.variableNameIndexDictionary = None

  def fit(self, X=None, y=None, filename=None, targetName=None):
    """
    parameters:
        X: {array-like, sparse matrix} of shape (n_samples, n_features)
            training data. Warning: Raises ValueError if either filename or targetname is not None. Raises ValueError
            if y is None.
        y: array-like of shape (n_samples)
            Target values. Warning: Raises ValueError if either filename or targetname is not None. Raises ValueError
            if X is None
        filename: str
            specifies the csv file where the training data and target values are located. Warning: Raises ValueError
            if either X or y is not None. Raises ValueError if targetName is None
        targetName: str
            specifies the name of the targetVariable in the csv file. Warning: Raises ValueError if either X or y is
            not None. Raises ValueError if filename is None.
    returns:
        void

    Fits the model to the training data provided. The two possible uses of this function are fit(X,y) and fit(filename,
    targetName). Any other combination will raise a ValueError
    """

    if filename is None:
      if targetName is not None:
        raise ValueError(
          "This function should be used either as fit(X,y) or fit(filename=...,targetAttribute=...). You have set "
          "filename to None, but have entered a targetName")
      if X is None or y is None:
        raise ValueError(
          "This function should be used either as fit(X,y) or fit(filename=...,targetAttribute=...). You have not "
          "entered a csv file name and not specified the X and y matrices that should be used")
    else:
      if targetName is None:
        raise ValueError(
          "This function should be used either as fit(X,y) or fit(filename=...,targetAttribute=...). The name of the "
          "target must be specified if using this function with a csv file.")
      if X is not None or y is not None:
        raise ValueError(
          "This function should be used either as fit(X,y) or fit(filename=...,targetAttribute=...). You have entered "
          "a filename and the X and y matrices at the same time.")
      X, y = self.XYfromCSV(filename, True, targetName)

    self.fromModel = False
    variableNames = None
    self.discretizer.clear()

    if type(y) == pandas.DataFrame:
      self.target = y.columns.tolist()[0]
    elif type(y) == pandas.core.series.Series:
      self.target = y.name
    else:
      self.target = 'y'

    if type(X) == pandas.DataFrame:
      variableNames = X.columns.tolist()

    # verifies the shape of the two arrays
    X, y = sklearn.utils.check_X_y(X, y, dtype=None, accept_sparse=True)

    d = X.shape[1]

    if variableNames is None:
      variableNames = ["x" + str(i) for i in range(d)]

    self.variableNameIndexDictionary = dict()

    for i in range(d):
      self.variableNameIndexDictionary[variableNames[i]] = i

    self.targetType = y.dtype

    possibleValuesY = numpy.unique(y)

    if len(possibleValuesY) == 1:
      raise ValueError(
        "There is only 1 possible values for Y in the data provided")
    if len(possibleValuesY) > 10:
      warnings.warn(
        f"A classifier with too many possible values for Y (here : {possibleValuesY}) in the data provided is not meaningfull ("
        "please use regression methods instead).")

    self.isBinaryClassifier = (len(possibleValuesY) == 2)

    self.bn = gum.BayesNet('Template')

    var = gum.LabelizedVariable(self.target, self.target, 0)
    for value in possibleValuesY:
      var.addLabel(str(value))
    self.bn.add(var)

    for i in range(d):
      var = self.discretizer.createVariable(
        variableNames[i], X[:, i], y, possibleValuesY)
      self.bn.add(var)

    csvfile = tempfile.NamedTemporaryFile(delete=False)
    tmpfilename = csvfile.name
    csvfilename = tmpfilename + ".csv"
    csvfile.close()

    CSV(X, y, self.target, self.variableNameIndexDictionary, csvfilename)

    self.learner = gum.BNLearner(csvfilename, self.bn)

    IPrior(self.aPriori, self.learner, self.aPrioriWeight, self.DirichletCsv)

    if self.learningMethod == 'NaiveBayes':
      self.bn = BN_fitNaiveBayes(
        X, y, self.bn, self.learner, variableNames, self.target, self.constraints)
    elif self.learningMethod == 'TAN':
      self.bn = BN_fitTAN(X, y, self.bn, self.learner,
                          variableNames, self.target)
    elif self.learningMethod == 'Chow-Liu':
      self.bn = BN_fitChowLiu(X, y, self.bn, self.learner,
                              variableNames, self.target)
    else:
      self.bn = BN_fitStandard(X, y, self.learner, self.learningMethod, self.possibleSkeleton, self.scoringType,
                               self.constraints)

    self.label = self.bn.variableFromName(self.target).labels()[1]

    self.MarkovBlanket = compileMarkovBlanket(self.bn, self.target)

    if self.isBinaryClassifier:
      self.threshold = CThreshold(
        self.MarkovBlanket, self.target, csvfilename, self.usePR, self.significant_digit)

    os.remove(csvfilename)
    os.remove(tmpfilename)

  def fromTrainedModel(self, bn, targetAttribute, targetModality="", copy=False, threshold=0.5, variableList=None):
    """
    parameters:
        bn: pyagrum.BayesNet
            The Bayesian network we want to use for this classifier
        targetAttribute: str
            the attribute that will be the target in this classifier
        targetModality: str
            If this is a binary classifier we have to specify which modality we are looking at if the target
            attribute has more than 2 possible values
            if  !="", a binary classifier is created.
            if =="", a classifier is created that can be non binary depending on the number of modalities for targetAttribute. If binary, the second one is taken as targetModality.
        copy: bool
            Indicates whether we want to put a copy of bn in the classifier, or bn itself.
        threshold: double
            The classification threshold. If the probability that the target modality is true is larger than this
            threshold we predict that modality
        variableList: list(str)
            A list of strings. variableList[i] is the name of the variable that has the index i. We use this information
            when calling predict to know which column corresponds to which variable.
            If this list is set to none, then we use the order in which the variables were added to the network.

    returns:
        void

    Creates a BN classifier from an already trained pyAgrum Bayesian network
    """

    self.fromModel = True

    # the set of the names of all the variables in the Bayesian network
    namesSet = set(bn.names())

    # The target specified must be a variable in the Bayesian network
    if targetAttribute not in namesSet:
      raise ValueError(
        "the target variable does not appear in the Bayesian network")

    self.target = targetAttribute

    self.learner = None

    if copy:
      self.bn = gum.BayesNet(bn)
    else:
      self.bn = bn

    self.threshold = threshold

    self.MarkovBlanket = compileMarkovBlanket(self.bn, self.target)

    self.variableNameIndexDictionary = dict()
    # if the user specified an order for the variables then we use this order
    if variableList is not None:

      if len(namesSet) - 1 != len(variableList):
        raise ValueError(
          "variableList should include all variables in the Bayesian network except the target")

      i = 0
      for name in variableList:
        if name not in namesSet:
          raise ValueError(
            "variableList includes a name that does not appear in the Bayesian network")
        self.variableNameIndexDictionary[name] = i
        i = i + 1

    # if the user didn't specify an order we use the order that the variables were added in
    else:
      variableList = bn.names()
      i = 0
      for name in variableList:
        if name == self.target:
          continue
        self.variableNameIndexDictionary[name] = i
        i = i + 1

    if targetModality != "":
      self.isBinaryClassifier = True
      self.label = targetModality
    else:
      if self.bn.variableFromName(self.target).domainSize() == 2:
        self.isBinaryClassifier = True
        self.label = self.bn.variableFromName(self.target).labels()[
          1]  # we take the label 1 as targetModality
      else:
        self.isBinaryClassifier = False

    def changeVariableName(self, oldName, newName):
      """
      parameters:
          oldName: str
              the old name of the variable
          newName: str
              the new name of the variable
      returns:
          void

      changes the name of a variable inside the Bayesian network
      """
      if oldName == self.target:
        self.bn.changeVariableName(oldName, newName)
        self.target = newName
        self.MarkovBlanket.changeVariableName(oldName, newName)
        return

      if oldName not in self.variableNameIndexDictionary:
        raise ValueError(
          "The oldName you have specified is not a name of a variable in the Bayesian network")
      index = self.variableNameIndexDictionary.pop(oldName)

      self.variableNameIndexDictionary[newName] = index

      self.bn.changeVariableName(oldName, newName)

      if oldName in self.MarkovBlanket.names():
        self.MarkovBlanket.changeVariableName(oldName, newName)

  # ------------------methode Markov Blanket et predict---------------------

  def predict(self, X, with_labels=True):
    """
    parameters:
        X: {array-like, sparse matrix} of shape (n_samples, n_features) or str
            test data, can be either dataFrame, matrix or name of a csv file
        with_labels: bool
            tells us whether the csv includes the labels themselves or their indexes.
    returns:
        y: array-like of shape (n_samples,)
            Predicted classes

    Predicts the most likely class for each row of input data, with bn's Markov Blanket
    """
    if type(X) == str:
      X, _ = self.XYfromCSV(X, target=self.target)

    if type(X) == pandas.DataFrame:
      dictName = DFNames(X)
    else:
      dictName = self.variableNameIndexDictionary

    if self.fromModel:
      X = sklearn.utils.check_array(X, dtype='str', ensure_2d=False)
    else:
      X = sklearn.utils.check_array(X, dtype=None, ensure_2d=False)

    if self.isBinaryClassifier:
      returned_list = self._binary_predict(X, dictName)
    else:
      returned_list = self._nary_predict(X, dictName, with_labels)

    returned_list = numpy.array(returned_list)
    if not self.fromModel:
      if self.targetType == "bool":
        returned_list = returned_list == "True"
      elif numpy.issubdtype(self.targetType, numpy.number):
        returned_list = returned_list.astype('float')

    return returned_list

  def _nary_predict(self, X, dictName, with_labels):
    """
     For a classifier, predicts the most likely class for each row of input data, with bn's Markov Blanket

    :param X: data
    :param dictName: dictionnary of the name of a variable and his column in the data base
    :param with_labels: tells us whether the csv includes the labels themselves or their indexes.
    :return:
    """
    returned_list = []
    I = self.MarkovBlanket.completeInstantiation()
    I.erase(self.target)
    for x in X:
      vals, _ = _calcul_most_probable_for_nary_class(
        x, I, dictName, self.MarkovBlanket, self.target)
      if with_labels:
        returned_list.append(self.MarkovBlanket.variable(
          self.target).label(vals[0][self.target]))
      else:
        returned_list.append(vals[0][self.target])

    return returned_list

  def _binary_predict(self, X, dictName):
    """
     For a binary classifier, predicts the most likely class for each row of input data, with bn's Markov Blanket

    :param X: data
    :param dictName: dictionnary of the name of a variable and his column in the data base
    :return:
    """
    returned_list = []
    # list of other labels of the target
    labels = [self.bn.variable(self.target).label(i)
              for i in range(self.bn.variable(self.target).domainSize())
              if self.bn.variable(self.target).label(i) != self.label]

    # negative value to add to the list returned
    label0 = labels[0]
    # label of the target
    label1 = self.label
    # Instantiation use to apply values of the data base
    I = self.MarkovBlanket.completeInstantiation()
    # read through data base's ligns
    for x in X:
      res = round(_calcul_proba_for_binary_class(x, label1, labels, I, dictName, self.MarkovBlanket, self.target),
                  self.significant_digit)

      if res >= self.threshold:  # Positive value predicted
        if self.fromModel:
          returned_list.append(True)
        else:
          returned_list.append(label1)
      else:  # Negative value predicted
        if self.fromModel:
          returned_list.append(False)
        else:
          returned_list.append(label0)

    return returned_list

  # ------------------interaction with sklearn, pour ROC et Precision-Recall ---------------------

  def predict_proba(self, X):
    """
    parameters:
        X: {array-like, sparse matrix} of shape (n_samples, n_features) or str
            test data, can be either dataFrame, matrix or name of a csv file
    returns:
        y: array-like of shape (n_samples,)
            Predicted probability for each classes

    Predicts the probability of classes for each row of input data, with bn's Markov Blanket
    """

    # dictionnary of the name of a variable and his column in the data base
    dictName = self.variableNameIndexDictionary

    if type(X) == pandas.DataFrame:
      dictName = DFNames(X)
      vals = X.to_numpy()
    elif type(X) == str:
      vals, _ = self.XYfromCSV(X, target=self.target)
      dictName = DFNames(vals, vals)
      vals = vals.to_numpy()
    else:
      vals = X

    if self.fromModel:
      vals = sklearn.utils.check_array(vals, dtype='str', ensure_2d=False)
    else:
      sklearn.utils.check_array(vals, dtype=None, ensure_2d=False)

    returned_list = []

    # label of the target
    label1 = self.label
    # list of other labels of the target
    labels = [self.bn.variable(self.target).label(i)
              for i in range(self.bn.variable(self.target).domainSize())
              if self.bn.variable(self.target).label(i) != self.label]

    # Instantiation use to apply values of the data base
    I = self.MarkovBlanket.completeInstantiation()

    # read through data base's ligns
    if self.isBinaryClassifier:
      for x in vals:
        res = round(_calcul_proba_for_binary_class(x, label1, labels, I,
                                                   dictName, self.MarkovBlanket, self.target), self.significant_digit)
        returned_list.append([1 - res, res])
    else:
      local_inst = gum.Instantiation(I)
      local_inst.erase(self.target)
      for x in vals:
        returned_list.append(_calcul_proba_for_nary_class(
          x, local_inst, dictName, self.MarkovBlanket, self.target).tolist())

    return numpy.array(returned_list)

  # ------------------ BNClassifier compatible from pyAgrum to sklearn ---------------------

  def XYfromCSV(self, filename, with_labels=True, target=None):
    """
    parameters:
        filename: str
            the name of the csv file
        with_labels: bool
            tells us whether the csv includes the labels themselves or their indexes.
        target: str or None
            The name of the column that will be put in the dataframe y. If target is None, we use the target that is
            already specified in the classifier
    returns:
        X: pandas.dataframe
            Matrix containing the data
        y: pandas.dataframe
            Column-vector containing the class for each data vector in X

    Reads the data from a csv file and separates it into a X matrix and a y column vector.
    """

    if self.fromModel:
      dataframe = pandas.read_csv(filename, dtype='str')
    else:
      dataframe = pandas.read_csv(filename)

    if target is None:
      target = self.target
    y = dataframe[target]
    X = dataframe.drop(target, axis=1)

    if not with_labels:
      variableList = X.columns.tolist()
      targetVariable = self.bn.variableFromName(target)
      for index in range(len(variableList)):
        variableList[index] = self.bn.variableFromName(variableList[index])
      for row in X:
        for i in len(row):
          row[i] = variableList[i].labels(row[i])
      if self.fromModel:
        if self.isBinaryClassifier:
          labelIndex = 0
          labelList = targetVariable.labels()
          while labelIndex < len(labelList):
            if labelList[labelIndex] == self.label:
              break
            labelIndex += 1
          y = y == labelIndex
      else:
        for index in range(len(y)):
          y[index] = targetVariable(y[index])

    elif self.fromModel:
      y = y.astype('str')
      if self.isBinaryClassifier:
        y = y == self.label

    return X, y

  def showROC_PR(self, filename, save_fig=False, show_progress=False):
    """
    Use the `pyAgrum.lib.bn2roc` tools to create ROC and Precision-Recall curve

    parameters:
      csv_name : str
        a csv filename
      save_fig : bool
        whether the graph soulb de saved
      show_progress : bool
        indicates if the resulting curve must be printed
    """
    import pyAgrum.lib.bn2roc as bn2roc
    bn2roc.showROC_PR(self.bn, filename, self.target,
                      self.label, significant_digits=self.significant_digit, show_progress=show_progress)
