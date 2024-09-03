# (c) Copyright 2015-2024 by Pierre-Henri Wuillemin(@LIP6)
# (pierre-henri.wuillemin@lip6.fr)
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
from typing import Dict, Tuple, List, Union

import time
import logging
import csv
import math
import matplotlib.pyplot as plt
import pyAgrum

from pyAgrum.ctbn import CIM
from pyAgrum.ctbn.constants import INFINITY
from pyAgrum.ctbn import CTBN

"""
Contains Trajectory Class and functions to compute stats from samples and tools for trajectory plotting.
Also contains Stats Class to store stats that are used for independence tests.
"""


def readTrajectoryCSV(filename: str) -> Dict[int, List[Tuple[float, str, str]]]:
  """
  Reads trajectories from a csv file.
  Storing format : {IdSample, time, var, state}

  Parameters
  ----------
  filename : str
      Path to the file.

  Returns
  -------
  Dict[int, List[Tuple[float, str, str]]]
      The trajectories, a trajectory for every index.
  """
  data = dict()
  with open(filename, newline='') as csvfile:
    reader = csv.DictReader(csvfile)
    for row in reader:
      IdSample, time, var, state = (row['IdSample'], row['time'], row['var'], row['state'])
      IdSample = int(IdSample)
      if IdSample not in data.keys():
        data[IdSample] = list()
      data[IdSample].append((float(time), var, state))
  return data


def plotTrajectory(v: pyAgrum.DiscreteVariable, traj: List[Tuple[float, str, str]],
                   timeHorizon: float = None,
                   plotname: str = None):
  """
  Plot a variable's trajectory using matplotlib.pyplot.

  Parameters
  ----------
  v : pyAgrum.DiscreteVariable
      Variable to follow.
  traj : List[Tuple[float, str, str]]
      Trajectory to plot.
  timeHorizon : float
      Maximum time length to show.
  plotname : str
      Name of the plot.
  """
  if plotname is None:
    name = f"trajectory of {v.name()}"
  else:
    name = plotname

  if timeHorizon is None:
    Tlim = traj[-1][0]
  else:
    Tlim = timeHorizon

  XAxis = []
  YAxis = []

  prevtimepoint = 0
  for timepoint, var, state in traj:
    if timepoint == 0:
      continue
    if timepoint > Tlim:
      break
    if var == v.name():
      XAxis.append(prevtimepoint)
      XAxis.append(timepoint)
      YAxis.append(state)
      YAxis.append(state)
      prevtimepoint = timepoint

  plt.plot(XAxis, YAxis)
  plt.yticks(range(v.domainSize()), v.labels())
  plt.title(name)
  plt.xlabel("time")
  plt.ylabel("state")
  plt.show()


def plotFollowVar(v: pyAgrum.DiscreteVariable, trajectories: Dict[int, List[Tuple[float, str, str]]],
                  timeHorizon: float = None, N: int = None, plotname: str = None):
  """
  Plot the evolution (the proportions of the states the variable transition into) of a variable over time.

  Parameters
  ----------
  v : pyAgrum.DiscreteVariable
      The variable to follow.
  trajectories : Dict[int, List[Tuple[float, str, str]]]
      Contains trajectories. The result is a mean over all the trajectories.
  timeHorizon : float
      Maximum time length to plot. If None, then the entire time length of the trajectories is used.
  N : int
      Number of division of interval [0, timeHorizon].
  plotname : str
      Name of the plot. If None, the picked name is "Proportions for each state of {v.name()}".

  """
  if plotname is None:
    name = f"Proportions for each state of {v.name()}"
  else:
    name = plotname

  if timeHorizon is None:
    # timeHorizon is always found at the end of a trajectory.
    T = trajectories[0][-1][0]
  else:
    T = timeHorizon

  if N is None:
    h = 1
  else:
    h = timeHorizon / N

  n = len(trajectories.keys())
  domain = sorted(v.labels())
  division = [h * i for i in range(math.ceil(T // h) + 1)]
  count = {label: {h * i: 0 for i in range(math.ceil(T // h) + 1)} for label in domain}
  total = {h * i: 0 for i in range(math.ceil(T // h) + 1)}

  # Counting transitions
  for i in range(n):
    traj = trajectories[i]
    for time, var, state in traj:
      if var == v.name() and time > 0:
        step = math.ceil(time / h) * h
        # if a transition appears at time t, then we count it for all the next step(>t) until the end
        for i in range(len(division)):
          if division[i] >= step:
            count[state][division[i]] += 1
            total[division[i]] += 1

  # Compute average of all sums
  for label in domain:
    for step in division:
      if total[step] != 0:
        count[label][step] /= total[step]

  YAxisList = [[count[lab][step] for step in division] for lab in domain]

  _, ax = plt.subplots()
  plt.xlim(left=0, right=T)
  plt.ylim(top=1, bottom=0)

  plt.xlabel("time")
  plt.ylabel("state proportion")
  ax.stackplot(division, YAxisList)
  plt.title(name)
  plt.legend(domain)
  plt.show()


def CTBNFromData(data: Dict[int, List[Tuple[float, str, str]]]) -> CTBN:
  """
  Constructs a CTBN and add the corresponding variables found in the trajectories.

  Warning
  -------
  If data is too short, some variables or state labels might be missed.

  Parameters
  ----------
  data : Dict[int, List[Tuple[float, str, str]]]
      The trajectories used to look for variables.

  Returns
  -------
  CTBN
      The resulting CTBN.
  """
  names: Set[str] = set()
  labels: Dict[str, Set[str]] = dict()

  for i in range(len(data.keys())):
    for t, var, state in data[i]:
      names.add(var)
      if var not in labels.keys():
        labels[var] = set()
      labels[var].add(state)

  ctbn = CTBN()

  for name in names:
    var = pyAgrum.LabelizedVariable(name, name, sorted(list(labels[name])))
    ctbn.add(var)

  return ctbn


def computeCIMFromStats(X: str, M: pyAgrum.Potential, T: pyAgrum.Potential) -> "pyAgrum.Potential":
  """
  Computes a CIM (Conditional Intensity Matrix) using stats from a trajectory. Variables in the potential
  are not copied but directly used in the result to avoid memory issues.

  Parameters
  ----------
  X : str
      Name of the variable to compute CIM for.
  M : pyAgrum.Potential
      Potential containing the number of transitions for each pair of ``X``'s states.
  T : pyAgrum.Potential
      Potential containing the time spent to transition from every state of ``X``.

  Returns
  -------
  pyAgrum.Potential
      The resulting potential, ``X``'s CIM.
  """

  res: pyAgrum.Potential = pyAgrum.Potential(M)
  res.fillWith(0)

  i = pyAgrum.Instantiation(res)
  iTime = pyAgrum.Instantiation(T)

  i.setFirst()
  iTime.setFirst()

  posI = i.pos(res.variable(CIM.varI(X)))
  posJ = i.pos(res.variable(CIM.varJ(X)))

  while not i.end():
    iTime.setVals(i)

    iSum = pyAgrum.Instantiation(i)
    sumCIM = 0
    iSum.setFirstVar(i.variable(posJ))
    while not iSum.end():
      sumCIM += M.get(iSum)
      iSum.incVar(i.variable(posJ))

    if i.val(posI) == i.val(posJ):
      if T.get(iTime) != 0:
        res.set(i, -round(sumCIM / T.get(iTime), 3))
    else:
      if T.get(iTime) != 0:
        res.set(i, round((M.get(i) / T.get(iTime)), 3))
    i.inc()

  return res


class Trajectory:
  """
  Tools to extract useful informations from a trajectory. It is used for parameters/graph learning.
  It can be created from a trajectory (a dict of trajectories) or from a file that contains one.

  Parameters
  ----------
  source : str|Dict[int, List[Tuple[float, str, str]]]
      The path to a csv file containing the samples or the dict of trajectories itself.
  ctbn : CTBN
      To link the variables's name in the trajectory to their pyAgrum variable. If not given, a new CTBN is created
      with the variables and labels found in the trajectory. (warning : if the trajectory is short, all of the variables
      may not be found correctly).

  Attributes
  ----------
  data : Dict[int, List[Tuple[float, str, str]]]
      The samples.
  ctbn : CTBN
      The CTBN used to link the names in the trajectory to pyAgrum variables.
  timeHorizon : float
      The time length of the trajectory.
  """

  def __init__(self, source, ctbn: CTBN = None):
    if isinstance(source, str):
      self.data = readTrajectoryCSV(source)
    else:
      self.data = source

    if ctbn is None:
      self.ctbn = CTBNFromData(self.data)
    else:
      self.ctbn = ctbn
    # to assert
    self.timeHorizon = self.data[0][-1][0]

  def setStatValues(self, X: str, inst_u: Dict[str, str], Txu: pyAgrum.Potential, Mxu: pyAgrum.Potential):
    """
    Fills the potentials given.

    Parameters
    ----------
    X : str
        Name of the variable.
    inst_u : Dict[str, str]
        Instance of conditioning variables.
    Txu : pyAgrum.Potential
        Potential to fill. Contains the time spent in each state.
    Mxu : pyAgrum.Potential
        Potential to fill. Contains the number of transitions from any pair of states.
    """

    def checkAllValues(curr, goal):
      for e in curr.keys():
        if curr[e] != goal[e]:
          return False
      return True

    def findNextValue(X, traj, start):
      for i in range(start + 1, len(traj)):
        if traj[i][1] == X:
          return traj[i][2]

    X_i = CIM.varI(X)
    X_j = CIM.varJ(X)
    data = self.data
    for i in range(len(data)):
      traj = data[i]
      prev_duration = 0
      duration = 0

      # store current values at duration
      u_values = dict()
      X_value = None

      # init (find initial values)
      for l in range(len(traj)):
        time, var, state = traj[l]
        if time != 0:
          break
        if var == X:
          X_value = state
        elif var in inst_u.keys():
          u_values[var] = state

      # loop over the transitions
      for l in range(len(traj)):
        time, var, state = traj[l]
        if time == 0:
          continue

        prev_duration = duration
        duration = time

        ########## begin check
        if checkAllValues(u_values, inst_u):
          # update T
          inst1 = u_values.copy()
          inst1[X_i] = X_value
          Txu[inst1] += duration - prev_duration

          # update M
          if time < self.timeHorizon and var == X:
            nextv = findNextValue(X, traj, l)
            if nextv != X_value:
              inst2 = u_values.copy()
              inst2[X_i] = X_value
              inst2[X_j] = nextv
              Mxu[inst2] += 1
        ########## end check

        if time < self.timeHorizon and var == X:
          X_value = findNextValue(X, traj, l)
        elif time < self.timeHorizon and var in inst_u.keys():
          u_values[var] = findNextValue(var, traj, l)

  def computeStats(self, X: str, U: List[str]) -> Tuple[pyAgrum.Potential, pyAgrum.Potential]:
    """
    Computes time spent and number of transitions values of ``X`` and returns them as ``pyAgrum.Potential``.

    Parameters
    ----------
    X : str
        Name of the variable.
    U : List[str]
        List of conditioning variable's name.

    Returns
    -------
    Tuple[pyAgrum.Potential, pyAgrum.Potential]
        The resulting potentials.
    """
    par = [self.ctbn.variable(nv) for nv in U]
    n = len(self.data)

    Txu = pyAgrum.Potential()
    Mxu = pyAgrum.Potential()
    current = pyAgrum.Instantiation()

    X_from = self.ctbn.CIM(X).findVar(CIM.varI(X))
    X_to = self.ctbn.CIM(X).findVar(CIM.varJ(X))

    Txu.add(X_from)
    Mxu.add(X_from)
    Mxu.add(X_to)
    for v in par:
      current.add(v)
      Txu.add(v)
      Mxu.add(v)

    Txu.fillWith(0)
    Mxu.fillWith(0)

    # looping over all possible instances
    current.setFirst()
    while not current.end():
      inst_par = {vname: current.todict(withLabels=True)[vname] for vname in U}
      self.setStatValues(X, inst_par, Txu, Mxu)
      current.inc()

    Mxu = Mxu.putFirst(X_to.name())
    return (Txu / n, Mxu / n)

  def computeAllCIMs(self):
    """
    Computes the CIMs of the variables in ``self.ctbn``. Conditioning is given by the graph of ``self.ctbn``.
    """
    for var in self.ctbn.variables():
      T, M = self.computeStats(var.name(), self.ctbn.parentNames(var.name()))
      self.ctbn.CIM(var.name())._pot = pyAgrum.Potential(
        computeCIMFromStats(var.name(), M.putFirst(CIM.varJ(var.name())), T))

  def setStatsForTests(self, X: str, Y: str, inst_u: Dict[str, str], Txu: pyAgrum.Potential, Txyu: pyAgrum.Potential,
                       Mxyu: pyAgrum.Potential):
    """
    Fills the potentials given. They are used for independence testing.

    Parameters
    ----------
    X : str
        Name of the variable.
    Y : str
        Name of a conditioning variable.
    inst_u : Dict[str, str]
        Instance of conditioning variables.
    Txu : pyAgrum.Potential
        Potential to fill. Contains the time spent in each state. Conditioned by variables in ``inst_u``.
    Txyu : pyAgrum.Potential
        Potential to fill. Contains the time spent in each state. Conditioned by ``Y`` and variables in ``inst_u``.
    Mxyu : pyAgrum.Potential
        Potential to fill. Contains the number of transitions from any pair of states. Conditioned by ``Y`` and variables in ``inst_u``.
    """

    def checkAllValues(curr, goal):
      for e in curr.keys():
        if curr[e] != goal[e]:
          return False
      return True

    def findNextValue(X, traj, start):
      for i in range(start + 1, len(traj)):
        if traj[i][1] == X:
          return traj[i][2]

    X_i = CIM.varI(X)
    X_j = CIM.varJ(X)
    data = self.data
    for i in range(len(data)):
      traj = data[i]
      prev_duration = 0
      duration = 0

      # store current values at duration
      u_values = dict()
      X_value = None
      Y_value = None

      # init (find initial values)
      for l in range(len(traj)):
        time, var, state = traj[l]
        if time != 0:
          break
        if var == X:
          X_value = state
        elif var == Y:
          Y_value = state
        elif var in inst_u.keys():
          u_values[var] = state

      # loop over the transitions
      for l in range(len(traj)):
        time, var, state = traj[l]
        if time == 0:
          continue

        prev_duration = duration
        duration = time

        ########## begin check
        if checkAllValues(u_values, inst_u):
          # update T
          inst1 = u_values.copy()
          inst1[X_i] = X_value
          Txu[inst1] += duration - prev_duration

          inst2 = inst1.copy()
          inst2[Y] = Y_value
          Txyu[inst2] += duration - prev_duration

          # update M
          if time < self.timeHorizon and var == X:
            nextv = findNextValue(X, traj, l)
            if nextv != X_value:
              inst3 = u_values.copy()
              inst3[X_i] = X_value
              inst3[X_j] = nextv
              inst3[Y] = Y_value
              Mxyu[inst3] += 1
        ########## end check

        if time < self.timeHorizon and var == X:
          X_value = findNextValue(X, traj, l)
        elif time < self.timeHorizon and var == Y:
          Y_value = findNextValue(Y, traj, l)
        elif time < self.timeHorizon and var in inst_u.keys():
          u_values[var] = findNextValue(var, traj, l)

  def computeStatsForTests(self, X: str, Y: str, U: List[str]) -> Tuple[
    pyAgrum.Potential, pyAgrum.Potential, pyAgrum.Potential]:
    """
    Computes time spent and number of transitions values of ``X`` when conditioned by ``Y`` and ``U`` and
    returns them as ``pyAgrum.Potential``. Used for independence testing.

    Parameters
    ----------
    X : str
        Name of the variable.
    Y : str
        Name of a conditioning variable not in ``U``.
    U : List[str]
        List of conditioning variable's name.

    Returns
    -------
    Tuple[pyAgrum.Potential, pyAgrum.Potential, pyAgrum.Potential]
        The resulting potentials.
    """
    par = [self.ctbn.variable(nv) for nv in U]
    n = len(self.data)

    Txu = pyAgrum.Potential()
    Txyu = pyAgrum.Potential()
    Mxyu = pyAgrum.Potential()
    current = pyAgrum.Instantiation()

    X_from = self.ctbn.CIM(X).findVar(CIM.varI(X))
    X_to = self.ctbn.CIM(X).findVar(CIM.varJ(X))
    varY = self.ctbn.variable(Y)

    Txu.add(X_from)
    Txyu.add(X_from)
    Txyu.add(varY)
    Mxyu.add(varY)
    Mxyu.add(X_from)
    Mxyu.add(X_to)

    for v in par:
      current.add(v)
      Txu.add(v)
      Txyu.add(v)
      Mxyu.add(v)

    Txu.fillWith(0)
    Txyu.fillWith(0)
    Mxyu.fillWith(0)

    # looping over all possible instances
    current.setFirst()
    while not current.end():
      inst_par = {vname: current.todict(withLabels=True)[vname] for vname in U}
      self.setStatsForTests(X, Y, inst_par, Txu, Txyu, Mxyu)
      current.inc()

    Txu.putFirst(X_from.name())
    Txyu.putFirst(X_from.name())
    Mxyu = Mxyu.putFirst(X_to.name())
    return (Txu / n, Txyu / n, Mxyu / n)


class Stats:
  """
  Stores all potentials used for learning.

  Parameters
  ----------
  trajectory : Trajectory
      Samples used to find stats.
  X : str
      Name of the variable to study.
  Y : str
      Name of the variable used for conditioning variable ``X``.
  par : List[str]
      List of conditioning variables of ``X``.

  Attributes
  ----------
  Mxy : pyAgrum.Potential
      Potential containing the number of transitions the variable ``X`` does from any
      of its states for any instance of its parents and variable``Y``.
  Mx : pyAgrum.Potential
      Potential containing the number of transitions the variable ``X`` does from any
      of its states for any instance of its parents.
  Tx : pyAgrum.Potential
      Potential containing the time spent by ``X`` to transition from a state to another for any instance of its parents.
  Txy : pyAgrum.Potential
      Potential containing the time spent by ``X`` to transition from a state to another for any instance of its parents and of ``Y``.
  Qx : pyAgrum.Potential
      Conditional Intensity Matrix(CIM) of ``X``.
  QxY : pyAgrum.Potential
      Conditional Intensity Matrix(CIM) of ``X`` that includes the conditioning variable ``Y``.
  """

  def __init__(self, trajectory: Trajectory, X: str, Y: str, par: List[str]):
    self.Tx, self.Txy, self.Mxy = trajectory.computeStatsForTests(X, Y, par)
    self.Mx = pyAgrum.Potential(self.Mxy).sumOut([Y])
    self.Qx = computeCIMFromStats(X, self.Mx, self.Tx)
    self.Qxy = computeCIMFromStats(X, self.Mxy, self.Txy)
