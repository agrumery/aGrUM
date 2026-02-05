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

from abc import ABC, abstractmethod
import pyagrum as gum
from pyagrum.explain._FIFOCache import FIFOCache

# Calculations
import numpy as np
import math
from itertools import combinations
from typing import Callable


class Explainer(ABC):
  """
  ___Documentation___
  """

  def __init__(self, bn: gum.BayesNet):
    """
    Parameters:
    ------
    bn : pyagrum.BayesNet
        The Bayesian Network.

    Raises:
    ------
    TypeError : If bn is not a gum.BayesNet.
    """
    super().__init__()
    if not isinstance(bn, gum.BayesNet):
      raise TypeError("bn must be a gum.BayesNet instance, but got {}".format(type(bn)))
    self.bn = bn
    self.M = len(bn.nodes())  # Total number of nodes in the Bayesian Network.
    self.feat_names = np.empty(self.M, dtype=object)  # Array to store feature names by their node ID.
    for name in self.bn.names():
      self.feat_names[self.bn.idFromName(name)] = name

  @staticmethod
  def _log(p: float):
    # Applies the log function to the probability.
    return np.log(p) if p > 0 else -np.inf

  @staticmethod
  def _logit(p: any):
    # Applies the logit transformation to the probabilities.
    p = np.asarray(p)  # Guarantee p is a numpy array.
    with np.errstate(divide="ignore", invalid="ignore"):
      result = np.log(p / (1 - p))
    result = np.where(p == 0, 0.0, result)
    result = np.where(p == 1, 0.0, result)
    return result

  @staticmethod
  def _identity(x: any):
    # Returns the input as is (identity function).
    return x

  def _labelToPos_row(self, x: np.ndarray, elements: list[int]) -> np.ndarray:
    # Converts labels to positions for a single instance.
    y = np.empty(shape=x.shape, dtype=int)
    for i in elements:
      try:
        val = self.bn.variable(i).posLabel(x[i])
      except:
        val = int(x[i])
      y[i] = val
    return y

  def _labelToPos_df(self, x: np.ndarray, elements: list[int]) -> np.ndarray:
    # Converts labels to positions for multiple instances.
    y = np.empty(shape=x.shape, dtype=int)  # Initialisation
    posLabelVect = np.vectorize(lambda i, j: self.bn.variable(int(j)).posLabel(i))
    for j in elements:
      try:
        self.bn.variable(j).posLabel(x[0, j])
        y[:, j] = posLabelVect(x[:, j], j)
      except NotImplementedError:
        y[:, j] = x[:, j].astype(int)
    return y

  @staticmethod
  def _extract(data: np.ndarray, tau: list[int], alpha: np.ndarray) -> np.ndarray:
    # Extracts the data given the values in alpha for the nodes in tau.
    mask = np.all(data[:, tau] == alpha, axis=1)
    idx = np.nonzero(mask)[0]
    return idx

  @staticmethod
  def _coalitions(elements_for_coalitions):
    # Generates all possible coalitions from the given elements.
    all_coalitions = []
    for r in range(1, len(elements_for_coalitions) + 1):
      for combo in combinations(elements_for_coalitions, r):
        all_coalitions.append(list(combo))
    return all_coalitions

  @staticmethod
  def _invcoeff_shap(m, s):
    # Computes the inverse coefficient for the Shapley value formula.
    return (m - s) * math.comb(m, s)

  @staticmethod
  def _shap_term(prob_with, prob_without, m, s):
    return (prob_with - prob_without) / Explainer._invcoeff_shap(m, s)

  def _value(
    self, data: np.ndarray, counts: list[int], elements: list[int], sigma: list[int], cache: FIFOCache, **kwargs
  ) -> float | np.ndarray:
    # -- #
    length: int = len(data)
    func1: Callable = kwargs["func1"]
    func2: Callable = kwargs["func2"]
    params1: dict[str, any] = kwargs["params1"]
    params2: dict[str, any] = kwargs["params2"]
    # -- #

    val = 0.0
    norm = 0.0
    for i in range(length):
      evidces1 = {self.feat_names[key]: int(data[i, key]) for key in elements}
      evidces2 = {self.feat_names[key]: int(data[i, key]) for key in sigma}
      term1 = cache.get(tuple(data[i, elements]), None)
      if term1 is None:
        term1 = func1(evidces1, **params1)
        cache[tuple(data[i, elements])] = term1

      term2 = func2(evidces2, counts[i], **params2)
      val += term1 * term2
      norm += term2

    return 0.0 if norm == 0.0 else val / norm

  @abstractmethod
  def compute(self, data: tuple | None, N=100):
    # Computes the Shapley values for the target node based on the provided data.
    # This method must be implemented in subclasses
    raise NotImplementedError("This method must be implemented in subclasses.")
