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

import csv
import logging
from concurrent.futures import ProcessPoolExecutor
from typing import Dict, Tuple, List
from numpy.random import default_rng, choice
from scipy.linalg import expm

import pyagrum

from pyagrum.ctbn import CIM
from pyagrum.ctbn import CTBN
from pyagrum.ctbn.constants import NodeId, NameOrId

"""
This file contains inference tools for CTBNs
"""


class CTBNInference:
  """
  Class CtbnInference includes tools for inference calculation : exact inference through amalgamation
  and (forward) sampling inference.

  Parameters
  ----------
  model : CTBN
      The CTBN used for inference.
  """

  def __init__(self, model: CTBN):
    self._model = model

  def makeInference(self):
    raise NotImplementedError("Not yet implemented.")

  def posterior(self, name: str) -> "pyagrum.Tensor":
    raise NotImplementedError("Not yet implemented.")


class SimpleInference(CTBNInference):
  """
  Exact inference using amalgamation to compute the Intensity Matrix corresponding to the CTBN
  (not efficient for models with great number of variables)

  Parameters
  ----------
  ctbn : CTBN
      The CTBN used for simple inference.
  """

  def __init__(self, ctbn: CTBN):
    super().__init__(ctbn)
    self._joint = None

  def setEvidence(self, evs=None):
    raise NotImplementedError("Not yet implemented.")

  def makeInference(self, t: float = 5000):
    """
    Compute exact inference at time ``t``. Distribution for initial state is uniform.

    Parameters
    ----------
    t : float
        Time to make inference calculation at.
    """
    q = CIM()
    # amalgamation
    for nod in self._model.nodes():
      q = q.amalgamate(self._model.CIM(nod))

    q.fromMatrix(expm(t * q.toMatrix()))
    # initial state distribution
    t0 = pyagrum.Tensor()
    for n in q.varNames:
      if n[-1] == "i":
        t0.add(q._pot.variable(n))
    t0.fillWith(1).normalize()

    self._joint = (t0 * q._pot).sumOut(list(t0.names))

  def posterior(self, v: NameOrId) -> "pyagrum.Tensor":
    """
    Parameters
    ----------
    v : NameOrId
        Name or id of the variable.

    Returns
    -------
    pyagrum.Tensor
        The computed distribution of variable ``v`` using exact inference.
    """
    vj = CIM.varJ(self._model.variable(v).name())
    return pyagrum.Tensor().add(self._model.variable(v)).fillWith(self._joint.sumIn(vj), [vj])


class ForwardSamplingInference(CTBNInference):
  """
  Inference using forward sampling (slow convergence).

  Notes
  -----
  Sometimes samples are called trajectories. One sample is one trajectory.
  When making inference, the last sample is stored in the class as a trajectory.
  idtraj indicates the number of samplings done.

  Parameters
  ----------
  ctbn : CTBN
      The CTBN used for sampling.

  Attributes
  ----------
  idtraj : int
      Id of the sample.
  trajectory : List[Tuple[float, str, str]]
      Contains the trajectory from the last sampling.

  Examples
  --------
  How to read a trajectory?
  A tuple ``(t, var, s)`` means that at time ``t`` the variable ``var`` transition from state ``s`` to another one.

  When storing many trajectories from different samples, a trajectory is identified by an id.
  """

  def __init__(self, ctbn: CTBN):
    self.trajectory = list()
    self.idtraj = 0
    super().__init__(ctbn)
    self._posteriors = {nod: pyagrum.Tensor().add(self._model.variable(nod))
                        for nod in self._model.names()}

  def makeSample(self, posteriors: Dict[str, pyagrum.Tensor], timeHorizon: float = 5000, burnIn: int = 100) -> int:
    """
    Fills posteriors using forward sampling.

    Parameters
    ----------
    posteriors : Dict[str, pyagrum.Tensor]
        A dict containing a posterior for each variable of the CTBN.
    timeHorizon : float
        Duration of the sampling.
    burnIn : int
        Number of runs before starting the sampling (to ensure ergodicity).

    Returns
    -------
    int
        Number of runs.
    """

    def init(current):
      """
      Initializes posteriors and draw the initial values of each variable using uniform distribution.

      Parameters
      ----------
      current : pyagrum.Instantiation
          An instance of the variables of the CTBN.
      """
      for nam in self._model.names():
        posteriors[nam].fillWith(0)

      # initial uniform distribution for each variable
      for nod in self._model.nodes():
        v = self._model.variable(nod)
        newval = int(choice(range(v.domainSize())))
        current.chgVal(v.name(), newval)
        posteriors[v.name()][newval] = 1

    def getNextEvent(current: pyagrum.Instantiation, indice: pyagrum.Instantiation) -> Tuple[NodeId, float]:
      """
      Chooses the next variable to change value. The variable is chosen by drawing values of all of the variables transition
      time (i.e how much time does a variable stay in the same state). Those durations follow an exponential distribution.
      The variable with the shortest transition time is chosen.

      Parameters
      ----------
      current : pyagrum.Instantiation
          An instance of the variables in the CTBN.
      indice : pyagrum.Instantiation
          An instance of the variables in the CTBN plus their from_state/to_state corresponding variables.

      Returns
      -------
      Tuple[NodeId, float]
          The nodeId and transition time of the variable with the lowest transition time.
      """
      indice.setVals(current)
      # compute next variable event
      dt = None
      argmin = None
      for nod in self._model.nodes():
        v = self._model.variable(nod)
        indice.chgVal(CIM.varI(v.name()), indice.val(v))
        indice.chgVal(CIM.varJ(v.name()), indice.val(v))  # just the diagonal

        v_lambda = -self._model.CIM(nod)[indice]
        if v_lambda != 0:
          d = rand.exponential(1 / v_lambda)
          if dt is None or dt > d:
            dt = d
            argmin = nod

      return (argmin, dt)

    def sampleNextState(current: pyagrum.Instantiation, indice: pyagrum.Instantiation, nextEvt: NameOrId) -> str:
      """
      Draw the next state of the variable nextEvt using uniform distribution over its states.
      ``current`` will contain the new value of ``nextEvt``.

      Parameters
      ----------
      current : pyagrum.Instantiation
          An instantiation of the variables in the ctbn to contain the new value of nextEvt.
      indice : pyagrum.Instantiation
          An instance of the variables in the ctbn plus their from_state/to_state corresponding variables.
      nextEvt : NameOrId
          Name of id of the next variable to change state.

      Returns
      -------
      str
          Name of the variable.
      """
      v = self._model.variable(nextEvt)
      indice.chgVal(CIM.varI(v.name()), indice.val(v))
      indice.chgVal(CIM.varJ(v.name()), indice.val(v))  # just the diagonal

      somme = -self._model.CIM(nextEvt)[indice]
      choices = []
      probas = []
      for j in range(v.domainSize()):
        if j != indice.val(v):  # not the diagonal
          choices.append(j)
          indice.chgVal(CIM.varJ(v.name()), j)
          probas.append(self._model.CIM(nextEvt)[indice] / somme)

      newval = int(choice(choices, p=probas))
      current.chgVal(v.name(), newval)
      return v.name()

    # start
    current = self._model.completeInstantiation()
    rand = default_rng()
    nb_events = 0
    indice = self._model.fullInstantiation()
    init(current)  # initialize the starting value of each variable

    # BURNIN
    while nb_events < burnIn:
      # find next variable which will change its value
      nextEvt, dt = getNextEvent(current, indice)
      if nextEvt is None:  # if none of variables changes
        pass
      else:
        # find the value of the variable which will change its value and let this variable update his value
        sampleNextState(current, indice, nextEvt)
        nb_events += 1  # an event means a variable changes its value

    # SAMPLING
    duration = 0
    for name in self._model.names():
      current_state = self._model.variable(name).label(current.val(self._model.variable(name)))
      self.trajectory.append((0, name, current_state))
    while True:
      nextEvt, dt = getNextEvent(current, indice)

      # if none of variables changes before the timeHorizon (or just doesn't change at all)
      if duration + dt > timeHorizon or nextEvt is None:
        # update everyone for last event
        for name in self._model.names():
          posteriors[name][current] += timeHorizon - duration
          current_state = self._model.variable(name).label(current.val(self._model.variable(name)))
          self.trajectory.append((timeHorizon, name, current_state))
        break  # arriving at the timeHorizon, we quit the loop

      # update posteriors
      for name in self._model.names():
        posteriors[name][current] += dt
      duration += dt

      # add event to trajectory
      current_state = self._model.variable(nextEvt).label(current.val(self._model.variable(nextEvt)))
      self.trajectory.append((duration, self._model.name(nextEvt), current_state))

      sampleNextState(current, indice, nextEvt)
      nb_events += 1

    return nb_events

  def makeInference(self, timeHorizon: float = 5000, burnIn: int = 100) -> int:
    """
    Start a new sample and normalize resulting posteriors to get an aproximation of the inference.

    Parameters
    ----------
    timeHorizon : float
        Duration of the sampling.
    burnIn : int
        Number of runs before starting the sampling (to ensure ergodicity).

    Returns
    -------
    int
        Number of runs.
    """
    self.idtraj += 1
    self.trajectory = list()
    res = self.makeSample(self._posteriors, timeHorizon, burnIn)
    for nam in self._model.names():
      self._posteriors[nam].normalize()
    return res

  def makeParallelInference(self, nbTrajectories: int = 5, timeHorizon: float = 5000, burnIn: int = 100):
    """
    Start a given number of sample and approximate the inference over all the samples.

    Parameters
    ----------
    nbTrajectories : int
        Number of sampling in parallel.
    timeHorizon : float
        Duration of the sampling.
    burnIn : int
        Number of runs before starting the sampling (to ensure ergodicity).
    """
    posteriorsList = [{nod: pyagrum.Tensor().add(self._model.variable(nod))
                       for nod in self._model.names()} for _ in range(nbTrajectories)]

    def runMakeSample(task: int):
      res = self.makeSample(posteriorsList[task], timeHorizon, burnIn)
      return res

    with ProcessPoolExecutor(max_workers=1000) as executor:
      future = {executor.submit(runMakeSample, task) for task in range(nbTrajectories)}

    print([f.result() for f in future])

    for nam in self._model.names():
      self._posteriors[nam].fillWith(0)
      for i in range(nbTrajectories):
        self._posteriors[nam] += posteriorsList[i][nam]
      self._posteriors[nam].normalize()

  def averageInference(self, nbTrajectories: int = 5, timeHorizon: float = 5000, burnIn: int = 100):
    """
    Start ``nbTrajectories`` sampling and approximate the inference over all samples.

    Parameters
    ----------
    nbTrajectories: int
        Number of sampling in parallel.
    timeHorizon : float
        Duration of the sampling.
    burnIn : int
        Number of runs before starting the sampling (to ensure ergodicity).
    """
    posteriorsList = [{nod: pyagrum.Tensor().add(self._model.variable(nod))
                       for nod in self._model.names()} for _ in range(nbTrajectories)]

    for i in range(nbTrajectories):
      self.makeSample(posteriorsList[i], timeHorizon, burnIn)

    for nam in self._model.names():
      self._posteriors[nam].fillWith(0)
      for i in range(nbTrajectories):
        self._posteriors[nam] += posteriorsList[i][nam]
      self._posteriors[nam].normalize()

  def posterior(self, name: str) -> "pyagrum.Tensor":
    """
    Parameters
    ----------
    name : str
        Name of the variable.

    Returns
    -------
    pyagrum.Tensor
        The aproximate inference of a given variable.
    """
    p = pyagrum.Tensor(self._posteriors[name])
    p._model = self._model
    return p

  def writeTrajectoryCSV(self, filename: str, n: int = 1, timeHorizon=10, burnIn=100):
    """
    Makes ``n`` samples using Forward Sampling and saves trajectories into a csv file.
    Storing format : {IdSample, time, Var, state}

    Parameters
    ----------
    filename : str
        Name of the file to save trajectories in.
    n : int
        Number of sampling to run.
    timeHorizon : float
        Duration of the sampling.
    burnIn : int
        Number of preliminary iterations before starting.
    """
    data = dict()
    for i in range(n):
      self.makeInference(timeHorizon=timeHorizon, burnIn=burnIn)
      data[i] = self.trajectory

    with open(filename, 'w', newline='') as csvfile:
      fieldnames = ['IdSample', 'time', 'var', 'state']
      writer = csv.DictWriter(csvfile, fieldnames=fieldnames)

      writer.writeheader()
      for i in range(len(data)):
        for tr in data[i]:
          writer.writerow({'IdSample': str(i), 'time': str(tr[0]), 'var': tr[1], 'state': tr[2]})
