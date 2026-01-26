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
from pyagrum.explain._ComputationMarginal import MarginalComputation
from pyagrum.explain._CustomShapleyCache import CustomShapleyCache
from pyagrum.explain._FIFOCache import FIFOCache


import numpy as np


class MarginalShallValues(ShallValues, MarginalComputation):
  """
  The MarginalShallValues class computes the Marginal Shall values in a Bayesian Network.
  """

  def __init__(self, bn: gum.BayesNet, background: tuple | None, sample_size: int = 1000, log: bool = True):
    """
    Note: All rows in the background data that contain NaN values in columns corresponding to variables in the Bayesian Network will be dropped.

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
      sigma=[],
      cache=FIFOCache(100),
      func1=self._joint,
      params1={},
      func2=self._weight,
      params2={},
    )

  def _coalition_contribution(self, k, ex, feature, fifo_cache, nodes_id, nodes_vals, cache):
    key1, key2, _ = cache.generate_keys(self.bn, None, feature, nodes_id)
    if k == 0:
      interv = self._data.copy()
      interv[:, nodes_id] = nodes_vals
      cache.set(
        ex,
        key1,
        self._value(
          data=interv,
          counts=self.counts,
          elements=self.vars_ids,
          sigma=[],
          cache=fifo_cache,
          func1=self._joint,
          params1={},
          func2=self._weight,
          params2={},
        ),
      )

    joint_prob_with = cache.get(ex, key1)
    joint_prob_without = cache.get(ex, key2) if len(key1) > 1 else cache.get(-1, ())
    return (joint_prob_with - joint_prob_without) / self._invcoeff_shap(len(self.vars_ids), len(nodes_id) - 1)

  def _shall_1dim(self, x):
    # Result initialisation.
    contributions = np.zeros(self.M)
    # Cache management.
    fifo_cache = FIFOCache(2000)
    custom_cache = CustomShapleyCache(5000)
    # Sets the baseline probability in the cache.
    custom_cache.set(-1, (), self.baseline)
    coalitions = self._coalitions(self.vars_ids)
    for nodes_id in coalitions:
      nodes_vals = x[nodes_id]
      for k, feature in enumerate(nodes_id):
        contributions[feature] += self._coalition_contribution(
          k, 0, int(feature), fifo_cache, nodes_id, nodes_vals, custom_cache
        )
    return contributions

  def _shall_ndim(self, x):
    # Result initialisation.
    contributions = np.zeros((self.M, len(x)))
    # Cache management.
    fifo_cache = FIFOCache(2000)
    custom_cache = CustomShapleyCache(5000)
    # Sets the baseline probability in the cache.
    custom_cache.set(-1, (), self.baseline)
    coalitions = self._coalitions(self.vars_ids)
    for nodes_id in coalitions:
      for ex, nodes_values in enumerate(x[:, nodes_id]):
        for k, feature in enumerate(nodes_id):
          contributions[feature, ex] += self._coalition_contribution(
            k, ex, int(feature), fifo_cache, nodes_id, nodes_values, custom_cache
          )
    return contributions
