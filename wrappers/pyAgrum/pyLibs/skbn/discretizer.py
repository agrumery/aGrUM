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

import numpy
import math
import pandas
import scipy
import sklearn
import sklearn.preprocessing as skp
import pyAgrum as gum


class BNDiscretizer():
  """
  Represents a tool to discretize some variables in a database in order to obtain  a way to learn a pyAgrum's (discrete)
  Bayesian networks.
  
  parameters:
      defaultDiscretizationMethod: str
            sets the default method of discretization for this discretizer. Possible values are: 'quantile', 'uniform',
            'kmeans', 'NML', 'CAIM' and 'MDLP'. This method will be used if the user has not specified another method
            for that specific variable using the setDiscretizationParameters method.
      defaultNumberOfBins: str or int
          sets the number of bins if the method used is quantile, kmeans, uniform. In this case this parameter can also
          be set to the string 'elbowMethod' so that the best number of bins is found automatically.
          If the method used is NML, this parameter sets the the maximum number of bins up to which the NML algorithm
          searches for the optimal number of bins. In this case this parameter must be an int
          If any other discetization method is used, this parameter is ignored.
      discretizationThreshold: int or float
          When using default parameters a variable will be treated as continous only if it has more unique values than
          this number (if the number is an int greater than 1).
          If the number is a float between 0 and 1, we will test if the proportion of unique values is bigger than this
          number. For example if you have entered 0.95, the variable will be treated as continous only if more than 95%
          of its values are unique.

  """

  def __init__(self, defaultDiscretizationMethod='quantile', defaultNumberOfBins=10, discretizationThreshold=25):
    """
    parameters:
        defaultDiscretizationMethod: str
            sets the default method of discretization for this discretizer. Possible values are: 'quantile', 'uniform',
            'kmeans', 'NML', 'CAIM' and 'MDLP'. This method will be used if the user has not specified another method
            for that specific variable using the setDiscretizationParameters method.
        defaultNumberOfBins: str or int
            sets the number of bins if the method used is quantile, kmeans, uniform. In this case this parameter can
            also be set to the string 'elbowMethod' so that the best number of bins is found automatically.
            If the method used is NML, this parameter sets the the maximum number of bins up to which the NML algorithm
            searches for the optimal number of bins. In this case this parameter must be an int
            If any other discetization method is used, this parameter is ignored.
        discretizationThreshold: int or float
            When using default parameters a variable will be treated as continous only if it has more unique values than
            this number (if the number is an int greater than 1).
            If the number is a float between 0 and 1, we will test if the proportion of unique values is bigger than
            this number. For example if you have entered 0.95, the variable will be treated as continous only if more
            than 95% of its values are unique.

    Creates the BNDiscretizer
    """
    self.discretizationParametersDictionary = dict()
    self.numberOfContinous = 0
    self.totalNumberOfBins = 0
    self.defaultMethod = None
    self.defaultNbBins = None
    self.setDiscretizationParameters(None, defaultDiscretizationMethod, defaultNumberOfBins)

    self.discretizationThreshold = discretizationThreshold

  def clear(self, clearDiscretizationParameters=False):
    """
    parameters:
        clearDiscretizationParamaters: bool
            if True, this method also clears the parameters the user has set for each variable and resets them to the default.

    returns:
        void

    Sets the number of continous variables and the total number of bins created by this discretizer to 0. If
    clearDiscretizationParameters is True, also clears the the parameters for discretization the user has set for each
    variable.
    """
    self.numberOfContinous = 0
    self.totalNumberOfBins = 0
    if clearDiscretizationParameters:
      self.discretizationParametersDictionary = dict()

  def setDiscretizationParameters(self, variableName=None, methode=None, numberOfBins=None):
    """
    parameters:
        variableName: str
            the name of the variable you want to set the discretization paramaters of. Set to None to set the new
            default for this BNClassifier.
        methode: str
            The method of discretization used for this variable. Type "None" if you do not want to discretize this
            variable. Possible values are: 'None', 'quantile', 'uniform', 'kmeans', 'NML', 'CAIM' and 'MDLP'
        numberOfBins:
            sets the number of bins if the method used is quantile, kmeans, uniform. In this case this parameter can also
            be set to the string 'elbowMethod' so that the best number of bins is found automatically.
            if the method used is NML, this parameter sets the the maximum number of bins up to which the NML algorithm
            searches for the optimal number of bins. In this case this parameter must be an int
            If any other discetization method is used, this parameter is ignored.

    returns:
        void
    """
    if variableName in self.discretizationParametersDictionary.keys():
      oldNbBins = self.discretizationParametersDictionary[variableName]['k']
      oldMethod = self.discretizationParametersDictionary[variableName]['methode']
    else:
      oldNbBins = self.defaultNbBins
      oldMethod = self.defaultMethod

    if methode is None:
      methode = oldMethod

    if numberOfBins is None:
      if methode != 'None':
        numberOfBins = oldNbBins

    if methode not in {'kmeans', 'uniform', 'quantile', 'NML', 'MDLP', 'CAIM', 'None'}:
      raise ValueError(
        "This discretization method is not recognized! Possible values are keans, uniform, quantile, NML, "
        "CAIM and MDLP. You have entered " + str(
          methode))

    if numberOfBins == 'elbowMethod':
      if methode == "NML":
        raise ValueError(
          "The elbow Method cannot be used as the number of bins for the algorithm NML. Please an integer value")
    elif methode != 'None':
      try:
        numberOfBins = int(numberOfBins)
      except:
        raise ValueError(
          "The possible values for numberOfBins are any integer or the string 'elbowMethod'. You have entered: " + str(
            numberOfBins))
    if variableName is None:
      self.defaultMethod = methode
      self.defaultNbBins = numberOfBins
    else:
      self.discretizationParametersDictionary[variableName] = dict()
      self.discretizationParametersDictionary[variableName]['k'] = numberOfBins
      self.discretizationParametersDictionary[variableName]['methode'] = methode

  def audit(self, X, y=None):
    """
    parameters:
        X: {array-like, sparse matrix} of shape (n_samples, n_features)
            training data
        y: array-like of shape (n_samples,)
            Target values
    returns:
        auditDict: dict()

    Audits the passed values of X and y. Tells us which columns in X we think are already discrete and which need to
    be discretized, as well as the discretization algorithm that will be used to discretize them The parameters which
    are suggested will be used when creating the variables. To change this the user can manually set discretization
    parameters for each variable using the setDiscretizationParameters function.
    """

    auditDict = dict()

    if type(X) == pandas.DataFrame:
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

    possibleValues = dict()  # pour la ligne on compte les valeurs possibles

    for i in range(d):
      possibleValues[i] = numpy.unique(X[:, i])
    possibleValues[d] = numpy.unique(y)

    if len(possibleValues[d]) > 2:
      raise ValueError(
        "BNClassifier is a binary classifier! There are more than 2 possible values for y in the data provided")
    for i in range(d):
      variable = variableNames[i]
      auditDict[variable] = dict()
      try:
        sklearn.utils.check_array(X[:, i], dtype='float', ensure_2d=False)
        isNumeric = True
      except ValueError:
        isNumeric = False
      if variable in self.discretizationParametersDictionary.keys():
        auditDict[variable] = self.discretizationParametersDictionary[variable]
        if self.discretizationParametersDictionary[variable]['methode'] != "None" and not isNumeric:
          raise ValueError("The variable " + variable + " is not numeric and cannot be discretized!")

      else:
        if len(possibleValues[i]) > self.discretizationThreshold and isNumeric:
          auditDict[variable]['k'] = self.defaultNbBins
          auditDict[variable]['methode'] = self.defaultMethod
        else:
          auditDict[variable]['methode'] = 'None'
          auditDict[variable]['k'] = None
      if auditDict[variable]['methode'] == "None":
        auditDict[variable]['type'] = 'Discrete'
      else:
        auditDict[variable]['type'] = 'Continuous'

    return auditDict

  def discretizationElbowMethodRotation(self, discretizationStrategy, X):
    """
    parameters:
        discretizationStrategy: str
            The method of discretization that will be used. Possible values are: 'quantile' , 'kmeans' and 'uniform'
        X: one dimensional ndarray
            Contains the data that should be discretized
    returns:
        binEdges: the edges of the bins the algorithm has chosen.

    Calculates the sum of squared errors as a function of the number of clusters using the discretization strategy
    that is passed as a parameter. Returns the bins that are optimal for minimizing the variation and the number of
    bins at the same time. Uses the elbow method to find this optimal point. To find the "elbow" we rotate the curve
    and look for its minimum.
    """

    if discretizationStrategy not in {'kmeans', 'quantile', 'uniform'}:
      raise ValueError("cannot use elbow method with this type of discretization")
    variationArray = numpy.zeros(14)
    Xsorted = X[X.argsort(axis=None)]
    binEdgeMatrix = [[]] * 14
    for k in range(2, 16):
      discretizer = skp.KBinsDiscretizer(k, strategy=discretizationStrategy)
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

    # we caclulate the slope of the line that connects the first and last point on our graph
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

  def discretizationMDLP(self, x, y, possibleValuesX, possibleValuesY):
    """
    parametres:
        x: ndarray with shape (n,1) where n is the number of samples
            Column-vector that contains all the data that needs to be discretized
        y: ndarray with shape (n,1) where n is the number of samples
            Column-vector that contains the class for each sample. This vector will not be discretized, but the class-value of each sample is needed to properly apply the algorithm
        possibleValuesX: one dimensional ndarray
            Contains all the possible values that x can take sorted in increasing order. There shouldn't be any doubles inside
        possibleValuesY: one dimensional ndarray
            Contains the possible values of y. There should be two possible values since this is a binary classifier
    returns:
        binEdges: a list of the edges of the bins that are chosen by this algorithm

    Uses the MDLP algorithm described in Fayyad, 1995 to discretize the values of x.
    """
    xAndY = numpy.concatenate((x, y), axis=1)
    xAndY = xAndY[xAndY[:, 0].argsort()]
    B = (possibleValuesX[1:] + possibleValuesX[:-1]) / 2
    [class0, class1] = possibleValuesY

    binEdgesIndex = []
    nbElementsByIntervalClass0 = numpy.zeros(len(B) + 1)
    nbElementsByIntervalClass1 = numpy.zeros(len(B) + 1)
    currentIntervalIndex = 0
    for x in xAndY:
      if currentIntervalIndex < len(B) and x[0] > B[currentIntervalIndex]:
        currentIntervalIndex += 1
      if x[1] == class0:
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

    currentValues = dict()
    minimalValues = dict()

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
          # this function decides whether to accept the cutpoint in this interval and updates the relevant lists if
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

  def discretizationCAIM(self, x, y, possibleValuesX, possibleValuesY):
    """
    parametres:
        x: ndarray with shape (n,1) where n is the number of samples
            Column-vector that contains all the data that needs to be discretized
        y: ndarray with shape (n,1) where n is the number of samples
            Column-vector that contains the class for each sample. This vector will not be discretized, but the class-value of each sample is needed to properly apply the algorithm
        possibleValuesX: one dimensional ndarray
            Contains all the possible values that x can take sorted in increasing order. There shouldn't be any doubles inside
        possibleValuesY: one dimensional ndarray
            Contains the possible values of y. There should be two possible values since this is a binary classifier
    returns:
        binEdges: a list of the edges of the bins that are chosen by this algorithm

    Applies the CAIM algorithm to discretize the values of x
    """
    xAndY = numpy.concatenate((x, y), axis=1)
    xAndY = xAndY[xAndY[:, 0].argsort()]
    B = (possibleValuesX[1:] + possibleValuesX[:-1]) / 2
    [class0, class1] = possibleValuesY

    binEdgesIndex = []
    nbElementsByIntervalClass0 = numpy.zeros(len(B) + 1)
    nbElementsByIntervalClass1 = numpy.zeros(len(B) + 1)
    currentIntervalIndex = 0
    for x in xAndY:
      if currentIntervalIndex < len(B) and x[0] > B[currentIntervalIndex]:
        currentIntervalIndex += 1
      if x[1] == class0:
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
      currentsumClass0 = 0
      currentsumClass1 = 0

      for boundaryIndex in range(len(B)):
        if position < len(binEdgesIndex) and boundaryIndex == binEdgesIndex[position]:

          position += 1
          if Class0ByLargeInterval[position] > Class1ByLargeInterval[position]:
            oldCaim = globalCAIM * len(Class0ByLargeInterval) - math.pow(Class0ByLargeInterval[position], 2) / (
                Class0ByLargeInterval[position] + Class1ByLargeInterval[position])
          else:
            oldCaim = globalCAIM * len(Class0ByLargeInterval) - math.pow(Class1ByLargeInterval[position], 2) / (
                Class0ByLargeInterval[position] + Class1ByLargeInterval[position])
          currentsumClass0 = 0
          currentsumClass1 = 0
          continue

        currentsumClass0 += nbElementsByIntervalClass0[boundaryIndex]
        currentsumClass1 += nbElementsByIntervalClass1[boundaryIndex]
        caim = oldCaim

        if currentsumClass0 > currentsumClass1:
          caim = caim + math.pow(currentsumClass0, 2) / (currentsumClass0 + currentsumClass1)
        else:
          caim = caim + math.pow(currentsumClass1, 2) / (currentsumClass0 + currentsumClass1)

        intervalclass0 = Class0ByLargeInterval[position] - currentsumClass0
        intervalclass1 = Class1ByLargeInterval[position] - currentsumClass1

        if intervalclass0 > intervalclass1:
          caim = caim + math.pow(intervalclass0, 2) / (intervalclass0 + intervalclass1)
        else:
          caim = caim + math.pow(intervalclass1, 2) / (intervalclass0 + intervalclass1)

        caim = caim / (len(Class0ByLargeInterval) + 1)

        if caim > caimMax:
          maxLeftIntervalClass0 = currentsumClass0
          maxLeftIntervalClass1 = currentsumClass1
          maxRightIntervalClass0 = intervalclass0
          maxRightIntervalClass1 = intervalclass1
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

  def discretizationNML(self, X, possibleValuesX, kMax=10, epsilon=None):
    """
    parameters:
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
    returns:
        binEdges: a list of the edges of the bins that are chosen by this algorithm

    Uses the disceretization algorithm described in "MDL Histogram Density Estimator", Kontkaken and Myllymaki, 2007 to
    discretize.
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
      sum = 0
      for h1 in range(ne + 1):
        h2 = ne - h1
        sum += math.pow(h1 / ne, h1) * math.pow(h2 / ne, h2) * scipy.special.comb(ne, h1)
      Rk[i] = sum

    k = 2

    Bkminus1 = numpy.zeros(E)
    for e in range(1, E):
      ne = binCount[e]
      Bkminus1[e] = -ne * (math.log(2 * epsilon * ne) - math.log(n * ((candidateCutPoints[e] - candidateCutPoints[0]))))

    Bk = numpy.zeros(E)
    cutpoints = [candidateCutPoints[0]]
    Bvalues = [Bkminus1[-1]]
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
    parameters:
        variableName:
            the name of the created variable
        X: ndarray shape(n,1)
            A column vector containing n samples of a feature. The column for which the variable will be created
        y: ndarray shape(n,1)
            A column vector containing the corresponding for each element in X.
        possibleValuesX: onedimensional ndarray
            An ndarray containing all the unique values of X
        possibleValuesY: onedimensional ndarray
            An ndarray containing all the unique values of y
        returnModifiedX: bool
            X could be modified by this function during
    returns:
        var: pyagrum.DiscreteVariable
            the created variable

    Creates a variable for the column passed in as a parameter and places it in the Bayesian network
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
    possibleValuesX = numpy.unique(X)
    n = len(X)

    if variableName not in self.discretizationParametersDictionary.keys():  # The user has not manually set the discretization parameters for this variable

      if isNumeric and \
          ((self.discretizationThreshold >= 1 and len(possibleValuesX) > self.discretizationThreshold)
           or (self.discretizationThreshold < 1 and len(possibleValuesX) / len(X) > self.discretizationThreshold)):
        self.discretizationParametersDictionary[variableName] = dict()
        self.discretizationParametersDictionary[variableName]['methode'] = self.defaultMethod
        self.discretizationParametersDictionary[variableName]['k'] = self.defaultNbBins
      else:
        self.discretizationParametersDictionary[variableName] = dict()
        self.discretizationParametersDictionary[variableName]['methode'] = "None"
      usingDefaultParameters = True
    else:
      usingDefaultParameters = False
      if self.discretizationParametersDictionary[variableName]['methode'] != "None" and not isNumeric:
        raise ValueError("The variable " + variableName + " is not numeric and cannot be discretized!")

    if self.discretizationParametersDictionary[variableName]["methode"] == "None":
      var = gum.LabelizedVariable(variableName, variableName, 0)
      for value in possibleValuesX:
        var.addLabel(str(value))

    else:
      self.numberOfContinous += 1
      if self.discretizationParametersDictionary[variableName]['methode'] == "CAIM":
        if y is None:
          raise ValueError(
            "The CAIM discretization method requires a list of the associated classes for each data vector since it "
            "is a supervised discretization method. You should pass it as y.")
        if possibleValuesY is None:
          possibleValuesY = numpy.unique(y)
        binEdges = self.discretizationCAIM(Xtransformed.reshape(n, 1), y.reshape(n, 1), numpy.unique(Xtransformed),
                                           possibleValuesY)
      elif self.discretizationParametersDictionary[variableName]['methode'] == "MDLP":
        if y is None:
          raise ValueError(
            "The MDLP discretization method requires a list of the associated classes for each data vector since it "
            "is a supervised discretization method. You should pass it as y.")
        if possibleValuesY is None:
          possibleValuesY = numpy.unique(y)
        binEdges = self.discretizationMDLP(Xtransformed.reshape(n, 1), y.reshape(n, 1), numpy.unique(Xtransformed),
                                           possibleValuesY)
      elif self.discretizationParametersDictionary[variableName]['methode'] == "NML":
        binEdges = self.discretizationNML(Xtransformed.flatten(), numpy.unique(Xtransformed),
                                          kMax=self.discretizationParametersDictionary[variableName]["k"])
      else:
        if self.discretizationParametersDictionary[variableName]['k'] == 'elbowMethod':
          binEdges = self.discretizationElbowMethodRotation(
            self.discretizationParametersDictionary[variableName]['methode'], Xtransformed.flatten())
        else:
          discre = skp.KBinsDiscretizer(self.discretizationParametersDictionary[variableName]['k'],
                                        strategy=self.discretizationParametersDictionary[variableName]['methode'])
          discre.fit(X.reshape(-1, 1))
          binEdges = discre.bin_edges_[0].tolist()

      if len(binEdges) == 2:
        raise ValueError("Due to an error the discretization method " + str(
          self.discretizationParametersDictionary[variableName]['methode']) + " using " + str(
          self.discretizationParametersDictionary[variableName]['k']) + " bins for the variable " + str(
          variableName) + "gave only 1 bin. Try increasing the number of bins used by this variable using "
                          "setDiscetizationParameters to avoid this error")
      binEdges[0] = -math.inf
      binEdges[-1] = math.inf
      self.totalNumberOfBins += len(binEdges) - 1
      var = gum.DiscretizedVariable(variableName, variableName, binEdges)

    if usingDefaultParameters:
      self.discretizationParametersDictionary.pop(variableName)

    return var

  def _divideIntervalMDLP(self, minimalValues, shannonEntropyByLargeInterval, Class0ByLargeInterval,
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
