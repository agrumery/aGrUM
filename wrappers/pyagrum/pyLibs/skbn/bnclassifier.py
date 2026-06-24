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


import pandas
import numpy
import os
import tempfile
import warnings

import sklearn

import pyagrum as gum
from pyagrum.lib.discreteTypeProcessor import DiscreteTypeProcessor
from pyagrum.lib.discreteTypeProcessor import check_int

from ._utils import _ImplementPrior as IPrior
from ._utils import _CalculateThreshold as CThreshold
from ._utils import _DFNames as DFNames
from ._utils import _createCSVfromNDArrays as CSV

from ._MBCalcul import compileMarkovBlanket
from ._MBCalcul import (
  _calcul_proba_for_binary_class,
  _calcul_most_probable_for_nary_class,
  _calcul_proba_for_nary_class,
)

from ._learningMethods import _fitStandard as BN_fitStandard
from ._learningMethods import _fitNaiveBayes as BN_fitNaiveBayes
from ._learningMethods import _fitTAN as BN_fitTAN
from ._learningMethods import _fitChowLiu as BN_fitChowLiu


def createBNClassifier(
  *,
  learningMethod="MIIC",
  prior=None,
  scoringType="BIC",
  constraints=None,
  priorWeight=1,
  possibleSkeleton=None,
  DirichletCsv=None,
  discretizationStrategy="quantile",
  discretizationNbBins=5,
  discretizationThreshold=25,
  usePR=False,
  beta=1,
  significant_digit=10,
):
  """
  Convenience factory that creates a BNClassifier with a DiscreteTypeProcessor built from
  simple discretization parameters (strategy, bins, threshold).

  For per-variable discretization control, create a DiscreteTypeProcessor directly and
  pass it to BNClassifier(type_processor, ...) instead.

  Parameters
  ----------
  learningMethod : str, default="MIIC"
  prior : str, optional
      Possible values: Smoothing, BDeu, Dirichlet, NoPrior.
  scoringType : str, default="BIC"
      Ignored for fixed-structure algorithms (Chow-Liu, TAN, NaiveBayes).
  constraints : dict, optional
  priorWeight : float, default=1
  possibleSkeleton : pyagrum.UndiGraph, optional
  DirichletCsv : str, optional
  discretizationStrategy : str, default="quantile"
      Possible values: quantile, uniform, kmeans, NML, CAIM, MDLP.
  discretizationNbBins : int, default=5
  discretizationThreshold : int or float, default=25
  usePR : bool, default=False
  beta : float, default=1
  significant_digit : int, default=10
  """

  type_processor = DiscreteTypeProcessor(
    defaultDiscretizationMethod=discretizationStrategy,
    defaultNumberOfBins=discretizationNbBins,
    discretizationThreshold=discretizationThreshold,
  )
  return BNClassifier(
    type_processor=type_processor,
    learningMethod=learningMethod,
    prior=prior,
    scoringType=scoringType,
    constraints=constraints,
    priorWeight=priorWeight,
    possibleSkeleton=possibleSkeleton,
    DirichletCsv=DirichletCsv,
    usePR=usePR,
    beta=beta,
    significant_digit=significant_digit,
  )


def _bestTypedVal(v: gum.DiscreteVariable, idx: int) -> str | int | float:
  match v.varType():
    case gum.VarType_DISCRETIZED | gum.VarType_LABELIZED:
      return v.label(idx)
    case gum.VarType_INTEGER | gum.VarType_RANGE:
      return int(v.numerical(idx))
    case gum.VarType_NUMERICAL:
      return float(v.numerical(idx))
    case _:
      raise gum.NotFound("This type of variable does not exist yet.")


class BNClassifier(sklearn.base.ClassifierMixin, sklearn.base.BaseEstimator):
  """
  Represents a (scikit-learn compliant) classifier which uses a BN to classify. A BNClassifier is build using

   - a Bayesian network,
   - a database and a learning algorithm and parameters
   - the use of DiscreteTypeProcessor to discretize with different algorithms some variables.

  The classifier can be used to predict the class of new data.

  Parameters
  ----------
            learningMethod: str
                A string designating which learning algorithm we want to use. Possible values are: Chow-Liu, NaiveBayes,
                TAN, MIIC + (MDL ou NML), GHC, Tabu.
                GHC designates Greedy Hill Climbing.
                MIIC designates Multivariate Information based Inductive Causation
                TAN designates Tree-augmented NaiveBayes
                Tabu designated Tabu list searching

            prior: str
                A string designating the type of a priorsmoothing we want to use. Possible values are Smoothing,
                BDeu, Dirichlet and NoPrior .
                Note: if using Dirichlet smoothing DirichletCsv cannot be set to none
                By default (when prior is None) : a smoothing(0.01) is applied.

            scoringType: str
                A string designating the scoring method we want to use. Since scoring is used while constructing the
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

                Note: PossibleEdge allows between nodes x and y allows for either (x,y) or (y,x) (or none of them)
                to be added to the Bayesian network, while the others are not symmetric.

            priorWeight: double
                The weight used for a prior.

            possibleSkeleton: pyagrum.undigraph
                An undirected graph that serves as a possible skeleton for the Bayesian network

            DirichletCsv: str
                the file name of the csv file we want to use for the dirichlet prior. Will be ignored if prior is not
                set to Dirichlet.

            discretizationStrategy: str
                sets the default method of discretization for this discretizer. This method will be used if the user has
                not specified another method for that specific variable using the setDiscretizationParameters method
                possible values are: 'quantile', 'uniform', 'kmeans', 'NML', 'CAIM' and 'MDLP'

            discretizationNbBins: str or int
                sets the number of bins if the method used is quantile, kmeans, uniform. In this case this parameter can
                also be set to the string 'elbowMethod' so that the best number of bins is found automatically.
                If the method used is NML, this parameter sets the maximum number of bins up to which the NML
                algorithm searches for the optimal number of bins. In this case this parameter must be an int
                If any other discretization method is used, this parameter is ignored.

            discretizationThreshold: int or float
                When using default parameters a variable will be treated as continuous only if it has more unique values
                than this number (if the number is an int greater than 1).
                If the number is a float between 0 and 1, we will test if the proportion of unique values is bigger than
                this number.
                For instance, if you have entered 0.95, the variable will be treated as continuous only if more than 95%
                of its values are unique.

            usePR: bool
                indicates if the threshold to choose is Prevision-Recall curve's threshold or ROC's threshold by
                default.
                ROC curves should be used when there are roughly equal numbers of observations for each class.
                Precision-Recall curves should be used when there is a moderate to large class imbalance especially for
                the target's class.

    beta: float
        if you choose the Precision-Recall curve's threshold, the F-beta score is maximized. By default, beta=1
        to have the F1 score. A value inferior of 1 will give more weight to precision. A value superior of 1
        will give more weight to recall. For example, beta = 0.5 makes precision twice as important as recall,
        while beta = 2 does the opposite.

            significant_digit:
                number of significant digits when computing probabilities
  """

  def __init__(
    self,
    *,
    type_processor,
    learningMethod="MIIC",
    prior=None,
    scoringType="BIC",
    constraints=None,
    priorWeight=1,
    possibleSkeleton=None,
    DirichletCsv=None,
    usePR=False,
    beta=1,
    significant_digit=10,
  ):
    """
    Parameters
    ----------
            learningMethod: str
                A string designating which type of learning we want to use. Possible values are: Chow-Liu, NaiveBayes,
                TAN, MIIC + (MDL ou NML), GHC, Tabu.
                GHC designates Greedy Hill Climbing.
                MIIC designates Multivariate Information based Inductive Causation
                TAN designates Tree-augmented NaiveBayes
                Tabu designated Tabu list searching

            prior: str
                A string designating the type of prior we want to use. Possible values are Smoothing, BDeu ,
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

            priorWeight: double
                The weight used for a prior.

            possibleSkeleton: pyagrum.undigraph
                An undirected graph that serves as a possible skeleton for the Bayesian network

            DirichletCsv: str
                the file name of the csv file we want to use for the dirichlet prior. Will be ignored if prior is not
                set to Dirichlet.

            discretizationStrategy: str
                sets the default method of discretization for this discretizer. This method will be used if the user has
                not specified another method for that specific variable using the setDiscretizationParameters method
                possible values are: 'quantile', 'uniform', 'kmeans', 'NML', 'CAIM' and 'MDLP'

            discretizationNbBins: str or int
                sets the number of bins if the method used is quantile, kmeans, uniform. In this case this parameter can
                also be set to the string 'elbowMethod' so that the best number of bins is found automatically.
                If the method used is NML, this parameter sets the the maximum number of bins up to which the NML
                algorithm searches for the optimal number of bins. In this case this parameter must be an int
                If any other discetization method is used, this parameter is ignored.

            discretizationThreshold: int or float
                When using default parameters a variable will be treated as continuous only if it has more unique values
                than this number (if the number is an int greater than 1).
                If the number is a float between 0 and 1, we will test if the proportion of unique values is bigger than
                this number.
                For instance, if you have entered 0.95, the variable will be treated as continouus only if more than 95%
                of its values are unique.

            usePR: bool
                indicates if the threshold to choose is Prevision-Recall curve's threshold or ROC's threshold by
                default.
                ROC curves should be used when there are roughly equal numbers of observations for each class.
                Precision-Recall curves should be used when there is a moderate to large class imbalance especially for
                the target's class.

            significant_digit:
                number of significant digits when computing probabilities
    """

    # The method of learning used
    self.learningMethod = learningMethod

    self.usePR = usePR
    self.beta = beta

    # the type of prior used
    self.prior = prior

    # the weight used for the a prior
    self.priorWeight = priorWeight

    # the type of scoring used
    self.scoringType = scoringType

    # the constraints forced onto the structure of the Bayesian network
    self.constraints = constraints

    self.possibleSkeleton = possibleSkeleton

    self.DirichletCsv = DirichletCsv

    self.significant_digit = significant_digit

    self.type_processor = type_processor

  def fit(
    self,
    X,
    y,
  ):
    """
    Fits the model to the training data provided. The two possible uses of this function are `fit(X,y)`
    and `fit(data=...,targetName=...)`. Any other combination will raise a ValueError

    Parameters
    ----------
        X: {array-like, sparse matrix} of shape (n_samples, n_features)
            training data. Warning: Raises ValueError if either data or targetname is not None. Raises ValueError
            if y is None.
        y: array-like of shape (n_samples)
            Target values. Warning: Raises ValueError if either data or targetname is not None. Raises ValueError
            if X is None
        data: Union[str,pandas.DataFrame]
            the source of training data : csv filename or pandas.DataFrame. targetName is mandatory to find the class
            in this source.
        targetName: str
            specifies the name of the targetVariable in the csv file. Warning: Raises ValueError if either X or y is
            not None. Raises ValueError if data is None.
    """

    # The threshold used for predicting the class. THe algorithm calculates the probability of a certain class, the
    # classifier designates it as that class only if the probability is higher than the threshold.
    # The ROC curve is used to calculate the optimal threshold
    self.threshold_ = 0.5

    # the name of the target variable
    self.target_ = "y"

    if X is not None and y is None:
      raise ValueError("requires y to be passed, but the target y is None")
    if X is None or y is None:
      raise ValueError(
        "fit() requires both X and y. To fit from a CSV or DataFrame, use fitFromData(data, targetName)."
      )

    # boolean that tells us whether this classifier is obtained from an already trained model (using the function
    # fromTrainedModel) or not
    self.fromModel_ = False
    variableNames = None
    self.type_processor.clear()

    if isinstance(y, pandas.DataFrame):  # type(y) == pandas.DataFrame:
      self.target_ = y.columns.tolist()[0]
      if check_int(self.target_):
        self.target_ = "Y"
    elif type(y) is pandas.core.series.Series:
      self.target_ = y.name if y.name is not None else "y"
    else:
      self.target_ = "y"

    if isinstance(X, pandas.DataFrame):
      variableNames = [f"X{x}" if check_int(x) else x for x in X.columns]
      X = X.astype(object)

    # verifies the shape of the two arrays
    X, y = sklearn.utils.validation.validate_data(self, X, y, dtype=None, accept_sparse=False)
    X = X.astype(object)

    d = X.shape[1]

    if variableNames is None:
      variableNames = ["x" + str(i) for i in range(d)]

    # dict(str:int)
    # The keys of this dictionary are the names of the variables. The value associated to each name is
    # the index of the variable.
    self.variableNameIndexDictionary_ = dict()

    for i in range(d):
      self.variableNameIndexDictionary_[variableNames[i]] = i

    sklearn.utils.multiclass.check_classification_targets(y)

    # the type of the target variable
    self.targetType_ = y.dtype

    self.classes_ = numpy.unique(y)

    if len(self.classes_) == 1:
      raise ValueError("There is only 1 class for Y in the data provided")
    if len(self.classes_) > 10:
      warnings.warn(
        f"A classifier with too many possible values for Y (here : {self.classes_})"
        "in the data provided is not meaningfull ("
        "please use regression methods instead)."
      )

    # Used to stock the Bayesian network learned
    self.bn_ = gum.BayesNet("Template")

    is_int_varY = True
    min_vY = max_vY = None
    for value in self.classes_:
      if not check_int(value):
        is_int_varY = False
        break
      else:
        v = int(value)
        if min_vY is None or min_vY > v:
          min_vY = v
        if max_vY is None or max_vY < v:
          max_vY = v

    if is_int_varY:
      if len(self.classes_) == max_vY - min_vY + 1:  # no hole in the list of int
        var = gum.RangeVariable(self.target_, self.target_, min_vY, max_vY)
      else:
        var = gum.IntegerVariable(self.target_, self.target_, [int(v) for v in self.classes_])
    else:
      var = gum.LabelizedVariable(self.target_, self.target_, [str(v) for v in self.classes_])
    self.bn_.add(var)

    for i in range(d):
      var = self.type_processor._createVariable(variableNames[i], X[:, i], y, self.classes_)
      self.bn_.add(var)

    csvfile = tempfile.NamedTemporaryFile(delete=False)
    tmpfilename = csvfile.name
    csvfilename = tmpfilename + ".csv"
    csvfile.close()

    CSV(X, y, self.target_, self.variableNameIndexDictionary_, csvfilename)

    # An object used to stock the learner object from pyagrum
    self.learner_ = gum.BNLearner(csvfilename, self.bn_)

    IPrior(self.prior, self.learner_, self.priorWeight, self.DirichletCsv)

    if self.learningMethod == "NaiveBayes":
      self.bn_ = BN_fitNaiveBayes(X, y, self.bn_, self.learner_, variableNames, self.target_, self.constraints)
    elif self.learningMethod == "TAN":
      self.bn_ = BN_fitTAN(X, y, self.bn_, self.learner_, variableNames, self.target_)
    elif self.learningMethod == "Chow-Liu":
      self.bn_ = BN_fitChowLiu(X, y, self.bn_, self.learner_, variableNames, self.target_)
    else:
      self.bn_ = BN_fitStandard(
        X, y, self.learner_, self.learningMethod, self.possibleSkeleton, self.scoringType, self.constraints
      )

    self.label_ = self.bn_.variableFromName(self.target_).labels()[1]

    self.MarkovBlanket_ = compileMarkovBlanket(self.bn_, self.target_)

    if len(self.classes_) == 2:
      if self.MarkovBlanket_.size() <= 1:
        self.threshold_ = 0.5
      else:
        mb_features = [n for n in self.MarkovBlanket_.names() if n != self.target_]
        df_full = pandas.read_csv(csvfilename)
        df_mb = df_full[[self.target_] + mb_features]
        mb_tmpfile = tempfile.NamedTemporaryFile(delete=False, suffix=".csv", mode="w")
        mb_csvfilename = mb_tmpfile.name
        mb_tmpfile.close()
        df_mb.to_csv(mb_csvfilename, index=False)
        self.threshold_ = CThreshold(
          self.MarkovBlanket_, self.target_, mb_csvfilename, self.usePR, self.beta, self.significant_digit
        )
        os.remove(mb_csvfilename)

    os.remove(csvfilename)
    os.remove(tmpfilename)
    return self

  def fitFromTabular(self, data, targetName):
    """
    Convenience wrapper around fit() for loading training data from a CSV file or
    a pandas DataFrame directly.

    Parameters
    ----------
    data : str or pandas.DataFrame
        Either the path to a CSV file or a pandas DataFrame containing both
        the features and the target column.
    targetName : str
        Name of the column in data that contains the target values.

    Returns
    -------
    self
        Fitted BNClassifier instance.

    Raises
    ------
    ValueError
        If data is None, targetName is None, or data is neither a str nor a DataFrame.
    """
    if data is None:
      raise ValueError("Data must be specified.")
    elif targetName is None:
      raise ValueError("targetName must be specified.")

    self.fromModel_ = False

    if isinstance(data, str):
      X, y = self.XYfromCSV(data, True, targetName)
    elif isinstance(data, pandas.DataFrame):
      y = data[targetName]
      X = data.drop(targetName, axis=1)
    else:
      raise ValueError("data must be a csv filename or a pandas.DataFrame.")

    return self.fit(X, y)

  def fromTrainedModel(
    self, bn, targetAttribute, targetModality="", copy=False, threshold=0.5, variableList=None, dtype=str
  ):
    """
    parameters:
        bn: pyagrum.BayesNet
            The Bayesian network we want to use for this classifier
        targetAttribute: str
            the attribute that will be the target in this classifier
        targetModality: str
            If this is a binary classifier we have to specify which modality we are looking at if the target
            attribute has more than 2 possible values
            if !="", a binary classifier is created.
            if =="", a classifier is created that can be non-binary depending on the number of modalities
            for targetAttribute. If binary, the second one is taken as targetModality.
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

    self.fromModel_ = True

    # the set of the names of all the variables in the Bayesian network
    namesSet = set(bn.names())

    # The target specified must be a variable in the Bayesian network
    if targetAttribute not in namesSet:
      raise ValueError("the target variable does not appear in the Bayesian network")

    self.target_ = targetAttribute

    self.learner_ = None

    if copy:
      self.bn_ = gum.BayesNet(bn)
    else:
      self.bn_ = bn

    self.threshold_ = threshold

    self.MarkovBlanket_ = compileMarkovBlanket(self.bn_, self.target_)

    self.variableNameIndexDictionary_ = dict()
    # if the user specified an order for the variables then we use this order
    if variableList is not None:
      if len(namesSet) - 1 != len(variableList):
        raise ValueError("variableList should include all variables in the Bayesian network except the target")

      for i, name in enumerate(variableList):
        if name not in namesSet:
          raise ValueError("variableList includes a name that does not appear in the Bayesian network")
        self.variableNameIndexDictionary_[name] = i

    else:
      non_target_names = [n for n in bn.names() if n != self.target_]
      self.variableNameIndexDictionary_ = {name: i for i, name in enumerate(non_target_names)}

    if targetModality != "":
      self.label_ = targetModality
    else:
      self.label_ = ""

    if dtype not in (str, int, numpy.int32, numpy.int64):
      raise ValueError(f"dtype={dtype} non supporté dans fromTrainedModel. Utilisez str (défaut) ou int.")

    raw_classes = numpy.array(
      [self.bn_.variable(self.target_).label(i) for i in range(self.bn_.variable(self.target_).domainSize())]
    )
    try:
      self.classes_ = raw_classes.astype(dtype)
    except (ValueError, TypeError):
      raise ValueError(f"Impossible de convertir les classes en {dtype}: les labels du BN sont {raw_classes.tolist()}.")

    self.targetType_ = self.classes_.dtype

    self.n_features_in_ = len(self.variableNameIndexDictionary_)

  def changeVariableName(self, oldName, newName):
    """
    Changes the name of a variable inside the Bayesian network

    Parameters
    ----------
          oldName: str
              the old name of the variable
          newName: str
              the new name of the variable
    """
    if oldName == self.target_:
      self.bn_.changeVariableName(oldName, newName)
      self.target_ = newName
      self.MarkovBlanket_.changeVariableName(oldName, newName)
      return

    if oldName not in self.variableNameIndexDictionary_:
      raise ValueError("The oldName you have specified is not a name of a variable in the Bayesian network")
    index = self.variableNameIndexDictionary_.pop(oldName)

    self.variableNameIndexDictionary_[newName] = index

    self.bn_.changeVariableName(oldName, newName)

    if oldName in self.MarkovBlanket_.names():
      self.MarkovBlanket_.changeVariableName(oldName, newName)

  def model(self) -> gum.BayesNet:
    """
    Returns a copy of the Bayesian network used by this classifier.

    Returns
    -------
    pyagrum.BayesNet
      A copy of the underlying Bayesian network.
    """
    return gum.BayesNet(self.bn_)

  # ------------------method Markov Blanket and predict---------------------

  def _prepare_X(self, X: pandas.DataFrame | numpy.ndarray | str) -> tuple[numpy.ndarray, dict]:
    """Convert input X to a numpy array and build the column-name-to-index dict."""
    if isinstance(X, str):
      X, _ = self.XYfromCSV(X, target=self.target_)

    if isinstance(X, pandas.DataFrame):
      dictName = DFNames(X)
      vals = X.to_numpy()
      named = True
    else:
      dictName = self.variableNameIndexDictionary_
      vals = numpy.asarray(X)
      named = False

    if self.fromModel_:
      # For named input (DataFrame/CSV), column mapping via dictName handles any layout,
      # so extra or differently-ordered columns are fine — skip sklearn's feature count check.
      # For unnamed input (ndarray), no column mapping is possible, so enforce feature count.
      if not named and vals.shape[1] != self.n_features_in_:
        raise ValueError(
          f"X has {vals.shape[1]} features, but {self.__class__.__name__} "
          f"is expecting {self.n_features_in_} features as input."
        )
      vals = vals.astype(str)
    else:
      with warnings.catch_warnings():
        # BNClassifier handles feature-name mapping via dictName independently of sklearn's tracking,
        # so mismatches between fit-time and predict-time feature name presence are harmless.
        warnings.filterwarnings("ignore", message="X does not have valid feature names", category=UserWarning)
        warnings.filterwarnings("ignore", message="X has feature names, but", category=UserWarning)
        vals = sklearn.utils.validation.validate_data(self, vals, dtype=None, reset=False)

    return vals, dictName

  def predict(self, X):
    """
    Predicts the most likely class for each row of input data, with bn's Markov Blanket

    Parameters
    ----------
        X: str,{array-like, sparse matrix} of shape (n_samples, n_features) or str
            test data, can be either dataFrame, matrix or name of a csv file
    returns:
        y: array-like of shape (n_samples,)
            Predicted classes
    """
    sklearn.utils.validation.check_is_fitted(self)

    X, dictName = self._prepare_X(X)

    if len(self.classes_) == 2 and self.label_:
      returned_list = self._binary_predict(X, dictName)
    else:
      returned_list = self._nary_predict(X, dictName)

    returned_list = numpy.array(returned_list)
    if not self.fromModel_:
      if self.targetType_ == "bool":
        returned_list = returned_list == "True"
      elif numpy.issubdtype(self.targetType_, numpy.number):
        returned_list = returned_list.astype("float")
    else:
      if numpy.issubdtype(self.targetType_, numpy.number):
        returned_list = returned_list.astype(self.targetType_)
      elif self.label_ != "":
        returned_list = returned_list == self.label_

    return returned_list

  @property
  def _other_labels(self) -> list[str]:
    var = self.bn_.variable(self.target_)
    return [var.label(i) for i in range(var.domainSize()) if var.label(i) != self.label_]

  def _nary_predict(self, X, dictName) -> list[str]:
    """
    For a classifier, predicts the most likely class for each row of input data, with bn's Markov Blanket

    Parameters
    ----------
    X: {array-like, sparse matrix} of shape (n_samples, n_features) or str
            test data, can be either dataFrame, matrix or name of a csv file
      the data
    dictName: Dict[str,int]
      dictionary of the name of a variable and his column in the database

    Returns
    -------
    array-like of shape (n_samples,)
      the list of predictions as label names.
    """
    returned_list = []
    I = self.MarkovBlanket_.completeInstantiation()
    I.erase(self.target_)
    for x in X:
      vals, _ = _calcul_most_probable_for_nary_class(x, I, dictName, self.MarkovBlanket_, self.target_)
      returned_list.append(self.MarkovBlanket_.variable(self.target_).label(vals[0][0][self.target_]))

    return returned_list

  def _binary_predict(self, X, dictName) -> list[str]:
    """
    For a binary classifier, predicts the most likely class for each row of input data, with bn's Markov Blanket

    Parameters
    ----------
    X: {array-like, sparse matrix} of shape (n_samples, n_features) or str
            test data, can be either dataFrame, matrix or name of a csv file
      the datas
    dictName: Dict[str,int]
      dictionary of the name of a variable and his column in the database

    Returns
    -------
    array-like of shape (n_samples,)
      the list of predictions
    """
    labels = self._other_labels
    label0 = labels[0]
    label1 = self.label_
    I = self.MarkovBlanket_.completeInstantiation()
    returned_list = []
    for x in X:
      res = round(
        _calcul_proba_for_binary_class(x, label1, labels, I, dictName, self.MarkovBlanket_, self.target_),
        self.significant_digit,
      )

      if res >= self.threshold_:
        returned_list.append(label1)
      else:
        returned_list.append(label0)

    return returned_list

  # ------------------interaction with sklearn, pour ROC et Precision-Recall ---------------------

  def predict_proba(self, X):
    """
    Predicts the probability of classes for each row of input data, with bn's Markov Blanket.

    Warnings
    ---------
    For binary classifiers, the raw posterior P(target=label1 | evidence) is piecewise-linearly recalibrated so that argmax(predict_proba(X)) always
    matches predict(X), regardless of the decision threshold.
    The mapping is: p → 0.5·p/t if p < t, else 0.5 + 0.5·(p - t)/(1 - t), where t = ``self.threshold_``. When t = 0.5 the transformation is the identity.


    Parameters
    ----------
    X: str or {array-like, sparse matrix} of shape (n_samples, n_features) or str
            test data, can be either dataFrame, matrix or name of a csv file

    Returns
    -------
    array-like of shape (n_samples,)
      Predicted probability for each classes
    """
    sklearn.utils.validation.check_is_fitted(self)

    vals, dictName = self._prepare_X(X)

    returned_list = []
    label1 = self.label_
    labels = self._other_labels
    I = self.MarkovBlanket_.completeInstantiation()

    if len(self.classes_) == 2 and self.label_:
      for x in vals:
        res = round(
          _calcul_proba_for_binary_class(x, label1, labels, I, dictName, self.MarkovBlanket_, self.target_),
          self.significant_digit,
        )
        if self.threshold_ != 0.5:
          res_cal = (
            (0.5 + 0.5 * (res - self.threshold_) / (1 - self.threshold_))
            if res >= self.threshold_
            else (0.5 * res / self.threshold_)
          )
        else:
          res_cal = res
        returned_list.append([1 - res_cal, res_cal])
    else:
      local_inst = gum.Instantiation(I)
      local_inst.erase(self.target_)
      for x in vals:
        returned_list.append(
          _calcul_proba_for_nary_class(x, local_inst, dictName, self.MarkovBlanket_, self.target_).tolist()
        )

    return numpy.array(returned_list)

  # ------------------ BNClassifier compatible from pyagrum to sklearn ---------------------

  def XYfromCSV(self, filename, with_labels=True, target=None):
    """
    Reads the data from a csv file and separates it into an X matrix and a y column vector.

    Parameters
    ----------
    filename: str
        the name of the csv file
    with_labels: bool
        tells us whether the csv includes the labels themselves or their indexes.
    target: str or None
        The name of the column that will be put in the dataframe y. If target is None, we use the target that is
        already specified in the classifier

    Returns
    -------
    Tuple(pandas.Dataframe,pandas.Dataframe)
        Matrix X containing the data,Column-vector containing the class for each data vector in X
    """
    if getattr(self, "fromModel_", False):
      dataframe = pandas.read_csv(filename, dtype="str")
    else:
      dataframe = pandas.read_csv(filename)

    if target is None:
      target = self.target_
    y = dataframe[target]
    X = dataframe.drop(target, axis=1)

    if not with_labels:
      variableList = X.columns.tolist()
      targetVariable = self.bn_.variableFromName(target)
      for index in range(len(variableList)):
        variableList[index] = self.bn_.variableFromName(variableList[index])
      for row in X:
        for i in range(len(row)):
          row[i] = variableList[i].labels(row[i])
      if getattr(self, "fromModel_", False):
        if len(self.classes_) == 2:
          labelIndex = 0
          labelList = targetVariable.labels()
          while labelIndex < len(labelList):
            if labelList[labelIndex] == self.label_:
              break
            labelIndex += 1
          y = y == labelIndex
      else:
        for index in range(len(y)):
          y[index] = targetVariable(y[index])

    elif getattr(self, "fromModel_", False):
      y = y.astype("str")
      if self.label_:
        y = y == self.label_

    return X, y

  def preparedData(self, X=None, y=None, data=None):
    """
    Given an X and a y (or a data source : filename or pandas.DataFrame),
    returns a pandas.Dataframe with the prepared (especially discretized) values of the base

    Parameters
    ----------
        X: {array-like, sparse matrix} of shape (n_samples, n_features)
            training data. Warning: Raises ValueError if either filename or targetname is not None. Raises ValueError
            if y is None.
        y: array-like of shape (n_samples)
            Target values. Warning: Raises ValueError if either filename or targetname is not None. Raises ValueError
            if X is None
        data: Union[str,pandas.DataFrame]
            specifies the csv file or the DataFrame where the data values are located. Warning: Raises ValueError
            if either X or y is not None.

    Returns
    -------
      pandas.Dataframe
    """
    if self.variableNameIndexDictionary_ is None:
      raise ValueError("First, you need to fit a model !")

    targetName = self.target_
    if data is None:
      if X is None or y is None:
        raise ValueError(
          "This function should be used either as preparedData(X,y) or preparedData(data=...). You have not "
          "entered a csv file name and not specified the X and y matrices that should be used"
        )
    else:
      if targetName is None:
        raise ValueError(
          "This function should be used either as preparedData(X,y) or preparedData(data=...). The name of the "
          "target must be specified if using this function with a csv file."
        )
      if X is not None or y is not None:
        raise ValueError(
          "This function should be used either as preparedData(X,y) or preparedData(data=...). You have entered "
          "a data source and the X and y matrices at the same time."
        )

      if isinstance(data, str):
        X, y = self.XYfromCSV(data, True, targetName)
      else:  # pandas.DataFrame
        y = data[targetName]
        X = data.drop(targetName, axis=1)

    reverse = {v: k for k, v in self.variableNameIndexDictionary_.items()}
    if isinstance(X, pandas.DataFrame):
      X = X.rename(columns=reverse)
    varY = self.bn_.variable(self.target_)
    columns = [reverse[k] for k in range(len(reverse))] + [self.target_]
    is_df = isinstance(X, pandas.DataFrame)

    rows = []
    for n in range(len(X)):
      ligne = []
      for k in range(len(reverse)):
        val = X[reverse[k]][n] if is_df else X[n][k]
        var = self.bn_.variable(reverse[k])
        ligne.append(_bestTypedVal(var, var[str(val)]))
      ligne.append(_bestTypedVal(varY, varY[str(y[n])]))
      rows.append(ligne)

    return pandas.DataFrame(rows, columns=columns)

  def __getstate__(self):
    """
    Returns the state of the object (used for pickling)

    Warnings
    --------
    This method is used for pickling and should not be called directly.
    Note that the "learning side" of the BNClassifier is not in the state which focus on the classifier itself.

    Returns
    -------
    the state as a dictionary.
    """
    return {
      "bn": self.bn_,
      "target": self.target_,
      "targetType": self.targetType_,
      "label": self.label_,
      "fromModel": self.fromModel_,
      "threshold": self.threshold_,
      "variableNameIndexDictionary": self.variableNameIndexDictionary_,
      "params": self.get_params(),
      "discretizer": getattr(self, "type_processor", None),
      "classes": self.classes_,
      "n_features_in": self.n_features_in_,
    }

  def __setstate__(self, state):
    """
    Sets the state of the object (used for pickling)

    Warnings
    --------
    This method is used for pickling and should not be called directly.
    Note that the "learning side" of the BNClassifier is not in the state which focus on the classifier itself.

    Parameters
    ----------
    state : dict
      the state of the object as a dictionary

    Returns
    -------
      self
    """
    self.__init__(type_processor=DiscreteTypeProcessor())
    self.fromTrainedModel(bn=state["bn"], targetAttribute=state["target"], targetModality=state["label"], copy=False)

    self.targetType_ = state["targetType"]

    self.fromModel_ = state["fromModel"]
    self.threshold_ = state["threshold"]
    self.variableNameIndexDictionary_ = state["variableNameIndexDictionary"]
    self.set_params(**state["params"])

    if state["discretizer"] is not None:
      self.type_processor = state["discretizer"]

    self.classes_ = state["classes"]
    self.n_features_in_ = state["n_features_in"]

    return self

  def showROC_PR(self, data, *, beta=1, save_fig=False, show_progress=False, bgcolor=None):
    """
    Use the `pyagrum.lib.bn2roc` tools to create ROC and Precision-Recall curve

    Parameters
    ----------
    data: str | dataframe
        a csv filename or a DataFrame
    beta : float
        the value of beta for the F-beta score
    save_fig : bool
        whether the graph should be saved
    show_progress : bool
        indicates if the resulting curve must be printed
    bgcolor: str
        HTML background color for the figure (default: None if transparent)
    """
    import pyagrum.lib.bn2roc as bn2roc

    bn2roc.showROC_PR(
      self.bn_,
      data,
      self.target_,
      self.label_,
      beta=beta,
      significant_digits=self.significant_digit,
      save_fig=save_fig,
      show_progress=show_progress,
      bgcolor=bgcolor,
    )
