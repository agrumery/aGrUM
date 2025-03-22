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

from abc import abstractmethod
from typing import List
from scipy.stats import f, chi2
import logging
import math

import pyagrum

from pyagrum.ctbn import CIM
from pyagrum.ctbn import CTBN
from pyagrum.ctbn.constants import ALPHA, FAIL_TO_REJECT, REJECT
from pyagrum.ctbn import Trajectory, Stats


class IndepTest:
  """
  Mother class used to test independance between 2 variables knowing some other parents.
  """

  @abstractmethod
  def testIndep(self, X: str, Y: str, U: List[str]) -> bool:
    """
    Parameters
    ----------
    X : str
        Head of the arc we want to test.
    Y : str
        Tail of the arc we want to test.
    U : List[str]
        Known parents.
    """
    pass


class Oracle(IndepTest):
  """
  Oracle's testing tools.

  Parameters
  ----------
  ctbn : CTBN
  """

  def __init__(self, ctbn: CTBN):
    super().__init__()
    self.ctbn = ctbn

  def testIndep(self, X: str, Y: str, U: List[str]) -> bool:
    """
    Parameters
    ----------
    X : str
        Head of the arc we want to test.
    Y : str
        Tail of the arc we want to test.
    U : List[str]
        Known parents.

    Returns
    -------
    bool
        False if there is an arc from Y to X knowing U, True otherwise.
    """
    return not self.ctbn.graph.existsArc(self.ctbn.name2id[Y], self.ctbn.name2id[X])


def sqrtTensor(tensor: pyagrum.Tensor) -> "pyagrum.Tensor":
  """
  Applies sqrt function to all values inside the tensor.

  Parameters
  ----------
  tensor : pyagrum.Tensor
      tensor to play sqrt to.

  Returns
  -------
  pyagrum.Tensor
      sqrt of tensor.
  """
  res = pyagrum.Tensor(tensor)
  i: pyagrum.Instantiation = pyagrum.Instantiation(res)
  i.setFirst()
  while not i.end():
    res.set(i, math.sqrt(res.get(i)))
    i.inc()

  return res


class FChi2Test(IndepTest):
  """
  This class use 2 independence tests : Fisher Test (F-test) and chi2 Test.
  To test independence between 2 variables, we first consider them independent. There is independence until
  one of the 2 tests (F and chi2) contradict the independence hypothesis. If the hyopothesis is not rejected,
  the variables are considered independent.

  Parameters
  ----------
  tr : Trajectory
      Samples used to extract stats.
  """

  def __init__(self, tr: Trajectory):
    self.tr = tr
    self.stats_added = False
    self.chiSqValue = None

  def addVariables(self, X: str, Y: str, U: List[str]):
    """
    Saves variables ``X`` and ``Y`` and the conditioning set ``U``, and generates stats to be used in statistical tests.

    Parameters
    ----------
    X : str
        Name of the variable.
    Y : str
        Name of the variable to test independence from, not in ``U``.
    U : List[str]
        List of conditioning variables.
    """
    self.stats: Stats = Stats(self.tr, X, Y, U)

    # logging.debug("Means: %s\n%s", self.stats.Qxy, self.stats.Qx)

    self.x_from: str = CIM.varI(X)
    self.x_to: str = CIM.varJ(X)

    self.r1: pyagrum.Tensor = self.stats.Mxy.sumOut([self.x_to])
    self.r2: pyagrum.Tensor = self.stats.Mx.sumOut([self.x_to])

    logging.debug("Qx\n%s", str(self.stats.Qx))
    logging.debug("Qx,y\n%s", str(self.stats.Qxy))
    logging.debug("Mxx\n%s", str(self.stats.Mxy))

    self.stats_added = True
    self.chiSqValue = None
    self._X: str = X
    self._Y: str = Y
    self._U: List[str] = U

  def testIndep(self, X: str, Y: str, U: List[str]) -> bool:
    """
    Parameters
    ----------
    X : str
        Name of the variable.
    Y : str
        Name of the variable to test independence from, not in ``U``.
    U : List[str]
        List of conditioning variables.

    Returns
    -------
    bool
        true if ``X`` is independent to ``Y`` given ``U``, otherwise false.
    """
    if not self.stats_added or self._X != X or self._Y != Y or set(self._U) != set(U):
      self.addVariables(X, Y, U)

    chi2_is_relevant: bool = len(self.stats.Mx.variable(CIM.varI(X)).labels()) >= 3

    if self.nullTimeToTransitionHypothesisF(X, Y, U) == REJECT:
      return False
    if chi2_is_relevant and not self.nullStateToStateTransitionHypothesisChi2(X, Y, U):
      return False

    return True

  def getMxxGivenU(self, M: pyagrum.Tensor, Y: str) -> "pyagrum.Tensor":
    """
    Parameters
    ----------
    M : pyagrum.Tensor
        A matrix M_{x, x' | y, U}, for some instantiation U of the conditioning set and y of a specific parent.
    Y : str
        A parent.

    Returns
    -------
    pyagrum.Tensor
        The tensor M_{x, x' | U} by summing over all values of y.
    """
    return M.sumOut([Y])

  def computeF(self) -> "pyagrum.Tensor":
    """
    Compute F-test value for every instance of the variables.

    Returns
    -------
    pyagrum.Tensor
        F-test value.
    """
    F: pyagrum.Tensor = self.stats.Qx / self.stats.Qxy
    return F

  def computeChi2(self) -> "pyagrum.Tensor":
    """
    Compute chi2-test value for every instance of the variables.

    Returns
    -------
    pyagrum.Tensor
        chi2-test value.
    """
    Ksq: pyagrum.Tensor = self.r2 / self.r1
    Lsq: pyagrum.Tensor = self.r1 / self.r2

    logging.debug("r1:\n%s", self.r1)
    logging.debug("r2:\n%s", self.r2)
    logging.debug("Ksq:\n%s", Ksq)
    logging.debug("Lsq:\n%s", Lsq)

    K = sqrtTensor(Ksq)
    L = sqrtTensor(Lsq)

    logging.debug("K:\n%s", K)
    logging.debug("L:\n%s", L)

    T: pyagrum.Tensor = ((K * self.stats.Mxy - L * self.stats.Mx).sq()) / (self.stats.Mxy + self.stats.Mx)

    logging.debug("T before:\n%s", T)
    i: pyagrum.Instantiation
    for i in T.loopIn():
      if i[self.x_from] == i[self.x_to]:
        T.set(i, 0)

    logging.debug("T:\n%s", T)

    self.chiSqValue: pyagrum.Tensor = T.sumOut(self.x_to)

    return self.chiSqValue

  def nullTimeToTransitionHypothesisF(self, X: str, Y: str, _: List[str]) -> bool:
    """
    Decides if the null time to transition hypothesis is rejected using F-test.

    Parameters
    ----------
    X : str
        A random variable.
    Y : str
        A parent of ``X``.
    _: List[str]
        A subset of the parents of ``X`` that does not contain ``Y``.

    Returns
    -------
    bool
        False if ``X`` is not independent of ``Y`` given the conditioning set ``U``.
    """
    logging.debug("PERFORMING F TEST")

    F: pyagrum.Tensor = self.computeF()

    logging.debug("F\n%s", F)

    rej = False
    i: pyagrum.Instantiation
    for i in F.loopIn():
      max_f = f.ppf(ALPHA, self.r1.get(i), self.r2.get(i))
      min_f = f.ppf(1 - ALPHA, self.r1.get(i), self.r2.get(i))
      logging.debug("\t%f, %f, %f", F.get(i), min_f, max_f)
      if not (min_f <= F.get(i) <= max_f):
        # Reject the hypothesis
        rej = True
        break

    if rej:
      return REJECT

    # Fail to reject the hypothesis
    return FAIL_TO_REJECT

  def nullStateToStateTransitionHypothesisChi2(self, X: str, Y: str,
                                               _: List[str]):
    """
    Decides if the null state to state transition hypothesis is rejected using chi2-test.

    Parameters
    ----------
    X : str
        A random variable.
    Y : str
        A parent of ``X``.
    _: List[str]
        A subset of the parents of ``X`` that does not contain ``Y``.

    Returns
    -------
    bool
        False if ``X`` is not independent of ``Y`` given the conditioning set ``U``.
    """
    logging.debug("PERFORMING CHI2 TEST")

    card_val_x = len(self.tr.ctbn.variable(X).labels())

    chiSqValue: pyagrum.Tensor = self.computeChi2()

    degrees_of_freedom: int = card_val_x - 1
    max_chi2 = chi2.ppf(ALPHA, degrees_of_freedom)

    logging.debug("max chi2: %f", max_chi2)
    logging.debug(chiSqValue)

    i: pyagrum.Instantiation
    for i in chiSqValue.loopIn():
      if chiSqValue.get(i) > max_chi2:
        logging.debug(i)
        logging.debug("\tRejecting the hypothesis: %s is NOT independent to %s given the conditioning set.", X,
                      Y)
        # X is NOT independent of Y
        return False

    logging.debug("Confirming the hypothesis: %s IS independent to %s given the conditioning set.", X, Y)
    return True
