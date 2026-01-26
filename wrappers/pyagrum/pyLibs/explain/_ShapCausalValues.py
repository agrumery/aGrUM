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

import pyagrum as gum
from pyagrum.explain._ShapleyValues import ShapleyValues
from pyagrum.explain._ComputationCausal import CausalComputation
from pyagrum.explain._CustomShapleyCache import CustomShapleyCache
from pyagrum.explain._FIFOCache import FIFOCache

# Calculus
import numpy as np
import pandas as pd

# GL
import warnings


class CausalShapValues(ShapleyValues, CausalComputation):
  """
  The CausalShapValues class computes the Causal Shapley values for a given target node in a Bayesian Network.
  """

  def __init__(self, bn, target, background: tuple | None, sample_size=1000, logit=True):
    """
    Parameters:
    ------
    bn : pyagrum.BayesNet
        The Bayesian Network.
    target : int | str
        The node id (or node name) of the target.
    background : Tuple(pandas.DataFrame, bool) | None
        A tuple containing a pandas DataFrame and a boolean indicating whether the DataFrame contains labels or positions.
    sample_size : int
        The size of the background sample to generate if `background` is None.
    logit : bool
        If True, applies the logit transformation to the probabilities.

    Raises:
    ------
    TypeError : If bn is not a gum.BayesNet instance, background is not a tuple or target is not an integer or string.
    ValueError : If target is not a valid node id in the Bayesian Network or if sample_size is not a positive integer.
    """
    super().__init__(bn, target, logit)
    # Processing background data
    if background is None:
      if not isinstance(sample_size, int):
        raise TypeError("When `data`=None, `sample_size` must be an integer, but got {}".format(type(sample_size)))
      else:
        if sample_size <= 1:
          raise ValueError("`sample_size` must be greater than 1, but got {}".format(sample_size))
      data = gum.generateSample(self.bn, sample_size, with_labels=False)[0].reindex(columns=self.feat_names).to_numpy()
    else:
      if not isinstance(background, tuple):
        raise TypeError(f"`background` must be a tuple (pd.DataFrame, bool).")
      data, with_labels = background
      if not isinstance(with_labels, bool):
        warnings.warn(
          f"The second element of `background` should be a boolean, but got {type(with_labels)}. Unexpected calculations may occur."
        )
      if not isinstance(data, pd.DataFrame):
        raise TypeError("The first element of `background` must be a pandas DataFrame, but got {}".format(type(data)))
      if data.shape[0] < 2:
        warnings.warn("You are giving a single row as a background data, which will lead to biased Shapley values.")
      if data.shape[1] != self.M:
        raise ValueError(
          "The number of columns in the background data must match the number of variables in the Bayesian network. Although values outside the Markov blanket, including the target, are unused, they are required for indexing purposes."
        )
      data = data.reindex(columns=self.feat_names).to_numpy()
      if with_labels:
        data = self._labelToPos_df(data, [i for i in range(self.M) if i != self.target])
    self._data, self.counts = np.unique(data, return_counts=True, axis=0)
    self._N = len(self._data)
    # Calculating the baseline
    self.baseline = self.func(
      self._value(
        data=self._data,
        counts=self.counts,
        elements=[i for i in range(self.M) if i != self.target],
        sigma=self._mb,
        cache=FIFOCache(100),
        func1=self._posterior,
        params1={},
        func2=self._weight,
        params2={"doLazy": gum.LazyPropagation(self.bn)},
      )
    )

  def _shap_1dim(self, x, elements):
    # Computes the Shapley values for a 1-dimensional input x (local explanation).
    contributions = np.zeros((self.M, self.bn.variable(self.target).domainSize()))  # Initializes contributions array.
    cache = CustomShapleyCache(5000)
    markovImpact = FIFOCache(1000)
    cache.set(0, (), self.baseline)  # Sets the baseline probability in the cache.
    coalitions = self._coalitions(elements)  # Compute the coalitions

    for tau in coalitions:
      self.ie.eraseAllEvidence()  # Clears all evidence from the inference engine.
      doNet = self._doCalculus(self.bn, tau)  # Creates a new Bayesian Network to perform do-calculus.
      sigma = self._outOfCoalition(tau, range(self.M))  # Extracts the nodes outside the coalition tau.
      alpha = x[tau]  # Instanciation of tau
      self._chgCpt(doNet, tau, alpha)  # Changes the conditional probability tables to perform do-calculus.
      doLazy = gum.LazyPropagation(
        doNet
      )  # Creates a lazy propagation inference engine to compute partial join probabilities.
      doLazy.addTarget(self.target)
      idx = self._extract(self._data, tau, alpha)
      posterior_with = self.func(
        self._value(
          data=self._data[idx],
          counts=self.counts[idx],
          elements=elements,
          sigma=sigma,
          cache=markovImpact,
          func1=self._posterior,
          params1={},
          func2=self._weight,
          params2={"doLazy": doLazy},
        )
      )

      cache.set(0, tuple(tau), posterior_with)
      # Contribution of each feature
      for t in tau:
        key = tuple((f for f in tau if f != t))
        posterior_without = cache.get(0, key)
        contributions[t] += self._shap_term(posterior_with, posterior_without, len(elements), len(tau) - 1)
    return contributions

  def _shap_ndim(self, x, elements):
    contributions = np.zeros(
      (self.M, len(x), self.bn.variable(self.target).domainSize())
    )  # Initializes contributions array.
    cache = CustomShapleyCache(5000)  # Initializes the custom cache.
    markovImpact = FIFOCache(1000)
    cache.set(0, (), self.baseline)  # Sets the baseline probability in the cache.
    coalitions = self._coalitions(elements)  # Compute the coalitions

    for tau in coalitions:
      self.ie.eraseAllEvidence()  # Clears all evidence from the inference engine.
      doNet = self._doCalculus(self.bn, tau)  # Creates a new Bayesian Network to perform do-calculus.
      sigma = self._outOfCoalition(tau, range(self.M))  # Extracts the nodes outside the coalition tau.

      for i in range(len(x)):  # Iterates over each example in x
        alpha = x[i, tau]  # Instanciation of tau
        self._chgCpt(doNet, tau, alpha)  # Changes the conditional probability tables to perform do-calculus.
        doLazy = gum.LazyPropagation(
          doNet
        )  # Creates a lazy propagation inference engine to compute partial join probabilities.
        doLazy.addTarget(self.target)
        idx = self._extract(self._data, tau, alpha)
        posterior_with = self.func(
          self._value(
            data=self._data[idx],
            counts=self.counts[idx],
            elements=elements,
            sigma=sigma,
            cache=markovImpact,
            func1=self._posterior,
            params1={},
            func2=self._weight,
            params2={"doLazy": doLazy},
          )
        )

        cache.set(i, tuple(tau), posterior_with)
        # Contribution of each feature
        for t in tau:
          key = tuple((f for f in tau if f != t))
          posterior_without = cache.get(i, key) if len(key) > 0 else cache.get(0, ())
          contributions[t, i] += self._shap_term(posterior_with, posterior_without, len(elements), len(tau) - 1)
    return contributions
