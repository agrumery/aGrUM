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
