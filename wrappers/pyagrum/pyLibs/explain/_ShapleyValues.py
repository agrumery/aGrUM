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

from pyagrum.explain._Explainer import Explainer
from abc import abstractmethod
from pyagrum.explain._Explanation import Explanation

# Calculations
import pandas as pd
import numpy as np

# aGrUM
import pyagrum as gum

# GL
import warnings


class ShapleyValues(Explainer):
  """
  The ShapleyValues class is an abstract base class for computing Shapley values in a Bayesian Network.
  """

  def __init__(self, bn, target, logit):
    """
    Parameters:
    ------
    bn : pyagrum.BayesNet
        The Bayesian Network.
    target : int | str
        The node id (or node name) of the target.
    logit : bool
        If True, applies the logit transformation to the probabilities.

    Raises:
    ------
    TypeError : If bn is not a gum.BayesNet or target is not an integer or string.
    ValueError : If target is not a valid node id in the Bayesian Network.
    """
    super().__init__(bn)
    if isinstance(target, str):
      if target not in bn.names():
        raise ValueError("Target node name '{}' not found in the Bayesian Network.".format(target))
      target = bn.idFromName(target)  # Convert node name to ID.
    elif isinstance(target, int):
      if target not in bn.nodes():
        raise ValueError("Target node ID {} not found in the Bayesian Network.".format(target))
    else:
      raise TypeError("Target must be a node ID (int) or a node name (str), but got {}".format(type(target)))
    if not isinstance(logit, bool):
      warnings.warn("logit should be a boolean, unexpected calculation may occur.", UserWarning)

    # Class attributes.
    self.target = target  # ID of the target node.
    self.target_name = self.feat_names[self.target]
    self._mb = self._markov_blanket()
    self.ie = gum.LazyPropagation(self.bn)  # Inference engine for the Bayesian Network.
    self.ie.addTarget(self.target)  # Setting the target for inference.
    self.func = self._logit if logit else self._identity  # Function to apply to the probabilities.

  def _markov_blanket(self):
    # Retrieves the Markov blanket of the target node.
    mb = gum.MarkovBlanket(self.bn, self.target).nodes()
    mb.remove(self.target)
    return sorted(list(mb))

  def _posterior(self, evidces: dict[int, int]):
    # Returns the posterior probability of the target given the evidence.
    self.ie.updateEvidence(evidces)
    return self.ie.posterior(self.target).toarray()

  @abstractmethod
  def _shap_1dim(self, x, elements):
    # Computes the Shapley values for a single instance.
    # This method should be implemented in subclasses.
    raise NotImplementedError("This method should be implemented in subclasses.")

  @abstractmethod
  def _shap_ndim(self, x, elements):
    # Computes the Shapley values for multiple instances.
    # This method should be implemented in subclasses.
    raise NotImplementedError("This method should be implemented in subclasses.")

  def compute(self, data: tuple | None, N=100):
    """
    Computes the Shapley values for the target node based on the provided data.

    Parameters:
    ----------
    data : tuple | None
        A tuple containing a pandas DataFrame, Series or a dictionary and a boolean indicating whether data are provided with labels. If None, a random sample of size N is generated.
    N : int
        The number of samples to generate if data is None.

    Returns:
    -------
    Explanation
        An Explanation object containing the Shapley values and importances for the target node.

    Raises:
    ------
    TypeError : If the first element of data is not a pd.DataFrame, pd.Series or dict, or if N is not an integer when data is None.
    ValueError : If N is less than 2 when data is None.
    """
    if data is None:
      if not isinstance(N, int):
        raise TypeError("Since df is None, N must be an integer, but got {}".format(type(N)))
      if N < 2:
        raise ValueError("N must be greater than 1, but got {}".format(N))
      y = gum.generateSample(self.bn, N, with_labels=False)[0].reindex(columns=self.feat_names).to_numpy()
      elements = [i for i in range(self.M) if i != self.target]
      # Remove duplicate rows in x and unused columns.
      mask_cols = [i for i in range(self.M) if i not in elements]
      _, idx = np.unique(y[:, elements], axis=0, return_index=True)
      y = y[idx, :]
      y[:, mask_cols] = 0
      contributions = self._shap_ndim(y, sorted(elements))

    else:
      if not isinstance(data, tuple):
        raise TypeError(f"`data` must be a tuple (pd.DataFrame, bool).")
      df, with_labels = data
      if not isinstance(with_labels, bool):
        warnings.warn(
          f"The second element of `data` should be a boolean, but got {type(with_labels)}. Unexpected calculations may occur."
        )
      dtype = "U50" if with_labels else int

      if isinstance(df, pd.Series):
        # Here we are sure that df is a single instance (a Series).
        s = df.dropna()
        x = np.empty(self.M, dtype=dtype)
        elements = []
        for feat in s.index:
          id = self.bn.idFromName(feat)
          x[id] = s[feat]
          if id != self.target:
            elements.append(id)
        if with_labels:
          y = self._labelToPos_row(x, elements)
        else:
          y = x
        contributions = self._shap_1dim(y, sorted(elements))

      elif isinstance(df, pd.DataFrame):
        df_clean = df.dropna(axis=1)
        if len(df_clean) == 1:
          # Here we are sure that df is a single instance (a DataFrame with one row).
          x = np.empty(self.M, dtype=dtype)
          elements = []
          for feat in df_clean.columns:
            id = self.bn.idFromName(feat)
            x[id] = df_clean[feat].values[0]
            if id != self.target:
              elements.append(id)
          if with_labels:
            y = self._labelToPos_row(x, elements)
          else:
            y = x
          contributions = self._shap_1dim(y, sorted(elements))

        else:
          x = np.empty((len(df_clean), self.M), dtype=dtype)
          elements = []
          for feat in df_clean.columns:
            id = self.bn.idFromName(feat)
            x[:, id] = df_clean[feat].values
            if id != self.target:
              elements.append(id)
          # Remove duplicate rows in x and unused columns.
          mask_cols = [i for i in range(self.M) if i not in elements]
          _, idx = np.unique(x[:, elements], axis=0, return_index=True)
          x = x[idx, :]
          x[:, mask_cols] = 0
          if with_labels:
            y = self._labelToPos_df(x, elements)
          else:
            y = x
          contributions = self._shap_ndim(y, sorted(elements))

      elif isinstance(df, dict):
        try:
          N = len(list(df.values())[0])
          if not isinstance(list(df.values())[0], (list, np.ndarray)):
            raise TypeError("Each value in the dictionary must be a list or a numpy array.")
          elements = []
          x = np.empty((N, self.M), dtype=dtype)
          for feat in df.keys():
            if all(not (x is None) and not (isinstance(x, float) and np.isnan(x)) for x in df[feat]):
              id = self.bn.idFromName(feat)
              x[:, id] = df[feat]
              if id != self.target:
                elements.append(id)
          # Remove duplicate rows in x and unused columns.
          mask_cols = [i for i in range(self.M) if i not in elements]
          _, idx = np.unique(x[:, elements], axis=0, return_index=True)
          x = x[idx, :]
          x[:, mask_cols] = 0
          if with_labels:
            y = self._labelToPos_df(x, elements)
          else:
            y = x
          contributions = self._shap_ndim(y, sorted(elements))

        except TypeError:
          # Here we are sure that df is a single instance (a dictionary with one row).
          x = np.empty(self.M, dtype=dtype)
          elements = []
          for feat in df.keys():
            if not (df[feat] is None):
              id = self.bn.idFromName(feat)
              x[id] = df[feat]
              if id != self.target:
                elements.append(id)
          if with_labels:
            y = self._labelToPos_row(x, elements)
          else:
            y = x
          contributions = self._shap_1dim(y, sorted(elements))

      else:
        raise TypeError(
          "The first element of `data` must be a pandas DataFrame, Series or a dictionary, but got {}".format(type(df))
        )

    if contributions.ndim == 2:
      values = {
        z: {self.feat_names[i]: float(contributions[i, z]) for i in elements} for z in range(contributions.shape[1])
      }
      importances = {
        z: {self.feat_names[i]: abs(float(contributions[i, z])) for i in elements}
        for z in range(contributions.shape[1])
      }
      explanation = Explanation(
        values,
        importances,
        list(self.feat_names[sorted(elements)]),
        x[sorted(elements)],
        self.baseline,
        self.func.__name__,
        "SHAP",
      )
    else:
      values = {
        z: {self.feat_names[i]: [float(v) for v in contributions[i, :, z]] for i in elements}
        for z in range(contributions.shape[2])
      }
      mean_abs = np.mean(np.abs(contributions), axis=1)
      importances = {
        z: {self.feat_names[i]: abs(float(mean_abs[i, z])) for i in elements} for z in range(contributions.shape[2])
      }
      explanation = Explanation(
        values,
        importances,
        list(self.feat_names[sorted(elements)]),
        y[:, sorted(elements)],
        self.baseline,
        self.func.__name__,
        "SHAP",
      )
    return explanation
