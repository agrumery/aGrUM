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
from pyagrum.explain._ShallValues import ShallValues
from pyagrum.explain._CustomShapleyCache import CustomShapleyCache
from pyagrum.explain._ComputationCausal import CausalComputation
from pyagrum.explain._FIFOCache import FIFOCache

import numpy as np


class CausalShallValues(ShallValues, CausalComputation):
  """
  The CausalShallValues class computes the Causal Shall values in a Bayesian Network.
  """

  def __init__(self, bn: gum.BayesNet, background: tuple | None, sample_size: int = 1000, log: bool = True):
    """
    Note 1 : All rows in the background data that contain NaN values in columns corresponding to variables in the Bayesian Network will be dropped.
    Note 2 : In comparison to Marginal and Conditional Shall values it is impossible to calculate empirical probabilities 'true to the data'.
    We are forced to calculate probabilités 'true to the model'.


    Parameters:
    ------
     bn : pyagrum.BayesNet
        The Bayesian Network.
    background : tuple[pandas.DataFrame, bool] | None
        A tuple containing a pandas DataFrame and a boolean indicating whether the DataFrame includes labels or positional values.
    sample_size : int
        The size of the background sample to generate if `background` is None.
    log : bool
        If True, applies a logarithmic transformation to the probabilities.

    Raises
    ------
    TypeError : If bn is not a gum.BayesNet instance, background is not a tuple.
    ValueError : If background data does not contain all variables present in the Bayesian Network or if
        background data is empty after rows with NaNs were dropped.

    Raises:
    ------
    TypeError : If bn is not a gum.BayesNet instance, background is not a tuple.
    ValueError : If background data does not contain all variables present in the Bayesian Network or if
        background data is empty after rows with NaNs were dropped.
    """

    super().__init__(bn, background, sample_size, log)
    self.baseline = self._value(
      data=self._data,
      counts=self.counts,
      elements=self.vars_ids,
      sigma=self.vars_ids,
      cache=FIFOCache(100),
      func1=self._joint,
      params1={},
      func2=self._weight,
      params2={"doLazy": gum.LazyPropagation(self.bn)},
    )

  def _coalition_contribution(self, posterior_prob_with, posterior_prob_without, m, s):
    return (posterior_prob_with - posterior_prob_without) / self._invcoeff_shap(m, s)

  def _shall_1dim(self, x):
    contributions = np.zeros(self.M)  # M : number of nodes in BN

    # Caches
    custom_cache = CustomShapleyCache(5000)
    fifo_cache = FIFOCache(1000)
    # Sets the baseline probability in the cache.
    custom_cache.set(0, (), self.baseline)
    # Compute the coalitions
    coalitions = self._coalitions(self.vars_ids)

    for tau in coalitions:
      # self.ie.eraseAllEvidence()
      doNet = self._doCalculus(self.bn, tau)  # new BN
      sigma = self._outOfCoalition(tau, self.vars_ids)  # all nodes \ tau

      doInst = gum.Instantiation()
      for var in doNet.ids(self.feat_names):
        doInst.add(doNet.variable(var))

      # Instanciation of tau
      alpha = x[tau]  # extract columns in tau
      if sigma != []:
        self._chgCpt(doNet, tau, alpha)
        doLazy = gum.LazyPropagation(doNet)
        doLazy.addTarget(tau[0])  # see if target should be added for optimization
        idx = self._extract(self._data, tau, alpha)
        # Compute the value for this coalition.
        joint_with = self._value(
          data=self._data[idx],
          counts=self.counts[idx],
          elements=self.vars_ids,
          sigma=sigma,
          cache=fifo_cache,
          func1=self._joint,
          params1={},
          func2=self._weight,
          params2={"doLazy": doLazy},
        )
      else:
        self.inst.fromdict({self.feat_names[key]: int(val) for key, val in zip(tau, alpha)})
        joint = self.bn.jointProbability(self.inst)
        joint_with = self.func(joint)

      custom_cache.set(0, tuple(tau), joint_with)
      # Contribution of each feature
      for t in tau:
        key = tuple((f for f in tau if f != t))
        joint_without = custom_cache.get(0, key)
        contributions[t] += self._coalition_contribution(joint_with, joint_without, len(self.vars_ids), len(tau) - 1)
    return contributions

  def _shall_ndim(self, x):
    # Initialisation
    contributions = np.zeros((self.M, len(x)))

    # Caches
    custom_cache = CustomShapleyCache(5000)
    fifo_cache = FIFOCache(1000)
    # Sets the baseline probability in the cache.
    custom_cache.set(0, (), self.baseline)
    # Compute the coalitions
    coalitions = self._coalitions(self.vars_ids)

    for tau in coalitions:
      doNet = self._doCalculus(self.bn, tau)
      sigma = self._outOfCoalition(tau, self.vars_ids)

      for i in range(len(x)):
        alpha = x[i, tau]
        if sigma != []:
          # Instanciation of tau
          self._chgCpt(doNet, tau, alpha)  # BN knowing alpha
          doLazy = gum.LazyPropagation(doNet)
          doLazy.addTarget(tau[0])  # just to speed up the calculation
          idx = self._extract(self._data, tau, alpha)
          # Compute the value for this coalition.
          joint_with = self._value(
            data=self._data[idx],
            counts=self.counts[idx],
            elements=self.vars_ids,
            sigma=sigma,
            cache=fifo_cache,
            func1=self._joint,
            params1={},
            func2=self._weight,
            params2={"doLazy": doLazy},
          )

        else:
          self.inst.fromdict({self.feat_names[key]: int(val) for key, val in zip(tau, alpha)})
          joint = self.bn.jointProbability(self.inst)
          joint_with = self.func(joint)

        custom_cache.set(i, tuple(tau), joint_with)
        # Contribution of each feature
        for t in tau:
          key = tuple((f for f in tau if f != t))
          joint_without = custom_cache.get(i, key) if len(key) > 0 else custom_cache.get(0, ())
          contributions[t, i] += self._coalition_contribution(
            joint_with, joint_without, len(self.vars_ids), len(tau) - 1
          )
    return contributions
