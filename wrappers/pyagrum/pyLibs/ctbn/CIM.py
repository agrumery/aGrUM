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

from typing import Dict, Optional, Union, List

import numpy
import pyagrum

"""
This file contains the CIM class (Conditional Intensity Matrix).
"""


class CIM:
  """
  This class is used to represent a CIM (random variables and parameters of transition time).
  A CIM is mainly a pyagrum.Tensor that contains the parameters of an exponential distribution.
  This class contains also contains the ``amalgamation`` operator, used for merging CIMs into one.

  Notes
  -----
  A static string ``DELIMITER`` is used to differentiate between the current state of the variable and its next state.
  <V_name><DELIMITER>i : starting node
  <V_name><DELIMITER>j : ending node

  Parameters
  ----------
  pot : pyagrum.Tensor | None
      Defines the new CIM using existing tensor.
  """
  DELIMITER = "#"
  _pot: pyagrum.Tensor

  def __init__(self, pot=None):
    if pot is None:
      self._pot = pyagrum.Tensor()
    else:
      self._pot = pyagrum.Tensor(pot)
    self._recordVars()

  def __getitem__(self, i):
    return self._pot[i]

  def __setitem__(self, i, v):
    self._pot[i] = v

  def __mul__(self: "CIM", cimY: "CIM") -> "CIM":
    return self.amalgamate(cimY)

  def __repr__(self):
    return self._pot.__repr__()

  def __str__(self):
    return self.__repr__()

  def add(self, v: pyagrum.DiscreteVariable) -> "CIM":
    """
    Add a new variable to the CIM.

    Parameters
    ----------
    v : pyagrum.DiscreteVariable
        Variable to add.

    Returns
    -------
    CIM
        A reference to the CIM.
    """
    if not self._pot.contains(v):
      self._pot.add(v)
    self._recordVars()
    return self

  def remove(self, v: pyagrum.DiscreteVariable) -> "CIM":
    """
    Removes a variable from the CIM. Only for parent variables.

    Parameters
    ----------
    v : pyagrum.DiscreteVariable
        Variable to remove.

    Returns
    -------
    CIM
        A reference to the CIM.

    Raises
    ------
    pyagrum.NotFound
        If the variable isn't in the CIM.
    pyagrum.InvalidArgument
        If the variable isn't a parent in the CIM.
    """
    if v not in self.variablesSequence():
      raise pyagrum.NotFound("the variable is not in the CIM")
    elif not self.isParent(v):
      raise pyagrum.InvalidArgument("the variable is not a parent in the CIM")
    else:
      self._pot.remove(v)
    self._recordVars()
    return self

  def nbrDim(self) -> int:
    """
    Returns
    -------
    int
        The number of variables in the CIM (including v_i and v_j variables).

    """
    return self._pot.nbrDim()

  def extract(self, ctxt: Optional[Dict[str, str]]) -> "CIM":
    """
    Creates a new CIM extracted from the current CIM using the instantiation ``ctxt``.

    Parameters
    ----------
    ctxt : Optional[Dict[str, str]]
        Instantiation of variables.

    Returns
    -------
    CIM
        The extracted CIM.
    """
    return CIM(self._pot.extract(ctxt))

  def asTensor(self):
    """

    Returns
    -------
    pyagrum.Tensor
      a copy of the internal tensor
    """
    return pyagrum.Tensor(self._pot)

  @property
  def varNames(self):
    """
    Returns
    -------
    List[str]
        A list containing the name of each variable in the CIM.
    """
    # return self._pot.varNames
    return self._pot.names

  def variablesSequence(self) -> List[pyagrum.DiscreteVariable]:
    """
    Returns
    -------
    List[pyagrum.DiscreteVariable]
        A list containing the sequence of variables of the CIM.
    """
    return self._pot.variablesSequence()

  def variable(self, arg: int) -> "pyagrum.DiscreteVariable":
    """
    Parameters
    ----------
    arg : int
        Index of the variable in the CIM (i.e its node id).

    Returns
    -------
    pyagrum.DiscreteVariable
        The variable at index ``arg``.
    """
    return self._pot.variable(arg)

  def findVar(self, name: str) -> Union[None, pyagrum.DiscreteVariable]:
    """
    Finds a variable in the CIM using its name.

    Parameters
    ----------
    name : str
        Name of the variable to find.

    Returns
    -------
    pyagrum.DiscreteVariable | None
        The variable if it is in the CIM.
    """
    v: pyagrum.DiscreteVariable
    for v in self._pot.variablesSequence():
      if v.name() == name:
        return v
    return None

  def instantiation(self):
    """
    Returns
    -------
    pyagrum.Instantiation
        An Instantiation object using the CIM's tensor.
    """
    return pyagrum.Instantiation(self._pot)

  def _recordVars(self):
    self._parents = set()
    self._bases = set()
    for v in self.variablesSequence():
      if self.isParent(v):
        self._parents.add(v)
      else:
        self._bases.add(v)

  @staticmethod
  def varI(name: str) -> str:
    """
    Parameters
    ----------
    name : str
        Name of the variable to format.

    Returns
    -------
    str
        The name of the variable using {name}{DELIMITER}i format.
    """
    return f"{name}{CIM.DELIMITER}i"

  @staticmethod
  def varJ(name: str) -> str:
    """
    Parameters
    ----------
    name : str
        Name of the variable to format.

    Returns
    -------
    str
        The name of the variable using {name}{DELIMITER}j format.
    """
    return f"{name}{CIM.DELIMITER}j"

  @staticmethod
  def varRadical(v: pyagrum.DiscreteVariable) -> str:
    """
    Parameters
    ----------
    v : pyagrum.DiscreteVariable
        Variable to get name from.

    Returns
    -------
    str
        The name of the variable (without ``DELIMITER`` formatting).
    """
    if CIM.isParent(v):
      return v.name()
    return v.name()[:-2]

  @staticmethod
  def isParent(v: pyagrum.DiscreteVariable) -> bool:
    """
    Uses the syntax convention to check if a variable's name is the name of a parent in the CIM.

    Parameters
    ----------
    v : pyagrum.DiscreteVariable
        Variable to check.

    Returns
    -------
    bool
        True if ``v``'s name corresponds to the name of a parent in the CIM.
    """
    name = v.name()
    if len(name) < 2:
      return True
    if name[-2] != CIM.DELIMITER:
      return True
    return False

  def isIM(self) -> bool:
    """
    Returns
    -------
    bool
        True if there is no conditioning variable (parent) in the CIM.
    """
    for v in self.variablesSequence():
      if self.isParent(v):
        return False
    return True

  def toMatrix(self, ctxt: Optional[Dict[str, str]] = None) -> numpy.array:
    """
    Converts a CIM to a numpy.array matrix.

    Notes
    -----
    Only works for CIMs with no conditioning variable, or CIMs obtained through amalgamation.

    Parameters
    ----------
    ctxt : Optional[Dict[str, str]] | None
        Instantiation of variables to use if given.

    Returns
    ------
    numpy.array
        The CIM as a numpy.array.
    """
    if ctxt is not None:
      q = self.extract(ctxt)
    else:
      q = self
    if not q.isIM():
      raise ValueError("The cim is conditionnal.")
    i = q.instantiation()
    iI = pyagrum.Instantiation()
    iJ = pyagrum.Instantiation()
    for n in sorted(q.varNames):  # to be sure of a deterministic order
      v = i.variable(n)
      if n[-1] == "i":
        iI.add(v)
      else:
        iJ.add(v)

    res = []
    iI.setFirst()
    while not iI.end():
      iJ.setFirst()
      line = []
      while not iJ.end():
        i.setVals(iI)
        i.setVals(iJ)
        line.append(q[i])
        iJ.inc()
      res.append(list(line))
      iI.inc()

    return numpy.array(res)

  def fromMatrix(self, mat):
    """
    Fills the CIM with matrix ``mat``.

    Notes
    -----
    Only works for square-shaped matrixes, if the CIM is not conditional.

    Parameters
    ----------
    mat : numpy.array
        Matrix to convert into a CIM.
    """
    if not self.isIM():
      raise ValueError("The cim is conditionnal.")
    i = self.instantiation()
    iI = pyagrum.Instantiation()
    iJ = pyagrum.Instantiation()
    siz = 1
    for n in sorted(self.varNames):  # to be sure of a deterministic order
      v = i.variable(n)
      if n[-1] == "i":
        iI.add(v)
        siz *= v.domainSize()
      else:
        iJ.add(v)
    if mat.shape != (siz, siz):
      raise AttributeError(f"Shape {mat.shape} should be {(siz, siz)}")

    iI.setFirst()
    for lin in mat:
      iJ.setFirst()
      for v in lin:
        i.setVals(iI)
        i.setVals(iJ)
        self[i] = v
        iJ.inc()
      iI.inc()

  def amalgamate(self: "CIM", cimY: "CIM") -> "CIM":
    """
    Amalgamation of 2 CIMs, i.e combine 2 CIMs into one. When manipulating CIMs ``*`` can be used instead.

    Parameters
    ----------
    cimY : CIM
        CIM to amalgamate self with.

    Returns
    -------
    CIM
        Amalgamation of the CIMs.
    """
    cimX = self

    # Checks if either one of the CIM is empty
    if cimX.nbrDim() == 0:
      return CIM(pyagrum.Tensor(cimY._pot))
    if cimY.nbrDim() == 0:
      return CIM(pyagrum.Tensor(cimX._pot))

    # Names of the non-parent variables from the CIM
    sX = {self.varRadical(v) for v in cimX.variablesSequence()
          if not self.isParent(v)}
    sY = {self.varRadical(v) for v in cimY.variablesSequence()
          if not self.isParent(v)}

    amal = CIM()

    for v in cimX._bases.union(cimY._bases):
      amal.add(v)

    # parents of cimY that are bases from cimX
    pXinY = set()
    # parents of cimX that are bases from cimY
    pYinX = set()

    for v in cimX._parents:
      if v.name() not in sY:
        amal.add(v)
      else:
        pYinX.add(v)

    for v in cimY._parents:
      if v.name() not in sX:
        amal.add(v)
      else:
        pXinY.add(v)

    amal._recordVars()

    i = amal.instantiation()
    iX = cimX.instantiation()
    iY = cimY.instantiation()
    # goes over all possible combinations of values of variables and their parents
    i.setFirst()
    while not i.end():
      iX.setVals(i)
      iY.setVals(i)
      for v in pXinY:
        iY.chgVal(v, iX[CIM.varI(v.name())])
      for v in pYinX:
        iX.chgVal(v, iY[CIM.varI(v.name())])
      dX = True
      for v in sX:
        # checks if the variable v from sX changes value over the transition
        if iX[CIM.varI(v)] != iX[CIM.varJ(v)]:
          dX = False
          break
      dY = True
      for v in sY:
        if iY[CIM.varI(v)] != iY[CIM.varJ(v)]:
          dY = False
          break
      if dX and dY:  # case where both states don't change
        amal[i] = cimX[iX] + cimY[iY]
      elif dY:  # case were one of iX variables changes value but none of iY does
        amal[i] = cimX[iX]
      elif dX:  # case were one of iY variables changes value but none of iX does
        amal[i] = cimY[iY]
      else:  # case were 2 variables from iX and iY change value at the sime time
        amal[i] = 0

      i.inc()  # next combination of variables

    return amal

  def getTensor(self) -> "pyagrum.Tensor":
    """
    Returns
    -------
    pyagrum.Tensor
        A copy of the CIM's tensor
    """
    return pyagrum.Tensor(self._pot)
