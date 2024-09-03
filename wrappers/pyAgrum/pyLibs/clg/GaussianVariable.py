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
class GaussianVariable: Continuous Gaussian Variable
"""


class GaussianVariable:
  def __init__(self, name: str, mu: float, sigma: float):
    self._mu = mu
    self._sigma = sigma
    self._name = name

  def __str__(self):
    return self._name

  def __repr__(self):
    return str(self)

  def name(self) -> str:
    """
    Returns the name of the variable.

    Returns
    -------
    str
      The name of the variable.
    """
    return self._name

  def mu(self) -> float:
    """
    Returns the mean of the variable.

    Returns
    -------
    float
      The mean of the variable.
    """
    return self._mu

  def sigma(self) -> float:
    """
    Returns the standard deviation of the variable.

    Returns
    -------
    float
      The standard deviation of the variable.
    """
    return self._sigma

  def setMu(self, mu: float):
    """
    Set the mean of the variable.

    Parameters
    ----------
    mu : float
      The new mean of the variable.
    """
    self._mu = mu

  def setSigma(self, sigma: float):
    """
    Set the standard deviation of the variable.

    Parameters
    ----------
    sigma : float
      The new standard deviation of the variable.
    """
    self._sigma = sigma

  def __str__(self)->str:
    return f"{self._name}:{self._mu}[{self._sigma}]"
