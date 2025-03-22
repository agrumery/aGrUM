############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
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
#   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   #
#   licence (LICENSE.MIT) for more details.                                #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

import pandas
from numpy.random import dirichlet

import pyagrum
import pyagrum.bnmixture as BNM

from typing import List, Optional


class IMixtureLearner:
  def __init__(self):
    self._ref_learner = None
    raise NotImplementedError("Learner interface should not be initialized !")

  def updateState(self, learner: pyagrum.BNLearner, **kargs):
    """
    Updates a learner using methods in parameters. If there are no parameters given, ``learner`` will copy state of the reference learner, if it exists.

    Parameters
    ----------
    learner : pyagrum.BNLearner
        Learner to update.
    algorithm : str
        Algorithm to use.
    order : List[str or int]
        Order for K2 algorithm.
    tabu_size : int
        size for local search with tabu list.
    nb_decrease : int
        decrease for local search with tabu list.
    score : str
        Type of score to use.
    correction : str
        Correction to use.
    prior : str
        Prior to use.
    source : str | pyagrum.BayesNet
        Source for dirichlet prior
    prior_weight : float
        Weight used for prior.
    """
    if len(kargs) == 0 and self._ref_learner is not None:
      learner.copyState(self._ref_learner)
    else:
      zemethod = kargs.get('algorithm', 'Greedy Hill Climbing')
      if zemethod == 'Greedy Hill Climbing':
        learner.useGreedyHillClimbing()
      elif zemethod == 'MIIC':
        learner.useMIIC()
      elif zemethod == 'K2':
        if kargs.get('order', None) is None:
          raise pyagrum.ArgumentError("Tried to use K2 without an order")
        learner.useK2(kargs['order'])
      elif zemethod == 'Local Search with Tabu List':
        learner.useLocalSearchWithTabuList(kargs.get('tabu_size', 100), kargs.get('nb_decrease', 2))
      else:
        raise pyagrum.ArgumentError(f"could not find the algorithm {kargs['algorithm']}")

      zescore = kargs.get('score', 'BDeu')
      if zescore == 'K2':
        learner.useScoreK2()
      elif zescore == 'AIC':
        learner.useScoreAIC()
      elif zescore == 'BIC':
        learner.useScoreBIC()
      elif zescore == 'BD':
        learner.useScoreBD()
      elif zescore == 'BDeu':
        learner.useScoreBDeu()
      elif zescore == 'Log2Likelihood':
        learner.useScoreLog2Likelihood()
      else:
        raise pyagrum.ArgumentError(f"could not find a suitable score : {kargs['score']}")

      zecorrec = kargs.get('correction', 'MDL')
      if zecorrec == 'MDL':
        learner.useMDLCorrection()
      elif zecorrec == 'NML':
        learner.useNMLCorrection()
      elif zecorrec == 'No correction':
        learner.useNoCorrection()
      else:
        raise pyagrum.ArgumentError(f"could not find a suitable correction : {kargs['correction']}")

      zeprior = kargs.get('prior', '-')
      if zeprior == '-':
        learner.useNoPrior()
      elif zeprior == 'Dirichlet':
        if kargs.get('source', None) is None:
          raise pyagrum.ArgumentError("could not find source for dirichlet prior")
        if kargs.get('prior_weight', None) is not None:
          learner.useDirichletPrior(kargs['source'], kargs['prior_weight'])
        else:
          learner.useDirichletPrior(kargs['source'])
      elif zeprior == 'BDEU':
        if kargs.get('prior_weight', None) is not None:
          learner.useBDeuPrior(kargs['prior_weight'])
        else:
          learner.useBDeuPrior()
      elif zeprior == 'Smoothing':
        if kargs.get('prior_weight', None) is not None:
          learner.useSmoothingPrior(kargs['prior_weight'])
        else:
          learner.useSmoothingPrior()


class BNMLearner(IMixtureLearner):
  """
  Allows to learn mutiple BNs from a database. Learned BNs are given a weight and are stored in a BNMixture.

  Notes
  -----
  - How is memory handled? First, to reduce memory consumption, only one BNLearner is instancied at most at a given time. Their are more improvements if ``source`` contains files.
  In that case, dataframes are loaded one at a time to reduce memory consumption. Otherwise all DataFrames are stored together.
  - We create a reference learner using the source with maximum weight.

  Parameters
  ----------
  weights : List[float]
      Weights of each sample of the database.
  source : List[str] | List[pandas.DataFrame]
      Samples to learn from (csv format for now).
  states : List["state"]
      List of learners state.
  template : pyagrum.BayesNet | Optional
          BN to use to find modalities.

  Raises
  ------
  pyagrum.ArgumentError
      If arguments don't have the same dimensions.
  """

  def __init__(self, weights, source, template=None, states=None):
    if len(source) != len(weights):
      raise pyagrum.ArgumentError(
        f"weights and source don't have the same dimensions : weights({len(weights)}) and source({len(source)})")
    self._data = source.copy()
    self._weights = weights.copy()
    self._template = template
    self._states = states.copy() if states else [{}] * len(weights)
    self._ref_learner = None

  def add(self, source, weight, **kargs):
    """
    Adds a new BNLearner(its parameters) to the learner.
    """
    self._data.append(source)
    self._weights.append(weight)
    self._states.append(kargs)

  def learnBNM(self):
    """
    Learns the BNs from the database and return them stored in a BNMixture with corresponding weights.

    Returns
    -------
    BNM.BNMixture
        The learned BNMixture.
    """
    # finding heaviest source
    index, max_w = max(enumerate(self._weights), key=lambda x: x[1])
    if isinstance(self._data[index], str):
      datatmp = pandas.read_csv(self._data[index])
    elif isinstance(self._data[index], pandas.DataFrame):
      datatmp = self._data[index]

      # setting reference learner
    if self._template is None:
      self._ref_learner = pyagrum.BNLearner(datatmp)
    else:
      self._ref_learner = pyagrum.BNLearner(datatmp, self._template)
    self.updateState(self._ref_learner, **self._states[index])

    # create Mixture and learn reference BN
    bnm = BNM.BNMixture()
    refbn = self._ref_learner.learnBN()
    refname = "bn0"
    refbn.setProperty("name", refname)
    bnm.add(refname, refbn, w=max_w)

    # adding others BNs
    for i in range(len(self._data)):
      if i == index: continue

      # loading data
      if isinstance(self._data[i], str):
        datatmp = pandas.read_csv(self._data[i])
      elif isinstance(self._data[i], pandas.DataFrame):
        datatmp = self._data[i]

      # creating learner
      if self._template is None:
        learner = pyagrum.BNLearner(datatmp)
      else:
        learner = pyagrum.BNLearner(datatmp, self._template)

      self.updateState(learner, **self._states[i])
      bni = learner.learnBN()
      namei = f"bn{bnm.size()}"
      bni.setProperty("name", namei)
      bnm.add(namei, bni, w=self._weights[i])

    return bnm


class BNMBootstrapLearner(IMixtureLearner):
  """
  Allows to learn a BN and bootsrap-generated BNs. Learning a BN is not the only goal of this class. The purpose of bootstraping is to have an accuracy indicator about the
  BN learned from a given database.

  Notes
  -----
  - How is memory handled? First, to reduce memory consumption, only one BNLearner is instancied at most at a given time.
  - To keep one BNLearner at a time, we create a reference learner. To apply a method for the learning algorithm of all bootstraped BNs,
  "use" methods modify the reference learner. Then the other learners make use of ``BNLearner.copyState`` to update themself according to the reference learner.

  Parameters
  ----------
  source : str | pandas.DataFrame
      Database to learn from (csv format for now).
  template : pyagrum.BayesNet | Optional
      BN to use to find modalities.
  """

  def __init__(self, source, template=None, N=100):
    # loading source
    if isinstance(source, str):
      self._data = pandas.read_csv(source)
    elif isinstance(source, pandas.DataFrame):
      self._data = source

    # creating reference learner
    if template is None:
      self._ref_learner = pyagrum.BNLearner(self._data)
    else:
      self._ref_learner = pyagrum.BNLearner(self._data, template)
    self._template = template
    self._iter = N

  def updateState(self, learner, **kargs):
    raise NotImplementedError("Use the use... methods instead")

  def learnBNM(self):
    """
    Learns a reference BN from the database. Then add bootstrap-generated BNs to a BootstrapMixture object.
    """
    # Learn reference BN
    learner = self._ref_learner
    refBN = learner.learnBN()
    refBN.setProperty("name", "bn0")
    bnm = BNM.BootstrapMixture("bn0", refBN)

    n = self._data.shape[0]

    for i in range(self._iter):
      # updating learner
      weights_samples = dirichlet([1/n]*n, self._iter)*n
      for j in range(n):
        learner.setRecordWeight(j, weights_samples[i][j])

      bni = learner.learnBN()
      namei = f"bn{bnm.size() + 1}"
      bni.setProperty("name", namei)

      bnm.add(namei, bni)

    return bnm

  def useIter(self, N):
    """
    Set the number of bootstrap iterations used for learning.

    Parameters
    ----------
    N : int
        Number of iterations.
    """
    self._iter = N

  def useBDeuPrior(self, weight=1.0):
    self._ref_learner.useBDeuPrior(weight)

  def useDirichletPrior(self, source, weight=1.0):
    self._ref_learner.useDirichletPrior(source, weight)

  def useGreedyHillClimbing(self):
    self._ref_learner.useGreedyHillClimbing()

  def useK2(self, order):
    self._ref_learner.useK2(order)

  def useLocalSearchWithTabuList(self, tabu_size=100, nb_decrease=2):
    self._ref_learner.useLocalSearchWithTabuList(tabu_size, nb_decrease)

  def useMDLCorrection(self):
    self._ref_learner.useMDLCorrection()

  def useMIIC(self):
    self._ref_learner.useMIIC()

  def useNMLCorrection(self):
    self._ref_learner.useNMLCorrection()

  def useNoCorrection(self):
    self._ref_learner.useNoCorrection()

  def useNoPrior(self):
    self._ref_learner.useNoPrior()

  def useScoreAIC(self):
    self._ref_learner.useScoreAIC()

  def useScoreBD(self):
    self._ref_learner.useScoreBD()

  def useScoreBDeu(self):
    self._ref_learner.useScoreBDeu()

  def useScoreBIC(self):
    self._ref_learner.useScoreBIC()

  def useScoreK2(self):
    self._ref_learner.useScoreK2()

  def useScoreLog2Likelihood(self):
    self._ref_learner.useScoreLog2Likelihood()

  def useSmoothingPrior(self, weight=1):
    self._ref_learner.useSmoothingPrior(weight)
