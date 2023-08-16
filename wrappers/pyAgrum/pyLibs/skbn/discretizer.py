# (c) Copyright 2020-2023 by Pierre-Henri Wuillemin(@LIP6)  (pierre-henri.wuillemin@lip6.fr)

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
This module contains the BNDiscretizer class which is used to discretize the values of a database in order to learn
a (discrete) Bayesian Network classifier
"""
import math

import numpy
import pandas
import scipy
import sklearn
import sklearn.preprocessing as skp

import pyAgrum as gum

from ._utils import checkInt, checkFloat


class BNDiscretizer():
  """
  Represents a tool to discretize some variables in a database in order to obtain  a way to learn a pyAgrum's (discrete)
  Bayesian networks.

  Parameters
  ----------
      defaultDiscretizationMethod: str
            sets the default method of discretization for this discretizer. Possible values are: 'quantile', 'uniform',
            'kmeans', 'NML', 'CAIM' and 'MDLP'. This method will be used if the user has not specified another method
            for that specific variable using the setDiscretizationParameters method.
      defaultNumberOfBins: str or int
          sets the number of bins if the method used is quantile, kmeans, uniform. In this case this parameter can also
          be set to the string 'elbowMethod' so that the best number of bins is found automatically.
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

  def __init__(self, defaultDiscretizationMethod='quantile', defaultNumberOfBins=10, discretizationThreshold=25):
    """
    Creates the BNDiscretizer

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
    self.discretizationParametersDictionary = {}
    self.numberOfContinuous = 0
    self.totalNumberOfBins = 0
    self.defaultMethod = None
    self.defaultParamDiscretizationMethod = None
    self.setDiscretizationParameters(None, defaultDiscretizationMethod, defaultNumberOfBins)

    self.discretizationThreshold = discretizationThreshold

  def clear(self, clearDiscretizationParameters=False):
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

  @gum.deprecated_arg(newA="paramDiscretizationMethod", oldA="numberOfBins", version="1.9.0")
  def setDiscretizationParameters(self, variableName=None, method=None, paramDiscretizationMethod=None):
    """
    Sets the discretization parameters for a variable. If variableName is None, sets the default parameters for this

    parameters:
      variableName: str
          the name of the variable you want to set the discretization parameters of. Set to None to set the new
          default for this BNClassifier.
      method: str
          The method of discretization used for this variable. Type "NoDiscretization" if you do not want to discretize this
          variable. Possible values are: 'NoDiscretization', 'quantile', 'uniform', 'kmeans', 'NML', 'CAIM' and 'MDLP'
      paramDiscretizationMethod:
          sets the number of bins if the method used is quantile, kmeans, uniform. In this case this parameter can also
          be set to the string 'elbowMethod' so that the best number of bins is found automatically.
          if the method used is NML, this parameter sets the the maximum number of bins up to which the NML algorithm
          searches for the optimal number of bins. In this case this parameter must be an int
          If the method is NoDiscretization, this parameter can 
          If any other discretization method is used, this parameter is ignored.
  
    """
    if variableName in self.discretizationParametersDictionary:
      oldParamDiscretizationMethod = self.discretizationParametersDictionary[variableName]['param']
      oldMethod = self.discretizationParametersDictionary[variableName]['method']
    else:
      oldParamDiscretizationMethod = self.defaultParamDiscretizationMethod
      oldMethod = self.defaultMethod

    if method is None:
      method = oldMethod

    if paramDiscretizationMethod is None and method != 'NoDiscretization':
      paramDiscretizationMethod = oldParamDiscretizationMethod

    if method not in {'kmeans', 'uniform', 'quantile', 'NML', 'MDLP', 'CAIM', 'NoDiscretization'}:
      raise ValueError(
        "This discretization method is not recognized! Possible values are kmeans, uniform, quantile, NML, "
        "CAIM and MDLP. You have entered " + str(
          method))

    if paramDiscretizationMethod == 'elbowMethod':
      if method == "NML":
        raise ValueError(
          "The elbow Method cannot be used as the number of bins for the algorithm NML. Please select an integer value")
    elif method != 'NoDiscretization':
      try:
        paramDiscretizationMethod = int(paramDiscretizationMethod)
      except:
        raise ValueError(
          "The possible values for paramDiscretizationMethod are any integer or the string 'elbowMethod'. You have entered: " + str(
            paramDiscretizationMethod))
    else:
      if paramDiscretizationMethod is not None and not isinstance(paramDiscretizationMethod, list):
        raise ValueError(
          "For a NotDiscretized variable, the parameter has to be None or a list of values (labels) but not '" + str(
            paramDiscretizationMethod)) + "'."

    if variableName is None:
      self.defaultMethod = method
      self.defaultParamDiscretizationMethod = paramDiscretizationMethod
    else:
      self.discretizationParametersDictionary[variableName] = {}
      self.discretizationParametersDictionary[variableName]['method'] = method
      self.discretizationParametersDictionary[variableName]['param'] = paramDiscretizationMethod

  def audit(self, X, y=None):
    """
    Audits the passed values of X and y. Tells us which columns in X we think are already discrete and which need to
    be discretized, as well as the discretization algorithm that will be used to discretize them The parameters which
    are suggested will be used when creating the variables. To change this the user can manually set discretization
    parameters for each variable using the setDiscretizationParameters function.
   
    parameters:
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
    elif type(X) == pandas.core.series.Series:
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

    if len(possibleValues[d]) > 2:
      raise ValueError(
        "BNClassifier is a binary classifier! There are more than 2 possible values for y in the data provided")
    for i in range(d):
      variable = variableNames[i]
      auditDict[variable] = {}
      try:
        sklearn.utils.check_array(X[:, i], dtype='float', ensure_2d=False)
        isNumeric = True
      except ValueError:
        isNumeric = False
      if variable in self.discretizationParametersDictionary.keys():
        auditDict[variable] = self.discretizationParametersDictionary[variable]
        if self.discretizationParametersDictionary[variable]['method'] != "NoDiscretization" and not isNumeric:
          raise ValueError("The variable " + variable + " is not numeric and cannot be discretized!")

      else:
        if len(possibleValues[i]) > self.discretizationThreshold and isNumeric:
          auditDict[variable]['method'] = self.defaultMethod
          auditDict[variable]['nbBins'] = self.defaultParamDiscretizationMethod
        else:
          auditDict[variable]['method'] = 'NoDiscretization'
          auditDict[variable]['values'] = possibleValues[i]
      if auditDict[variable]['method'] == "NoDiscretization":
        auditDict[variable]['type'] = 'Discrete'
      else:
        auditDict[variable]['type'] = 'Continuous'

    return auditDict

  @staticmethod
  def discretizationElbowMethodRotation(discretizationStrategy, X):
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
        List[float]
          the edges of the bins the algorithm has chosen.
    """

    if discretizationStrategy not in {'kmeans', 'quantile', 'uniform'}:
      raise ValueError("cannot use elbow method with this type of discretization")
    variationArray = numpy.zeros(14)
    Xsorted = X[X.argsort(axis=None)]
    binEdgeMatrix = [[]] * 14
    for k in range(2, 16):
      discretizer = skp.KBinsDiscretizer(k, strategy=discretizationStrategy, subsample=None)
      discretizer.fit(Xsorted)
      binEdges = discretizer.bin_edges_[0]
      centresArray = (binEdges[1:] + binEdges[:-1]) / 2
      i = 0
      sumOfSquaredErrors = 0
      for x in Xsorted:
        if x > binEdges[i + 1]:
          i = i + 1
        sumOfSquaredErrors += (x - centresArray[i]) ** 2
      variationArray[k - 2] = sumOfSquaredErrors
      binEdgeMatrix[k - 2] = binEdges.to_list()

    # we calculate the slope of the line that connects the first and last point on our graph
    slope = (variationArray[13] - variationArray[0]) / 13

    # we calculate the slope of the line perpendicular to it
    otherSlope = -1 / slope

    # we change the coordinate system to the one with the two lines previously mentioned as its axes
    coordinateChangeMatrix = numpy.array([[1, slope], [1 / otherSlope, 1]])

    # we linearly transform the coordinates of every point in our curve
    transformedCoordinates = numpy.zeros((2, 14))
    for i in range(14):
      transformedCoordinates[:, i] = numpy.matmul(coordinateChangeMatrix,
                                                  numpy.array([[i], [variationArray[i] - variationArray[0]]])).reshape(
        2)

    # we search for the minimum in our newly obtained curve
    minkIndex = 0
    for k in range(14):
      if transformedCoordinates[1][minkIndex] > transformedCoordinates[1][k]:
        minkIndex = k
    # when we have found the minimum, we apply the inverse linear transformation to recover the optimal value of k
    minimumVector = numpy.matmul(numpy.linalg.inv(coordinateChangeMatrix),
                                 transformedCoordinates[:, minkIndex].reshape(2, 1))

    # we return the list of bin edges found using said optimal number of k
    return binEdgeMatrix[int(round(minimumVector[0]))]

  def discretizationMDLP(self, X, y, possibleValuesX, possibleValuesY):
    """
    Uses the MDLP algorithm described in Fayyad, 1995 to discretize the values of x.

    Parameters
    ----------
        X: ndarray with shape (n,1) where n is the number of samples
            Column-vector that contains all the data that needs to be discretized
        y: ndarray with shape (n,1) where n is the number of samples
            Column-vector that contains the class for each sample. This vector will not be discretized, but the class-value of each sample is needed to properly apply the algorithm
        possibleValuesX: one dimensional ndarray
            Contains all the possible values that x can take sorted in increasing order. There shouldn't be any doubles inside
        possibleValuesY: one dimensional ndarray
            Contains the possible values of y. There should be two possible values since this is a binary classifier
    Returns
    -------
        List[float]
         a list of the edges of the bins that are chosen by this algorithm
    """
    xAndY = numpy.concatenate((X, y), axis=1)
    xAndY = xAndY[xAndY[:, 0].argsort()]
    B = (possibleValuesX[1:] + possibleValuesX[:-1]) / 2
    [class0, _] = possibleValuesY

    binEdgesIndex = []
    nbElementsByIntervalClass0 = numpy.zeros(len(B) + 1)
    nbElementsByIntervalClass1 = numpy.zeros(len(B) + 1)
    currentIntervalIndex = 0
    for X in xAndY:
      if currentIntervalIndex < len(B) and X[0] > B[currentIntervalIndex]:
        currentIntervalIndex += 1
      if X[1] == class0:
        nbElementsByIntervalClass0[currentIntervalIndex] += 1
      else:
        nbElementsByIntervalClass1[currentIntervalIndex] += 1

    Class0ByLargeInterval = [nbElementsByIntervalClass0.sum()]
    Class1ByLargeInterval = [nbElementsByIntervalClass1.sum()]

    totalCountByLargeInterval = [Class0ByLargeInterval[0] + Class1ByLargeInterval[0]]

    probabilityClass0 = Class0ByLargeInterval[0] / totalCountByLargeInterval[0]
    probabilityClass1 = Class1ByLargeInterval[0] / totalCountByLargeInterval[0]
    shannonEntropyByLargeInterval = [
      (-1) * (probabilityClass0 * math.log2(probabilityClass0) + probabilityClass1 * math.log2(probabilityClass1))]

    continueDividingInterval = [True]

    currentValues = {}
    minimalValues = {}

    while any(continueDividingInterval):
      minimalValues['classInformationEntropy'] = math.inf
      for param in {'boundaryIndex', 'leftSubintervalClass0', 'leftSubintervalClass1', 'leftSubintervalShannonEntropy',
                    'rightSubintervalClass0', 'rightSubintervalClass1', 'rightSubintervalShannonEntropy'}:
        (currentValues[param], minimalValues[param]) = (0, 0)

      position = 0
      while currentValues['boundaryIndex'] < len(B):

        while not continueDividingInterval[position]:
          position = position + 1
          currentValues['boundaryIndex'] = binEdgesIndex[position - 1] + 1

        if position < len(binEdgesIndex) and currentValues['boundaryIndex'] == binEdgesIndex[position]:
          # this function decides whether to accept the cut point in this interval and updates the relevant lists if
          # the value is accepted.
          self._divideIntervalMDLP(minimalValues, shannonEntropyByLargeInterval, Class0ByLargeInterval,
                                   Class1ByLargeInterval, continueDividingInterval, totalCountByLargeInterval, position,
                                   binEdgesIndex)
          position += 1
          while position < len(continueDividingInterval) and not continueDividingInterval[position]:
            position += 1
          if position == len(continueDividingInterval):
            break
          else:
            currentValues['boundaryIndex'] = binEdgesIndex[position - 1] + 1
          (currentValues['leftSubintervalClass0'], currentValues['leftSubintervalClass1']) = (0, 0)
          minimalValues['classInformationEntropy'] = math.inf
          continue

        currentValues['leftSubintervalClass0'] += nbElementsByIntervalClass0[currentValues['boundaryIndex']]
        currentValues['leftSubintervalClass1'] += nbElementsByIntervalClass1[currentValues['boundaryIndex']]

        totalCountLeftInterval = currentValues['leftSubintervalClass0'] + currentValues['leftSubintervalClass1']

        probabilityClass0 = currentValues['leftSubintervalClass0'] / totalCountLeftInterval
        probabilityClass1 = currentValues['leftSubintervalClass1'] / totalCountLeftInterval
        currentValues['leftSubintervalShannonEntropy'] = 0
        if probabilityClass0 > 0:
          currentValues['leftSubintervalShannonEntropy'] += (-1) * probabilityClass0 * math.log2(probabilityClass0)
        if probabilityClass1 > 0:
          currentValues['leftSubintervalShannonEntropy'] += (-1) * probabilityClass1 * math.log2(probabilityClass1)

        classInformationEntropy = (totalCountLeftInterval / totalCountByLargeInterval[position]) * currentValues[
          'leftSubintervalShannonEntropy']

        currentValues['rightSubintervalClass0'] = Class0ByLargeInterval[position] - currentValues[
          'leftSubintervalClass0']
        currentValues['rightSubintervalClass1'] = Class1ByLargeInterval[position] - currentValues[
          'leftSubintervalClass1']
        NRightInterval = currentValues['rightSubintervalClass0'] + currentValues['rightSubintervalClass1']

        probabilityClass0 = currentValues['rightSubintervalClass0'] / NRightInterval
        probabilityClass1 = currentValues['rightSubintervalClass1'] / NRightInterval
        currentValues['rightSubintervalShannonEntropy'] = 0
        if probabilityClass0 > 0:
          currentValues['rightSubintervalShannonEntropy'] += (-1) * probabilityClass0 * math.log2(probabilityClass0)
        if probabilityClass1 > 0:
          currentValues['rightSubintervalShannonEntropy'] += (-1) * probabilityClass1 * math.log2(probabilityClass1)

        classInformationEntropy += (NRightInterval / totalCountByLargeInterval[position]) * currentValues[
          'rightSubintervalShannonEntropy']

        if classInformationEntropy < minimalValues['classInformationEntropy']:
          minimalValues = currentValues.copy()
          minimalValues['classInformationEntropy'] = classInformationEntropy

        currentValues['boundaryIndex'] += 1

      if continueDividingInterval[-1]:
        self._divideIntervalMDLP(minimalValues, shannonEntropyByLargeInterval, Class0ByLargeInterval,
                                 Class1ByLargeInterval, continueDividingInterval, totalCountByLargeInterval, position,
                                 binEdgesIndex)
    binEdges = [xAndY[0][0]]
    for index in binEdgesIndex:
      binEdges.append(B[index])
    binEdges.append(xAndY[-1][0])

    return binEdges

  @staticmethod
  def discretizationCAIM(X, y, possibleValuesX, possibleValuesY):
    """
    Applies the CAIM algorithm to discretize the values of x

    Parameters
    ----------
        X: ndarray with shape (n,1) where n is the number of samples
            Column-vector that contains all the data that needs to be discretized
        y: ndarray with shape (n,1) where n is the number of samples
            Column-vector that contains the class for each sample. This vector will not be discretized, but the class-value of each sample is needed to properly apply the algorithm
        possibleValuesX: one dimensional ndarray
            Contains all the possible values that x can take sorted in increasing order. There shouldn't be any doubles inside
        possibleValuesY: one dimensional ndarray
            Contains the possible values of y. There should be two possible values since this is a binary classifier
    Returns
    -------
        list[float]
          a list of the edges of the bins that are chosen by this algorithm
    """
    xAndY = numpy.concatenate((X, y), axis=1)
    xAndY = xAndY[xAndY[:, 0].argsort()]
    B = (possibleValuesX[1:] + possibleValuesX[:-1]) / 2
    [class0, class1] = possibleValuesY

    binEdgesIndex = []
    nbElementsByIntervalClass0 = numpy.zeros(len(B) + 1)
    nbElementsByIntervalClass1 = numpy.zeros(len(B) + 1)
    currentIntervalIndex = 0
    for X in xAndY:
      if currentIntervalIndex < len(B) and X[0] > B[currentIntervalIndex]:
        currentIntervalIndex += 1
      if X[1] == class0:
        nbElementsByIntervalClass0[currentIntervalIndex] += 1
      else:
        nbElementsByIntervalClass1[currentIntervalIndex] += 1

    Class0ByLargeInterval = [nbElementsByIntervalClass0.sum()]
    Class1ByLargeInterval = [nbElementsByIntervalClass1.sum()]

    k = 0
    globalCAIM = 0.0
    oldCaim = 0.0
    while True:
      caimMax = 0
      maxPosition = 0
      maxBoundaryIndex = 0
      position = 0
      currentSumClass0 = 0
      currentSumClass1 = 0
      maxLeftIntervalClass0 = currentSumClass0
      maxLeftIntervalClass1 = currentSumClass1
      maxRightIntervalClass0 = maxLeftIntervalClass0
      maxRightIntervalClass1 = maxLeftIntervalClass1

      for boundaryIndex in range(len(B)):
        if position < len(binEdgesIndex) and boundaryIndex == binEdgesIndex[position]:

          position += 1
          if Class0ByLargeInterval[position] > Class1ByLargeInterval[position]:
            oldCaim = globalCAIM * len(Class0ByLargeInterval) - math.pow(Class0ByLargeInterval[position], 2) / (
               Class0ByLargeInterval[position] + Class1ByLargeInterval[position])
          else:
            oldCaim = globalCAIM * len(Class0ByLargeInterval) - math.pow(Class1ByLargeInterval[position], 2) / (
               Class0ByLargeInterval[position] + Class1ByLargeInterval[position])
          currentSumClass0 = 0
          currentSumClass1 = 0
          continue

        currentSumClass0 += nbElementsByIntervalClass0[boundaryIndex]
        currentSumClass1 += nbElementsByIntervalClass1[boundaryIndex]
        caim = oldCaim

        if currentSumClass0 > currentSumClass1:
          caim = caim + math.pow(currentSumClass0, 2) / (currentSumClass0 + currentSumClass1)
        else:
          caim = caim + math.pow(currentSumClass1, 2) / (currentSumClass0 + currentSumClass1)

        intervalClass0 = Class0ByLargeInterval[position] - currentSumClass0
        intervalClass1 = Class1ByLargeInterval[position] - currentSumClass1

        if intervalClass0 > intervalClass1:
          caim = caim + math.pow(intervalClass0, 2) / (intervalClass0 + intervalClass1)
        else:
          caim = caim + math.pow(intervalClass1, 2) / (intervalClass0 + intervalClass1)

        caim = caim / (len(Class0ByLargeInterval) + 1)

        if caim > caimMax:
          maxLeftIntervalClass0 = currentSumClass0
          maxLeftIntervalClass1 = currentSumClass1
          maxRightIntervalClass0 = intervalClass0
          maxRightIntervalClass1 = intervalClass1
          caimMax = caim
          maxBoundaryIndex = boundaryIndex
          maxPosition = position

      if caimMax > globalCAIM:
        globalCAIM = caimMax
        binEdgesIndex.insert(maxPosition, maxBoundaryIndex)
        Class0ByLargeInterval.insert(maxPosition + 1, maxRightIntervalClass0)
        Class1ByLargeInterval.insert(maxPosition + 1, maxRightIntervalClass1)
        Class0ByLargeInterval[maxPosition] = maxLeftIntervalClass0
        Class1ByLargeInterval[maxPosition] = maxLeftIntervalClass1
        k = k + 1
        if Class0ByLargeInterval[0] > Class1ByLargeInterval[0]:
          oldCaim = globalCAIM * len(Class0ByLargeInterval) - math.pow(Class0ByLargeInterval[0], 2) / (
             Class0ByLargeInterval[0] + Class1ByLargeInterval[0])
        else:
          oldCaim = globalCAIM * len(Class0ByLargeInterval) - math.pow(Class1ByLargeInterval[0], 2) / (
             Class0ByLargeInterval[0] + Class1ByLargeInterval[0])

      else:
        break

    binEdges = [xAndY[0][0]]
    for index in binEdgesIndex:
      binEdges.append(B[index])
    binEdges.append(xAndY[-1][0])

    return binEdges

  @staticmethod
  def discretizationNML(X, possibleValuesX, kMax=10, epsilon=None):
    """
    Uses the discretization algorithm described in "MDL Histogram Density Estimator", Kontkaken and Myllymaki, 2007 to
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
            the value of epsilon used in the algorithm. Should be as small as possible. If None is passed the value is
            automatically calculated.
    Returns
    -------
        List[float]
          a list of the edges of the bins that are chosen by this algorithm
    """
    Xsorted = X[X.argsort(axis=None)]
    if epsilon is None:
      epsilon = numpy.amin(possibleValuesX[1:] - possibleValuesX[:-1]) / 2
    epsilon = epsilon / 2
    candidateCutPoints = numpy.zeros(2 * len(possibleValuesX))
    for i in range(len(possibleValuesX)):
      candidateCutPoints[2 * i] = possibleValuesX[i] - epsilon
      candidateCutPoints[2 * i + 1] = possibleValuesX[i] + epsilon

    E = len(candidateCutPoints)

    binCount = numpy.zeros(E)
    counter = 0
    j = 0
    for x in Xsorted:
      while x > candidateCutPoints[j]:
        binCount[j] = counter
        j = j + 1
      counter = counter + 1
    binCount[j] = counter
    n = binCount[-1]
    Rkminus1 = numpy.ones(E)
    Rk = numpy.zeros(E)
    for i in range(1, E):
      ne = int(binCount[i])
      total_amount = 0
      for h1 in range(ne + 1):
        h2 = ne - h1
        total_amount += math.pow(h1 / ne, h1) * math.pow(h2 / ne, h2) * scipy.special.comb(ne, h1)
      Rk[i] = total_amount

    k = 2

    Bkminus1 = numpy.zeros(E)
    for e in range(1, E):
      ne = binCount[e]
      Bkminus1[e] = -ne * (math.log(2 * epsilon * ne) - math.log(n * (candidateCutPoints[e] - candidateCutPoints[0])))

    Bk = numpy.zeros(E)
    cutpoints = [candidateCutPoints[0]]
    Bvalues = [Bkminus1[-1]]
    minimumeprime = 0
    while k <= kMax:

      for e in range(k, E):

        minimum = math.inf
        minimumeprime = 0
        for eprime in range(k - 1, e):
          if binCount[e] > binCount[eprime]:
            temp = Bkminus1[eprime] - (binCount[e] - binCount[eprime]) * (
               math.log(2 * epsilon * (binCount[e] - binCount[eprime])) - math.log(
              n * (candidateCutPoints[e] - candidateCutPoints[eprime])))
          else:
            temp = Bkminus1[eprime]
          temp = temp + math.log(Rk[e] / Rkminus1[eprime])
          if minimum > temp:
            minimum = temp
            minimumeprime = eprime
        minimum = minimum + math.log((E - k) / (k - 1))
        Bk[e] = minimum
      cutpoints.append(candidateCutPoints[minimumeprime])
      Bvalues.append(Bk[-1])

      k = k + 1
      temp = Rk
      Rk = Rk + numpy.multiply(binCount, Rkminus1) / (k - 2)
      Rkminus1 = temp
      Bkminus1 = Bk
    minimum = math.inf
    minimumIndex = 0
    for k in range(1, len(Bvalues)):
      if Bvalues[k] < minimum:
        minimum = Bvalues[k]
        minimumIndex = k
    cutpoints = sorted(set(cutpoints[:minimumIndex + 1]))
    cutpoints.append(candidateCutPoints[-1])

    return cutpoints

  def createVariable(self, variableName, X, y=None, possibleValuesY=None):
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
      Xtransformed = sklearn.utils.check_array(X, dtype='float', ensure_2d=False)
      isNumeric = True
    except ValueError:
      Xtransformed = X
      isNumeric = False

    foundValuesX = set(numpy.unique(X))
    n = len(X)

    if variableName not in self.discretizationParametersDictionary:  # The user has not manually set the discretization parameters for this variable
      if isNumeric and 1 <= self.discretizationThreshold < len(foundValuesX) or (
         self.discretizationThreshold < 1 and len(foundValuesX) / len(X) > self.discretizationThreshold):
        self.discretizationParametersDictionary[variableName] = {}
        self.discretizationParametersDictionary[variableName]['method'] = self.defaultMethod
        self.discretizationParametersDictionary[variableName]['param'] = self.defaultParamDiscretizationMethod
      else:
        self.discretizationParametersDictionary[variableName] = {}
        self.discretizationParametersDictionary[variableName]['method'] = "NoDiscretization"
      usingDefaultParameters = True
    else:
      usingDefaultParameters = False
      if self.discretizationParametersDictionary[variableName]['method'] != "NoDiscretization" and not isNumeric:
        raise ValueError("The variable " + variableName + " is not numeric and cannot be discretized!")

    if self.discretizationParametersDictionary[variableName]["method"] == "NoDiscretization":
      is_int_var = True
      min_v = max_v = None

      possibleValuesX = None
      if "param" in self.discretizationParametersDictionary[variableName]:
        possibleValuesX = self.discretizationParametersDictionary[variableName]["param"]

      if possibleValuesX is None:
        possibleValuesX = foundValuesX
      else:
        # foundValuesX must be in possibleValuesX
        if not foundValuesX.issubset(possibleValuesX):
          raise ValueError(
            f"The values passed in possibleValues ({possibleValuesX}) do not match database values ({foundValuesX})")

      for value in possibleValuesX:
        if checkInt(value):
          v = int(value)
          if min_v is None or min_v > v:
            min_v = v
          if max_v is None or max_v < v:
            max_v = v
        else:
          is_int_var = False
          break

      if is_int_var:
        if len(possibleValuesX) == max_v - min_v + 1:  # no hole in the list of int
          var = gum.RangeVariable(variableName, variableName, min_v, max_v)
        else:
          var = gum.IntegerVariable(variableName, variableName, [int(v) for v in possibleValuesX])
      else:
        is_float_var = True
        for value in possibleValuesX:
          if not checkFloat(value):
            is_float_var = False
            break

        if is_float_var:
          var = gum.NumericalDiscreteVariable(variableName, variableName, [float(v) for v in possibleValuesX])
        else:
          var = gum.LabelizedVariable(variableName, variableName, sorted([str(v) for v in possibleValuesX]))
    else:
      self.numberOfContinuous += 1
      if self.discretizationParametersDictionary[variableName]['method'] == "CAIM":
        if y is None:
          raise ValueError(
            "The CAIM discretization method requires a list of the associated classes for each data vector since it "
            "is a supervised discretization method. You should pass it as y.")
        if possibleValuesY is None:
          possibleValuesY = numpy.unique(y)
        binEdges = self.discretizationCAIM(Xtransformed.reshape(n, 1), y.reshape(n, 1), numpy.unique(Xtransformed),
                                           possibleValuesY)
      elif self.discretizationParametersDictionary[variableName]['method'] == "MDLP":
        if y is None:
          raise ValueError(
            "The MDLP discretization method requires a list of the associated classes for each data vector since it "
            "is a supervised discretization method. You should pass it as y.")
        if possibleValuesY is None:
          possibleValuesY = numpy.unique(y)
        binEdges = self.discretizationMDLP(Xtransformed.reshape(n, 1), y.reshape(n, 1), numpy.unique(Xtransformed),
                                           possibleValuesY)
      elif self.discretizationParametersDictionary[variableName]['method'] == "NML":
        binEdges = self.discretizationNML(Xtransformed.flatten(), numpy.unique(Xtransformed),
                                          kMax=self.discretizationParametersDictionary[variableName]["param"])
      else:
        if self.discretizationParametersDictionary[variableName]['param'] == 'elbowMethod':
          binEdges = self.discretizationElbowMethodRotation(
            self.discretizationParametersDictionary[variableName]['method'], Xtransformed.flatten())
        else:
          discre = skp.KBinsDiscretizer(self.discretizationParametersDictionary[variableName]['param'],
                                        strategy=self.discretizationParametersDictionary[variableName]['method'],
                                        subsample=None)
          discre.fit(X.reshape(-1, 1))
          binEdges = discre.bin_edges_[0].tolist()

      if len(binEdges) == 2:
        raise ValueError("Due to an error the discretization method " + str(
          self.discretizationParametersDictionary[variableName]['method']) + " using " + str(
          self.discretizationParametersDictionary[variableName]['param']) + " bins for the variable " + str(
          variableName) + "gave only 1 bin. Try increasing the number of bins used by this variable using "
                          "setDiscretizationParameters to avoid this error")

      # we replace infinity as min and max by the new empirical flag.
      # binEdges[0] = -math.inf
      # binEdges[-1] = math.inf
      self.totalNumberOfBins += len(binEdges) - 1
      var = gum.DiscretizedVariable(variableName, variableName, binEdges)
      var.setEmpirical(True)

    if usingDefaultParameters:
      self.discretizationParametersDictionary.pop(variableName)

    return var

  @staticmethod
  def _divideIntervalMDLP(minimalValues, shannonEntropyByLargeInterval, Class0ByLargeInterval,
                          Class1ByLargeInterval, continueDividingInterval, totalCountByLargeInterval, position,
                          binEdgesIndex):
    shannonEntropy = shannonEntropyByLargeInterval[position]

    gain = shannonEntropy - minimalValues['classInformationEntropy']

    # all the 2's here should be replaced by the number of classes present in the interval. However we know that if
    # the number of classes in the interval is equal to 1, then the shannon entropy will be 0 so the product of the 2
    # will be 0.
    deltaS = math.log2(7) - (
       2 * shannonEntropy - 2 * minimalValues['leftSubintervalShannonEntropy']
       - 2 * minimalValues['rightSubintervalShannonEntropy'])

    if gain > (math.log2(totalCountByLargeInterval[position] - 1) + deltaS) / totalCountByLargeInterval[
      position] or len(Class0ByLargeInterval) == 1:
      binEdgesIndex.insert(position, minimalValues['boundaryIndex'])

      Class0ByLargeInterval.insert(position + 1, minimalValues['rightSubintervalClass0'])
      Class1ByLargeInterval.insert(position + 1, minimalValues['rightSubintervalClass1'])
      continueDividingInterval.insert(position + 1, True)
      totalCountByLargeInterval.insert(position + 1, minimalValues['rightSubintervalClass0'] + minimalValues[
        'rightSubintervalClass1'])
      shannonEntropyByLargeInterval.insert(position + 1, minimalValues['rightSubintervalShannonEntropy'])

      Class0ByLargeInterval[position] = minimalValues['leftSubintervalClass0']
      Class1ByLargeInterval[position] = minimalValues['leftSubintervalClass1']
      totalCountByLargeInterval[position] = minimalValues['leftSubintervalClass0'] + minimalValues[
        'leftSubintervalClass1']
      shannonEntropyByLargeInterval[position] = minimalValues['leftSubintervalShannonEntropy']

      # if the class information entropy is 0, then we have perfectly cut the interval so that a class only appears
      # on one side, so we do not need to cut any further.
      if minimalValues['leftSubintervalShannonEntropy'] == 0:
        continueDividingInterval[position] = False
      if minimalValues['rightSubintervalShannonEntropy'] == 0:
        continueDividingInterval[position + 1] = False

        # if there are no potential boundary points left in this interval, we can't divide it any further
      if position > 0 and minimalValues['boundaryIndex'] - 1 == binEdgesIndex[position - 1]:
        continueDividingInterval[position] = False

      if minimalValues['boundaryIndex'] == 0:
        continueDividingInterval[position] = False

      if position < len(binEdgesIndex) - 1 and binEdgesIndex[position] + 1 == binEdgesIndex[position + 1]:
        continueDividingInterval[position + 1] = False
      position += 1
    else:
      continueDividingInterval[position] = False

  def discretizedBN(self, X, y=None, possibleValuesY=None):
    """
    return a BN discretized using the suggestion of the Discretized for date source X and for target y. This BN only contains the discretized variables. For instance, it can be used as a template for a BNLearner.

    Example
    -------
    >>> discretizer=skbn.BNDiscretizer(defaultDiscretizationMethod='uniform',defaultParamDiscretizationMethod=7,discretizationThreshold=10)
    >>> learner=gum.BNLearner(data,discretizer.discretizedBN(data))
    """
    template = gum.BayesNet()
    for name in X:
      template.add(self.createVariable(name, X[name], y, possibleValuesY))
    return template
