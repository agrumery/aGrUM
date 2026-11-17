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
This module contains the DiscreteTypeProcessor class used to attribute a type from the values of a database before learning a (discrete) Graphical Model.
Particularly, the DiscreteTypeProcessor class is used to discretize some continuous variables in a database.

The discretization is done using the following methods: 'quantile', 'uniform', 'kmeans', 'NML', 'CAIM' and 'MDLP'.
Some methods need specialized treatment of arguments for classification (y separated from X).
"""

import math
from typing import Any

import numpy
import pandas
import scipy
import sklearn
import sklearn.preprocessing as skp

import pyagrum


def check_int(v: Any) -> bool:
  """
  Test if v is an int or a str representing an int
  """
  if isinstance(v, (bool, numpy.bool_)):
    return False
  if isinstance(v, (int, numpy.integer)):
    return True

  if isinstance(v, str):
    if v[0] in ("-", "+"):
      return v[1:].isdigit()
    return v.isdigit()
  return False


def check_float(v: Any) -> bool:
  """
  Test if v is a float or a str representing a float.

  Parameters
  ----------
  v : Any

  Returns
  -------
  bool:
    True if v is a float or a str representing a float
  """
  if isinstance(v, (bool, numpy.bool_)):
    return False

  try:
    float(v)
    return True
  except ValueError:
    return False


class DiscreteTypeProcessor:
  """
  Represents a tool to process the type of the variables in a database in order to obtain  a way to learn a pyAgrum's discrete Graphical Model.
  Particularly, the DiscreteTypeProcessor class is used to discretize some continuous variables in a database.

  Warnings
  --------
  - The data are represented by tabular data (X and possibly y) where the columns are the variables and the rows are the samples. Generally, X can be replaced by a the name of a csv file.
  - In the case of a classification, y is the class variable and X are the features. y may have any number of classes.

  Parameters
  ----------
      defaultDiscretizationMethod: str
            sets the default method of discretization for this discretizer. Possible values are: ``quantile``, ``uniform``,
            ``kmeans``, ``NML``, ``CAIM`` and ``MDLP``. This method will be used if the user has not specified another method
            for that specific variable using the setDiscretizationParameters method.
      defaultNumberOfBins: str or int
          sets the number of bins if the method used is ``quantile``, ``kmeans``, ``uniform``. In this case this parameter can also
          be set to the string ``elbowMethod`` so that the best number of bins is found automatically.
          If the method used is NML, this parameter sets the the maximum number of bins up to which the NML algorithm
          searches for the optimal number of bins. In this case this parameter must be an int
          If any other discretization method is used, this parameter is ignored.
      discretizationThreshold: int or float
          When using default parameters a variable will be treated as continuous only if it has more unique values than
          this number (if the number is an int greater than 1).
          If the number is a float between 0 and 1, we will test if the proportion of unique values is bigger than this
          number. For example if you have entered 0.95, the variable will be treated as continuous only if more than 95%
          of its values are unique.

  """

  def __init__(
    self,
    defaultDiscretizationMethod: str = "quantile",
    defaultNumberOfBins: int | str = 10,
    discretizationThreshold: float = 25,
  ) -> None:
    """
    Initializes the DiscreteTypeProcessor object.

    Parameters
    ----------
        defaultDiscretizationMethod: str
            sets the default method of discretization for this discretizer. Possible values are: 'quantile', 'uniform',
            'kmeans', 'NML', 'CAIM' and 'MDLP'. This method will be used if the user has not specified another method
            for that specific variable using the setDiscretizationParameters method.
        defaultNumberOfBins: str or int
            sets the number of bins if the method used is quantile, kmeans, uniform. In this case this parameter can
            also be set to the string 'elbowMethod' so that the best number of bins is found automatically.
            If the method used is NML, this parameter sets the the maximum number of bins up to which the NML algorithm
            searches for the optimal number of bins. In this case this parameter must be an int
            If any other discretization method is used, this parameter is ignored.
        discretizationThreshold: int or float
            When using default parameters a variable will be treated as continuous only if it has more unique values than
            this number (if the number is an int greater than 1).
            If the number is a float between 0 and 1, we will test if the proportion of unique values is bigger than
            this number. For example if you have entered 0.95, the variable will be treated as continuous only if more
            than 95% of its values are unique.
    """
    self.discretizationParametersDictionary: dict[str, Any] = {}
    self.numberOfContinuous = 0
    self.totalNumberOfBins = 0
    self.defaultMethod = None
    self.defaultParamDiscretizationMethod = None
    self.setDiscretizationParameters(None, defaultDiscretizationMethod, defaultNumberOfBins)

    self.discretizationThreshold = discretizationThreshold

  def clear(self, clearDiscretizationParameters: bool = False) -> None:
    """
    Sets the number of continuous variables and the total number of bins created by this discretizer to 0. If
    clearDiscretizationParameters is True, also clears the the parameters for discretization the user has set for each
    variable.

    Parameters
    ----------
      clearDiscretizationParameters: bool
        if True, this method also clears the parameters the user has set for each variable and resets them to the default.
    """
    self.numberOfContinuous = 0
    self.totalNumberOfBins = 0
    if clearDiscretizationParameters:
      self.discretizationParametersDictionary = {}

  @pyagrum.deprecated_arg(newA="parameters", oldA="paramDiscretizationMethod", version="2.0.0")
  def setDiscretizationParameters(self, variableName: str | None, method: str, parameters: Any = None) -> None:
    """
    Sets the discretization parameters for a variable. If variableName is None, sets the default parameters.

    Parameters
    ----------
      variableName: str
          the name of the variable you want to set the discretization parameters of. Set to None to set the new
          default.
      method: str
          The method of discretization used for this variable. Use "NoDiscretization" if you do not want to discretize this
          variable. Possible values are: 'NoDiscretization', 'quantile', 'uniform', 'kmeans', 'NML', 'CAIM', 'MDLP' and 'expert'
      parameters:
          Each method of discretization has a parameter that can be set.
          - 'quantile': the number of bins
          - 'kmeans', 'uniform': the number of bins. The parameter can also be set to the string 'elbowMethod' so that the best
          number of bins is found automatically.
          - 'NML': this parameter sets the the maximum number of bins up to which the NML algorithm searches for the optimal
          number of bins.
          - 'MDLP', 'CAIM': this parameter is ignored
          - 'expert': this parameter is the set of ticks proposed by the expert. The discretized variable will set the flag
          'empirical' which means that if the values found in the data are not in the proposed intervals, they did not raise
          any exception but  are nevertheless accepted (as belonging to the smallest or biggest interval).
          - 'NoDiscretization': this parameter is a superset of the values for the variable found in the database (or None).
    """
    if parameters is None:
      parameters = self.defaultParamDiscretizationMethod

    match method:
      case "quantile" | "NML":
        if type(parameters) is not int:
          raise ValueError(
            "The parameter for the quantile/NML method must be an integer. You have entered: " + str(parameters)
          )
      case "kmeans" | "uniform":
        if type(parameters) is not int and str(parameters) != "elbowMethod":
          raise ValueError(
            "The parameter for the kmeans/uniform method must be an integer or the string 'elbowMethod'. You have entered: "
            + str(parameters)
          )
      case "expert":
        if not (isinstance(parameters, list) and all(map(check_float, parameters))):
          raise ValueError(
            "The parameter for the expert method must be a list of float. You have entered: " + str(parameters)
          )
      case "NoDiscretization":
        if parameters is not None and not (isinstance(parameters, str)):
          raise ValueError(
            "The parameter for the NoDiscretization method must be a string (fastVar syntax) or None. You have "
            "entered: " + str(parameters)
          )
      case "CAIM" | "MDLP":
        pass
      case _:
        raise ValueError(
          "This discretization method is not recognized! Possible values are kmeans, uniform, quantile, NML, "
          "CAIM, MDLP, NoDiscretization or expert. You have entered " + str(method)
        )

    if variableName is None:
      self.defaultMethod = method
      self.defaultParamDiscretizationMethod = parameters
    else:
      self.discretizationParametersDictionary[variableName] = {}
      self.discretizationParametersDictionary[variableName]["method"] = method
      self.discretizationParametersDictionary[variableName]["param"] = parameters

  def audit(self, X: Any, y: Any = None) -> dict[str, Any]:
    """
    Audits the passed values of X and y. Guess which columns in X are already discrete and which need to
    be discretized, as well as the discretization algorithm that will be used to discretize them The parameters which
    are suggested will be used when creating the variables. To change this the user can manually set discretization
    parameters for each variable using the setDiscretizationParameters function.

    Parameters
    ----------
      X: {array-like, pandas or polars dataframe} of shape (n_samples, n_features) or str (filename)
          training data
      y: {array-like, pandas or polars dataframe} of shape (n_samples,) or str (classname)
          Target values
    Returns
    -------
      Dict
        for each variable, the proposition of audit
    """
    if isinstance(X, str):
      Xp = pandas.read_csv(X)
    elif hasattr(X, "to_pandas"):  # for instance, polars dataframe
      Xp = X.to_pandas()
    else:
      Xp = X

    if isinstance(y, str):
      yp = Xp[y]
    elif y is not None and hasattr(y, "to_pandas"):  # for instance, polars dataframe
      yp = y.to_pandas()
    else:
      yp = y

    return self._audit(Xp, yp)

  def _audit(self, X, y=None):
    """
    Audits the passed values of X and y. Tells us which columns in X we think are already discrete and which need to
    be discretized, as well as the discretization algorithm that will be used to discretize them The parameters which
    are suggested will be used when creating the variables. To change this the user can manually set discretization
    parameters for each variable using the setDiscretizationParameters function.

        Parameters
    ----------
      X: {array-like, sparse matrix} of shape (n_samples, n_features)
          training data
      y: array-like of shape (n_samples,)
          Target values
    Returns
    -------
      Dict
        for each variable, the proposition of audit
    """

    auditDict = {}

    if isinstance(X, pandas.DataFrame):
      variableNames = X.columns.tolist()
    elif type(X) is pandas.core.series.Series:
      variableNames = [X.name]
    else:
      variableNames = None

    if y is not None:
      X, y = sklearn.utils.check_X_y(X, y, dtype=None, accept_sparse=True)
    else:
      X = sklearn.utils.check_array(X, dtype=None)

    d = X.shape[1]

    if variableNames is None:
      variableNames = ["x" + str(i) for i in range(d)]

    possibleValues = {}  # counting the possible values gor this line

    for i in range(d):
      possibleValues[i] = numpy.unique(X[:, i])
    possibleValues[d] = numpy.unique(y)

    for i in range(d):
      variable = variableNames[i]
      auditDict[variable] = {}
      try:
        sklearn.utils.check_array(X[:, i], dtype="float", ensure_2d=False)
        isNumeric = True
      except ValueError:
        isNumeric = False
      if variable in self.discretizationParametersDictionary.keys():
        auditDict[variable] = self.discretizationParametersDictionary[variable]
        if self.discretizationParametersDictionary[variable]["method"] != "NoDiscretization" and not isNumeric:
          raise ValueError("The variable " + variable + " is not numeric and cannot be discretized!")
      else:
        if len(possibleValues[i]) > self.discretizationThreshold and isNumeric:
          auditDict[variable]["method"] = self.defaultMethod
          auditDict[variable]["nbBins"] = self.defaultParamDiscretizationMethod
        else:
          auditDict[variable]["method"] = "NoDiscretization"
          auditDict[variable]["values"] = possibleValues[i]

      if auditDict[variable]["method"] == "NoDiscretization":
        auditDict[variable]["type"] = "Discrete"
      else:
        auditDict[variable]["type"] = "Continuous"
        auditDict[variable]["minInData"] = min(possibleValues[i])
        auditDict[variable]["maxInData"] = max(possibleValues[i])

    return auditDict

  @staticmethod
  def _discretizationElbowMethodRotation(discretizationStrategy, X):
    """
    Calculates the sum of squared errors as a function of the number of clusters using the discretization strategy
    that is passed as a parameter. Returns the bins that are optimal for minimizing the variation and the number of
    bins at the same time. Uses the elbow method to find this optimal point. To find the "elbow" we rotate the curve
    and look for its minimum.

    Parameters
    ----------
        discretizationStrategy: str
            The method of discretization that will be used. Possible values are: 'quantile' , 'kmeans' and 'uniform'
        X: one dimensional ndarray
            Contains the data that should be discretized
    Returns
    -------
        list[float]
          the edges of the bins the algorithm has chosen.
    """

    if discretizationStrategy not in {"kmeans", "quantile", "uniform"}:
      raise ValueError("cannot use elbow method with this type of discretization")
    nbBins = range(2, 16)
    variationArray = numpy.zeros(len(nbBins))
    Xsorted = X[X.argsort(axis=None)]
    binEdgeMatrix = []
    for index, k in enumerate(nbBins):
      discretizer = skp.KBinsDiscretizer(
        k,
        strategy=discretizationStrategy,
        quantile_method="averaged_inverted_cdf",
        subsample=None,
      )
      discretizer.fit(Xsorted.reshape(-1, 1))
      binEdges = discretizer.bin_edges_[0]
      centresArray = (binEdges[1:] + binEdges[:-1]) / 2
      i = 0
      sumOfSquaredErrors = 0
      for x in Xsorted:
        while i < len(centresArray) - 1 and x > binEdges[i + 1]:
          i = i + 1
        sumOfSquaredErrors += (x - centresArray[i]) ** 2
      variationArray[index] = sumOfSquaredErrors
      binEdgeMatrix.append(binEdges.tolist())

    # the elbow is the point of the curve that is the farthest from the chord joining its first and last points
    # (the distance is proportional to the vertical gap to the chord); a flat curve gives the smallest number of bins
    last = len(nbBins) - 1
    chord = variationArray[0] + (variationArray[last] - variationArray[0]) * numpy.arange(len(nbBins)) / last
    return binEdgeMatrix[int(numpy.argmax(numpy.abs(variationArray - chord)))]

  @staticmethod
  def _classCountsByValue(X, y, possibleValuesX, possibleValuesY):
    """
    Counts the samples of each class for each distinct value of x.

    Parameters
    ----------
        X: ndarray with shape (n,1) where n is the number of samples
            Column-vector that contains all the data that needs to be discretized
        y: ndarray with shape (n,1) where n is the number of samples
            Column-vector that contains the class for each sample
        possibleValuesX: one dimensional ndarray
            Contains all the possible values that x can take sorted in increasing order, without duplicates
        possibleValuesY: one dimensional ndarray
            Contains the possible values of y
    Returns
    -------
        ndarray
          a matrix whose row i contains the number of samples of each class among those whose value is possibleValuesX[i]
    """
    classIndex = {c: i for i, c in enumerate(possibleValuesY)}
    try:
      classes = numpy.array([classIndex[c] for c in y.ravel()], dtype=int)
    except KeyError as e:
      raise ValueError(f"The class {e.args[0]} is not in the possible values of y ({list(possibleValuesY)})") from None
    counts = numpy.zeros((len(possibleValuesX), len(classIndex)))
    numpy.add.at(counts, (numpy.searchsorted(possibleValuesX, X.ravel()), classes), 1)
    return counts

  @staticmethod
  def _entropy(counts):
    """
    Shannon entropy (in bits) of the class distribution of each row of counts (or of counts if it is a vector).
    """
    total = counts.sum(axis=-1, keepdims=True)
    p = numpy.divide(counts, total, out=numpy.zeros_like(counts), where=total > 0)
    return -(p * numpy.log2(p, out=numpy.zeros_like(p), where=p > 0)).sum(axis=-1)

  def _discretizationMDLP(self, X, y, possibleValuesX, possibleValuesY):
    """
    Uses the MDLP algorithm described in Fayyad and Irani, 1993 to discretize the values of x.

    Parameters
    ----------
        X: ndarray with shape (n,1) where n is the number of samples
            Column-vector that contains all the data that needs to be discretized
        y: ndarray with shape (n,1) where n is the number of samples
            Column-vector that contains the class for each sample. This vector will not be discretized, but the class-value of each sample is needed to properly apply the algorithm
        possibleValuesX: one dimensional ndarray
            Contains all the possible values that x can take sorted in increasing order. There shouldn't be any doubles inside
        possibleValuesY: one dimensional ndarray
            Contains the possible values of y.
    Returns
    -------
        list[float]
         a list of the edges of the bins that are chosen by this algorithm
    """
    counts = self._classCountsByValue(X, y, possibleValuesX, possibleValuesY)
    cuts = []
    self._splitIntervalMDLP(counts, 0, len(possibleValuesX), cuts)
    B = (possibleValuesX[1:] + possibleValuesX[:-1]) / 2
    return [possibleValuesX[0]] + [B[cut - 1] for cut in sorted(cuts)] + [possibleValuesX[-1]]

  @staticmethod
  def _splitIntervalMDLP(counts, low, high, cuts):
    """
    Recursively splits the interval of the values of indices low, ..., high-1 with the MDLP criterion. A cut c separates
    the values of indices low, ..., c-1 from the values of indices c, ..., high-1; accepted cuts are appended to cuts.
    """
    if high - low < 2:
      return
    total = counts[low:high].sum(axis=0)
    left = numpy.cumsum(counts[low : high - 1], axis=0)
    right = total - left
    N = total.sum()
    leftEntropy = DiscreteTypeProcessor._entropy(left)
    rightEntropy = DiscreteTypeProcessor._entropy(right)
    classInformationEntropy = (left.sum(axis=1) * leftEntropy + right.sum(axis=1) * rightEntropy) / N
    best = int(numpy.argmin(classInformationEntropy))

    entropy = DiscreteTypeProcessor._entropy(total)
    gain = entropy - classInformationEntropy[best]
    # numbers of classes present in the interval and in both sub-intervals
    k, k1, k2 = (numpy.count_nonzero(c) for c in (total, left[best], right[best]))
    deltaS = math.log2(3**k - 2) - (k * entropy - k1 * leftEntropy[best] - k2 * rightEntropy[best])
    if gain <= (math.log2(N - 1) + deltaS) / N:
      return

    cut = low + best + 1
    cuts.append(cut)
    DiscreteTypeProcessor._splitIntervalMDLP(counts, low, cut, cuts)
    DiscreteTypeProcessor._splitIntervalMDLP(counts, cut, high, cuts)

  @staticmethod
  def _discretizationCAIM(X, y, possibleValuesX, possibleValuesY):
    """
    Applies the CAIM algorithm (Kurgan and Cios, 2004) to discretize the values of x

    Parameters
    ----------
        X: ndarray with shape (n,1) where n is the number of samples
            Column-vector that contains all the data that needs to be discretized
        y: ndarray with shape (n,1) where n is the number of samples
            Column-vector that contains the class for each sample. This vector will not be discretized, but the class-value of each sample is needed to properly apply the algorithm
        possibleValuesX: one dimensional ndarray
            Contains all the possible values that x can take sorted in increasing order. There shouldn't be any doubles inside
        possibleValuesY: one dimensional ndarray
            Contains the possible values of y.
    Returns
    -------
        list[float]
          a list of the edges of the bins that are chosen by this algorithm
    """
    counts = DiscreteTypeProcessor._classCountsByValue(X, y, possibleValuesX, possibleValuesY)
    nbClasses = numpy.count_nonzero(counts.sum(axis=0))

    # the interval r contains the values of indices bounds[r], ..., bounds[r+1]-1
    bounds = [0, len(possibleValuesX)]
    intervalCounts = [counts.sum(axis=0)]
    globalCAIM = 0.0
    while True:
      # max_r^2 / M_r for each interval r
      terms = [c.max() ** 2 / c.sum() for c in intervalCounts]
      sumOfTerms = sum(terms)
      best = None
      for r, (low, high) in enumerate(zip(bounds[:-1], bounds[1:])):
        if high - low < 2:
          continue
        left = numpy.cumsum(counts[low : high - 1], axis=0)
        right = intervalCounts[r] - left
        caim = (
          sumOfTerms - terms[r] + left.max(axis=1) ** 2 / left.sum(axis=1) + right.max(axis=1) ** 2 / right.sum(axis=1)
        ) / (len(intervalCounts) + 1)
        i = int(numpy.argmax(caim))
        if best is None or caim[i] > best[0]:
          best = (caim[i], r, low + i + 1, left[i], right[i])

      # as in Kurgan and Cios, a cut is accepted if it increases CAIM or if there are fewer intervals than classes
      if best is None or (best[0] <= globalCAIM and len(intervalCounts) >= nbClasses):
        break
      globalCAIM, r, cut, leftCounts, rightCounts = best
      bounds.insert(r + 1, cut)
      intervalCounts[r : r + 1] = [leftCounts, rightCounts]

    B = (possibleValuesX[1:] + possibleValuesX[:-1]) / 2
    return [possibleValuesX[0]] + [B[cut - 1] for cut in bounds[1:-1]] + [possibleValuesX[-1]]

  @staticmethod
  def _regretOfTwoBins(h):
    """
    Parametric complexity (regret) of a 2-bin histogram of h samples (Kontkanen and Myllymäki, 2007, eq. 19), summed in
    log space so that the binomial coefficients do not overflow.
    """
    if h == 0:
      return 1.0
    k = numpy.arange(h + 1)
    logTerms = (
      scipy.special.gammaln(h + 1)
      - scipy.special.gammaln(k + 1)
      - scipy.special.gammaln(h - k + 1)
      + scipy.special.xlogy(k, k / h)
      + scipy.special.xlogy(h - k, (h - k) / h)
    )
    return numpy.exp(logTerms).sum()

  @staticmethod
  def _discretizationNML(X, possibleValuesX, kMax=10, epsilon=None):
    """
    Uses the discretization algorithm described in "MDL Histogram Density Estimation", Kontkanen and Myllymäki, 2007 to
    discretize.

    Parameters
    ----------
        X: one dimensional ndarray
            array that that contains all the data that needs to be discretized
        possibleValuesX: one dimensional ndarray
            Contains all the possible values that x can take sorted in increasing order. There shouldn't be any doubles
            inside.
        kMax: int
            the maximum number of bins before the algorithm stops itself.
        epsilon: float or None
            the precision of the data: the range of the data is extended by epsilon/2 on both sides. If None is passed,
            the smallest gap between two distinct values is used.
    Returns
    -------
        list[float]
          a list of the edges of the bins that are chosen by this algorithm
    """
    if epsilon is None:
      epsilon = numpy.amin(possibleValuesX[1:] - possibleValuesX[:-1])
    # candidate cut points: the midpoints of consecutive distinct values (one of the choices of section 4 of the
    # article, instead of the x +- epsilon/2 of eq. 22 which produces bins of width epsilon around isolated values),
    # with both end points: candidates[0] and candidates[-1] are always in the histogram
    candidates = numpy.concatenate(
      [
        [possibleValuesX[0] - epsilon / 2],
        (possibleValuesX[1:] + possibleValuesX[:-1]) / 2,
        [possibleValuesX[-1] + epsilon / 2],
      ]
    )
    E = len(candidates)
    # counts[e]: number of samples lower than or equal to candidates[e]
    counts = numpy.searchsorted(numpy.sort(X), candidates, side="right").astype(float)
    n = counts[-1]

    def binCost(e, start):
      # -h (log(epsilon h) - log(L n)) for the bins ]candidates[start], candidates[e]] (eq. 21)
      h = counts[e] - counts[start]
      length = candidates[e] - candidates[start]
      return -scipy.special.xlogy(h, epsilon * h / (length * n))

    # regrets of 1-bin and 2-bin histograms of the samples up to each candidate, then recursion (eq. 18)
    previousRegret = numpy.ones(E)
    regret = numpy.array([DiscreteTypeProcessor._regretOfTwoBins(int(h)) for h in counts])

    previousB = numpy.full(E, math.inf)
    previousB[1:] = binCost(numpy.arange(1, E), 0)  # eq. 30
    scores = []
    predecessors = []
    for K in range(2, kMax + 1):
      B = numpy.full(E, math.inf)
      predecessor = numpy.zeros(E, dtype=int)
      for e in range(K, E):
        starts = numpy.arange(K - 1, e)
        values = previousB[starts] + binCost(e, starts) + numpy.log(regret[e] / previousRegret[starts])
        best = int(numpy.argmin(values))
        # log of the ratio of the binomial coefficients counting the cut point sets (eq. 28)
        B[e] = values[best] + math.log((E - K) / (K - 1))
        predecessor[e] = starts[best]
      scores.append(B[-1])
      predecessors.append(predecessor)
      previousRegret, regret = regret, regret + counts * previousRegret / (K - 1)
      previousB = B

    # optimal number of bins, then the cut points by following the predecessors back from the last candidate
    bestK = int(numpy.argmin(scores)) + 2
    cuts = []
    e = E - 1
    for K in range(bestK, 1, -1):
      e = predecessors[K - 2][e]
      cuts.append(e)
    return [candidates[0]] + [candidates[e] for e in reversed(cuts)] + [candidates[-1]]

  def _createVariable(self, variableName, X, y=None, possibleValuesY=None):
    """
    Creates a variable for the column passed in as a parameter

    Parameters
    ----------
        variableName:
            the name of the created variable
        X: ndarray shape(n,1)
            A column vector containing n samples of a feature. The column for which the variable will be created
        y: ndarray shape(n,1)
            A column vector containing the corresponding for each element in X.
        possibleValuesY: ndarray
            An ndarray containing all the unique values of y
    Returns
    -------
        pyagrum.DiscreteVariable
            the created variable
    """

    if y is not None:
      X, y = sklearn.utils.check_X_y(X, y, dtype=None, accept_sparse=True, ensure_2d=False)
    X = sklearn.utils.check_array(X, dtype=None, ensure_2d=False)
    try:
      Xtransformed = sklearn.utils.check_array(X, dtype="float", ensure_2d=False)
      isNumeric = True
    except ValueError:
      Xtransformed = X
      isNumeric = False

    foundValuesX = set(numpy.unique(X))
    n = len(X)

    if (
      variableName not in self.discretizationParametersDictionary
    ):  # The user has not manually set the discretization parameters for this variable
      if (
        isNumeric
        and 1 <= self.discretizationThreshold < len(foundValuesX)
        or (self.discretizationThreshold < 1 and len(foundValuesX) / len(X) > self.discretizationThreshold)
      ):
        self.discretizationParametersDictionary[variableName] = {}
        self.discretizationParametersDictionary[variableName]["method"] = self.defaultMethod
        self.discretizationParametersDictionary[variableName]["param"] = self.defaultParamDiscretizationMethod
      else:
        self.discretizationParametersDictionary[variableName] = {}
        self.discretizationParametersDictionary[variableName]["method"] = "NoDiscretization"
      usingDefaultParameters = True
    else:  # The user has manually set the discretization parameters for this variable
      usingDefaultParameters = False
      if self.discretizationParametersDictionary[variableName]["method"] != "NoDiscretization" and not isNumeric:
        raise ValueError(f"The variable {variableName} is not numeric and cannot be discretized!")

    if self.discretizationParametersDictionary[variableName]["method"] == "NoDiscretization":
      is_int_var = True

      varSyntax = ""
      if "param" in self.discretizationParametersDictionary[variableName]:
        varSyntax = self.discretizationParametersDictionary[variableName]["param"]
        if varSyntax is None:
          varSyntax = ""

      if varSyntax != "":
        var = pyagrum.fastVariable(pyagrum.quoteFastName(variableName) + varSyntax)
        possibleValuesX = set(var.labels())
        f = {str(x) for x in foundValuesX}
        if not f.issubset(possibleValuesX):
          raise ValueError(
            f"The values passed in possibleValues ({sorted(possibleValuesX)}) do not match database values ("
            f"{sorted(f)})"
          )
        return var

      possibleValuesX = sorted(foundValuesX)
      is_int_var = all(map(check_int, possibleValuesX))
      if is_int_var:
        possibleValuesX = [int(x) for x in possibleValuesX]
        max_v = int(possibleValuesX[-1])  # sorted
        min_v = int(possibleValuesX[0])

        if len(possibleValuesX) == max_v - min_v + 1:  # no hole in the list of int
          return pyagrum.RangeVariable(variableName, variableName, min_v, max_v)
        else:
          return pyagrum.IntegerVariable(variableName, variableName, possibleValuesX)

      is_float_var = all(map(check_float, possibleValuesX))
      if is_float_var:
        possibleValuesX = [float(x) for x in possibleValuesX]
        return pyagrum.NumericalDiscreteVariable(variableName, variableName, possibleValuesX)
      else:
        return pyagrum.LabelizedVariable(variableName, variableName, [str(v) for v in possibleValuesX])
    else:
      self.numberOfContinuous += 1
      if self.discretizationParametersDictionary[variableName]["method"] == "expert":
        binEdges = self.discretizationParametersDictionary[variableName]["param"]
      elif self.discretizationParametersDictionary[variableName]["method"] == "CAIM":
        if y is None:
          raise ValueError(
            "The CAIM discretization method requires a list of the associated classes for each data vector since it "
            "is a supervised discretization method. You should pass it as y."
          )
        if possibleValuesY is None:
          possibleValuesY = numpy.unique(y)
        binEdges = self._discretizationCAIM(
          Xtransformed.reshape(n, 1),
          y.reshape(n, 1),
          numpy.unique(Xtransformed),
          possibleValuesY,
        )
      elif self.discretizationParametersDictionary[variableName]["method"] == "MDLP":
        if y is None:
          raise ValueError(
            "The MDLP discretization method requires a list of the associated classes for each data vector since it "
            "is a supervised discretization method. You should pass it as y."
          )
        if possibleValuesY is None:
          possibleValuesY = numpy.unique(y)
        binEdges = self._discretizationMDLP(
          Xtransformed.reshape(n, 1),
          y.reshape(n, 1),
          numpy.unique(Xtransformed),
          possibleValuesY,
        )
        if len(binEdges) == 2:
          raise ValueError(
            f"MDLP found no informative cut point for the variable {variableName} with respect to the target: "
            "this variable cannot be discretized by MDLP. Use another discretization method for it "
            "(setDiscretizationParameters)."
          )
      elif self.discretizationParametersDictionary[variableName]["method"] == "NML":
        binEdges = self._discretizationNML(
          Xtransformed.flatten(),
          numpy.unique(Xtransformed),
          kMax=self.discretizationParametersDictionary[variableName]["param"],
        )
      else:
        if self.discretizationParametersDictionary[variableName]["param"] == "elbowMethod":
          binEdges = self._discretizationElbowMethodRotation(
            self.discretizationParametersDictionary[variableName]["method"],
            Xtransformed.flatten(),
          )
        else:
          discre = skp.KBinsDiscretizer(
            self.discretizationParametersDictionary[variableName]["param"],
            strategy=self.discretizationParametersDictionary[variableName]["method"],
            quantile_method="averaged_inverted_cdf",
            subsample=None,
          )
          discre.fit(X.reshape(-1, 1))
          binEdges = discre.bin_edges_[0].tolist()

      if len(binEdges) == 2:
        raise ValueError(
          "Due to an error the discretization method "
          + str(self.discretizationParametersDictionary[variableName]["method"])
          + " using "
          + str(self.discretizationParametersDictionary[variableName]["param"])
          + " bins for the variable "
          + str(variableName)
          + " gave only 1 bin. Try increasing the number of bins used by this variable using "
          "setDiscretizationParameters to avoid this error"
        )

      self.totalNumberOfBins += len(binEdges) - 1
      var = pyagrum.DiscretizedVariable(variableName, variableName, binEdges)
      var.setEmpirical(True)

    if usingDefaultParameters:
      self.discretizationParametersDictionary.pop(variableName)

    return var

  def discretizedTemplate(self, X, y=None, *, possibleValuesY=None, template=None):
    """
    return a graphical model discretized using the suggestion of the Discretized for date source X (and for target y).
    This graphical model only contains the discretized variables.
    It can be used as a template for a BNLearner.

    Parameters
    ----------
        X: {array-like, sparse matrix, pandas or polars dataframe} of shape (n_samples, n_features)) or str (filename)
            training data
        y: array-like, pandas or polars dataframe of shape (n_samples,) or str (classname)
            Target values
        possibleValuesY: ndarray
            An ndarray containing all the unique values of y
        template: a graphical model such as `pyagrum.BayesNet`, `pyagrum.MarkovRandomField`, etc...
            the template that will contain the discretized variables.
            If None, a new Bayesian network is created.

    Returns
    -------
        pyagrum.BayesNet or other graphical model:
          the discretized graphical model (only (discretized) random variables are created in the model)
    """
    if template is None:
      template = pyagrum.BayesNet()

    if isinstance(X, str):
      Xp = pandas.read_csv(X)
    elif hasattr(X, "to_pandas"):
      Xp = X.to_pandas()
    else:
      Xp = X

    if isinstance(y, str):
      yp = Xp[y]
    elif y is not None and hasattr(y, "to_pandas"):
      yp = y.to_pandas()
    else:
      yp = y

    for name in Xp:
      template.add(self._createVariable(name, Xp[name], yp, possibleValuesY))
    return template
