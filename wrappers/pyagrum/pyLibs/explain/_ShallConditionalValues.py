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
from pyagrum.explain._FIFOCache import FIFOCache
from pyagrum.explain._ComputationConditional import ConditionalComputation

import numpy as np
from warnings import warn


class ConditionalShallValues(ShallValues, ConditionalComputation):
  """
  The ConditionalShallValues class computes the conditional Shall values in a Bayesian Network.
  """

  def __init__(self, bn: gum.BayesNet, background: tuple | None, sample_size: int = 1000, log: bool = True):
    """
    Note 1 : All rows in the background data that contain NaN values in columns corresponding to variables in the Bayesian Network will be dropped.
    Note 2 : For small databases SHALL values can be incorrect.

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

    super().__init__(bn, background, sample_size, log)  # Initializes the ShapleyValues class.
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

  def _coalition_contribution(self, k, ex, feature, nodes_id, nodes_vals, cache, fifo_cache):
    # key2 is always set since subsets are sorted by length
    key1, key2, _ = cache.generate_keys(self.bn, None, feature, nodes_id)
    # key1 : nodes_id, key2 : nodes id without feature
    if k == 0:
      idx = self._extract(self._data, nodes_id, nodes_vals)
      # warn(f"Extracted database is empty ({self.feat_names[nodes_id]} = {nodes_vals}). Conditional SHALL values may be incorrect. ")
      cache.set(
        ex,
        key1,
        self._value(
          data=self._data[idx],
          counts=self.counts[idx],
          elements=self.vars_ids,
          sigma=[],
          cache=fifo_cache,
          func1=self._joint,
          params1={},
          func2=self._weight,
          params2={},
        ),
      )

    joint_prob_with = cache.get(ex, key1)  # with feature
    joint_prob_without = cache.get(ex, key2) if len(key1) > 1 else cache.get(-1, "")  # without feature
    return (joint_prob_with - joint_prob_without) / self._invcoeff_shap(len(self.vars_ids), len(nodes_id) - 1)

  def _shall_1dim(self, x) -> np.ndarray:
    contributions = np.zeros((self.M))  # Initializes contributions array.
    fifo_cache = FIFOCache(2000)
    cache = CustomShapleyCache(5000)  # Initializes the custom cache.
    cache.set(-1, "", self.baseline)  # Sets the baseline probability in the cache.
    coalitions = self._coalitions(self.vars_ids)  # Generates coalitions.
    for nodes_id in coalitions:
      nodes_vals = x[nodes_id]  # Gets the values of the nodes in the coalition.
      for k, feature in enumerate(nodes_id):
        # Accumulates the contribution for each feature.
        contributions[feature] += self._coalition_contribution(
          k, 0, int(feature), nodes_id, nodes_vals, cache, fifo_cache
        )
    return contributions

  def _shall_ndim(self, x) -> np.ndarray:
    contributions = np.zeros((self.M, len(x)))  # Initializes contributions array.
    fifo_cache = FIFOCache(2000)
    cache = CustomShapleyCache(5000)  # Initializes the custom cache.
    cache.set(-1, "", self.baseline)  # Sets the baseline probability in the cache.

    coalitions = self._coalitions(self.vars_ids)  # Generates coalitions.
    for i, nodes_id in enumerate(coalitions):
      data_vals = x[:, nodes_id]  # Gets the values of the nodes in the coalition.
      for ex, nodes_vals in enumerate(data_vals):
        for k, feature in enumerate(nodes_id):
          contributions[feature, ex] += self._coalition_contribution(
            k, ex, int(feature), nodes_id, nodes_vals, cache, fifo_cache
          )
    return contributions
