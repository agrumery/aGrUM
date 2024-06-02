# -*- coding: utf-8 -*-
# (c) Copyright 2022-2023 by Pierre-Henri Wuillemin(@LIP6)  (pierre-henri.wuillemin@lip6.fr)

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
This module implements the forward sampling algorithm for CLG
"""

import numpy as np
from typing import Dict, List

import pandas as pd

from .CLG import CLG
from .Constants import NodeId


class ForwardSampling:
  _model: CLG
  id2mu: Dict[NodeId, float]  # the mu value during forward sampling
  id2samples: Dict[NodeId, List]

  def __init__(self, model: CLG):
    self._model = model
    self.id2mu = {}  # the mu value will change in every sample
    self.id2samples = {}

  def _findmu(self, i, val):
    """
    Find mu of variables in the i-th sample.

    Parameters
    ----------
    i : int
      We are in the No.i sample.
    val : NameOrId
      Name or id of the variable.
    
    Returns
    -------
    Dict[NodeId, float]
      Dict of NodeId to the mu value during forward sampling.
    """
    node = self._model.nameOrId(val)

    # no matter whether a variable have parents or not, its mu contains its own mu.
    self.id2mu[node] = self._model._id2var[node].mu()

    # check this variable's parents to get its complete mu
    for parentnode in self._model.parents(node):
      self.id2mu[node] += self._model._arc2coef[(parentnode, node)] * self.id2samples[parentnode][i]

    return self.id2mu

  def makeSample(self, N):
    """
    Make N samples using forward sampling.

    Parameters
    ----------
    N : int
      Amount of samples.

    Returns
    -------
    ForwardSampling
      the object itself.
    """
    for node in self._model.nodes():
      self.id2samples[node] = []

    for i in range(N):
      for node in self._model.topologicalOrder():
        self._findmu(i, node)
        self.id2samples[node].append(np.random.normal(self.id2mu[node],
                                                      self._model._id2var[node].sigma(),
                                                      1)[0])
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
      return np.array([self.id2samples[node] for node in self._model.nodes()]).T
    else:
      return np.array(self.id2samples[self._model.nameOrId(val)])

  def mean_sample(self, val):
    """
    Figure out the mean of variable <val> in the samples

    Parameters
    ----------
    val : NameOrId
      Name or id of the variable.
    
    Returns
    -------
    float
      The mean of variable <val>'s samples
    """
    return self.toarray(val).mean()

  def variance_sample(self, val):
    """
    Figure out the variance of variable <val> in the samples.

    Parameters
    ----------
    val : NameOrId
      Name or id of the variable.
    
    Returns
    -------
    float
      The variance of variable <val>'s samples
    """
    return self.toarray(val).var()

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
      The standard deviation of variable <val>'s samples
    """
    return self.toarray(val).std()

  def covariance_sample(self, vals):
    """
    Computes the covariance between variables in <vals>.

    Parameters
    ----------
    vals : List[NameOrId]
      List of names or ids of the variables.
    
    Returns
    -------
    np.ndarray
      The covariance between variables in <vals>.
    """
    samples = [self.toarray(val) for val in vals]
    return np.cov(samples)

  def topandas(self):
    """
    Convert the samples to pandas dataframe.

    Parameters
    ----------
    sample_name : str
      Name of the new csv file.
    """
    name2samples = {}
    for node in self._model.nodes():
      name2samples[self._model.name(node)] = self.id2samples[node]

    return pd.DataFrame(name2samples)

  def tocsv(self, sample_name:str):
    """
    Convert the samples to csv.

    Parameters
    ----------
    sample_name : str
      Name of the new csv file.
    """
    self.topandas().to_csv(sample_name, index=False, sep=',')
