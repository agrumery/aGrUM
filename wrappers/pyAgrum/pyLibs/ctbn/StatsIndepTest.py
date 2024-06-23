from abc import abstractmethod
from typing import List
from scipy.stats import f, chi2
import logging
import math

import pyAgrum

from pyAgrum.ctbn import CIM
from pyAgrum.ctbn import CTBN
from pyAgrum.ctbn.constants import ALPHA, FAIL_TO_REJECT, REJECT
from pyAgrum.ctbn import Trajectory, Stats


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


def sqrtPotential(potential: pyAgrum.Potential) -> "pyAgrum.Potential":
  """
  Applies sqrt function to all values inside the potential.

  Parameters
  ----------
  potential : pyAgrum.Potential
      potential to play sqrt to.

  Returns
  -------
  pyAgrum.Potential
      sqrt of potential.
  """
  res = pyAgrum.Potential(potential)
  i: pyAgrum.Instantiation = pyAgrum.Instantiation(res)
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

    self.r1: pyAgrum.Potential = self.stats.Mxy.sumOut([self.x_to])
    self.r2: pyAgrum.Potential = self.stats.Mx.sumOut([self.x_to])

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

  def getMxxGivenU(self, M: pyAgrum.Potential, Y: str) -> "pyAgrum.Potential":
    """
    Parameters
    ----------
    M : pyAgrum.Potential
        A matrix M_{x, x' | y, U}, for some instantiation U of the conditioning set and y of a specific parent.
    Y : str
        A parent.

    Returns
    -------
    pyAgrum.Potential
        The potential M_{x, x' | U} by summing over all values of y.
    """
    return M.sumOut([Y])

  def computeF(self) -> "pyAgrum.Potential":
    """
    Compute F-test value for every instance of the variables.

    Returns
    -------
    pyAgrum.Potential
        F-test value.
    """
    F: pyAgrum.Potential = self.stats.Qx / self.stats.Qxy
    return F

  def computeChi2(self) -> "pyAgrum.Potential":
    """
    Compute chi2-test value for every instance of the variables.

    Returns
    -------
    pyAgrum.Potential
        chi2-test value.
    """
    Ksq: pyAgrum.Potential = self.r2 / self.r1
    Lsq: pyAgrum.Potential = self.r1 / self.r2

    logging.debug("r1:\n%s", self.r1)
    logging.debug("r2:\n%s", self.r2)
    logging.debug("Ksq:\n%s", Ksq)
    logging.debug("Lsq:\n%s", Lsq)

    K = sqrtPotential(Ksq)
    L = sqrtPotential(Lsq)

    logging.debug("K:\n%s", K)
    logging.debug("L:\n%s", L)

    T: pyAgrum.Potential = ((K * self.stats.Mxy - L * self.stats.Mx).sq()) / (self.stats.Mxy + self.stats.Mx)

    logging.debug("T before:\n%s", T)
    i: pyAgrum.Instantiation
    for i in T.loopIn():
      if i[self.x_from] == i[self.x_to]:
        T.set(i, 0)

    logging.debug("T:\n%s", T)

    self.chiSqValue: pyAgrum.Potential = T.sumOut(self.x_to)

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

    F: pyAgrum.Potential = self.computeF()

    logging.debug("F\n%s", F)

    rej = False
    i: pyAgrum.Instantiation
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

    chiSqValue: pyAgrum.Potential = self.computeChi2()

    degrees_of_freedom: int = card_val_x - 1
    max_chi2 = chi2.ppf(ALPHA, degrees_of_freedom)

    logging.debug("max chi2: %f", max_chi2)
    logging.debug(chiSqValue)

    i: pyAgrum.Instantiation
    for i in chiSqValue.loopIn():
      if chiSqValue.get(i) > max_chi2:
        logging.debug(i)
        logging.debug("\tRejecting the hypothesis: %s is NOT independent to %s given the conditioning set.", X,
                      Y)
        # X is NOT independent of Y
        return False

    logging.debug("Confirming the hypothesis: %s IS independent to %s given the conditioning set.", X, Y)
    return True
