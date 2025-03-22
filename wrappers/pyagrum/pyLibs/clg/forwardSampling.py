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

"""
This module implements the forward sampling algorithm for CLG
"""

import numpy as np
from typing import Dict, List

import pandas as pd

from .CLG import CLG
from .constants import NodeId


class ForwardSampling:
  def __init__(self, model: CLG):
    self._model = model
    self._id2samples = {}

  def makeSample(self, N, seed: int = None):
    """
    Make N samples using forward sampling.

    Parameters
    ----------
    N : int
      Amount of samples.
    seed : int
      Seed for the random number generator

    Returns
    -------
    ForwardSampling
      the object itself.
    """
    nprng = np.random.default_rng(seed)
    self._id2samples = {}

    for node in self._model.topologicalOrder():
      v = self._model.variable(node)
      mu = v.mu()
      for parent in self._model.parents(node):
        mu += self._model.coefArc(parent, node) * self._id2samples[parent]
      self._id2samples[node] = nprng.normal(mu, v.sigma(), N)

    return self

  def toarray(self, val=None):
    """
    Return the samples of variable <val>.

    Parameters
    ----------
    val : NameOrId or None
      Name or id of the variable.

    Returns
    -------
    np.ndarray
      The samples of variable <val> or all the samples if <val> is None.
    """
    if val is None:
      return np.stack([self._id2samples[node] for node in self._model.nodes()]).T
    else:
      return self._id2samples[self._model.nameOrId(val)]

  def mean_sample(self, val):
    """
    Figure out the mean of variable <val> in the samples

    Parameters
    ----------
    val : NameOrId or None
      Name or id of the variable.

    Returns
    -------
    float
      The mean of variable <val>'s samples or all the means as a dict if <val> is None.
    """
    if val is None:
      return {self._model.variable(k).name(): self._id2samples[k].mean() for k in self._model.topologicalOrder()}
    else:
      return self._id2samples[self._model.nameOrId(val)].mean()

  def variance_sample(self, val):
    """
    Figure out the variance of variable <val> in the samples.

    Parameters
    ----------
    val : NameOrId or None
      Name or id of the variable.

    Returns
    -------
    float
      The variance of variable <val>'s samples or all the variances as a dict if <val> is None.
    """
    if val is None:
      return {self._model.variable(k).name(): self._id2samples[k].var() for k in self._model.topologicalOrder()}
    else:
      return self._id2samples[self._model.nameOrId(val)].var()

  def stddev_sample(self, val):
    """
    Figure out the standard deviation of variable <val> in the samples.

    Parameters
    ----------
    val : NameOrId
      Name or id of the variable.

    Returns
    -------
    float
      The standard deviation of variable <val>'s samples or all the standard deviations as a dict if <val> is None.
    """
    if val is None:
      return {self._model.variable(k).name(): self._id2samples[k].std() for k in self._model.topologicalOrder()}
    else:
      return self._id2samples[self._model.nameOrId(val)].std()

  def covariance_sample(self, vals=None):
    """
    Computes the covariance between variables in <vals>.

    Parameters
    ----------
    vals : List[NameOrId] option
      List of names or ids of the variables.

    Returns
    -------
    np.ndarray
      The covariance between variables in <vals> or all the covariances if <vals> is None.
    """
    if vals is None:
      samples = [self._id2samples[k] for k in self._model.nodes()]
    else:
      samples = [self._id2samples[self._model.nameOrId(k)] for k in vals]

    return np.cov(samples)

  def topandas(self):
    """
    Convert the samples to pandas dataframe.

    Parameters
    ----------
    sample_name : str
      Name of the new csv file.
    """
    return pd.DataFrame({self._model.name(node): self._id2samples[node] for node in self._model.nodes()})

  def tocsv(self, sample_name: str):
    """
    Convert the samples to csv.

    Parameters
    ----------
    sample_name : str
      Name of the new csv file.
    """
    self.topandas().to_csv(sample_name, index=False, sep=',')
