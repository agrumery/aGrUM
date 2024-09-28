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
from typing import Dict, Tuple, List, Set

import pyAgrum

from pyAgrum.ctbn import CIM
from pyAgrum.ctbn.constants import NodeId, NameOrId

import pyAgrum.ctbn

"""
This file contains the CTBN class.
CTBN : Continous-Time Bayesian Network
"""


class CTBN:
  """
  This class is used to represent a CTBN.
  A CTBN is : a set of random variables, a CIM for each one of them and a pyAgrum.DiGraph to represent dependency relations.

  Attributes
  ----------
  _graph : pyAgrum.DiGraph
      Graph representing dependency relations between variables. Also used to link a variable with an id.
  _cim : Dict[NodeId, CIM]
      Dict containing a CIM for each nodeId(the integer given to a variable).
  _id2var : Dict[NodeId, pyAgrum.DiscreteVariable]
      Dict containing the variable associated to a node id.
  _name2id : Dict[str, NodeId]
      Dict containing the nodeId associated to a variable's name.
  """
  _graph: pyAgrum.DiGraph
  _cim: Dict[NodeId, CIM]
  _id2var: Dict[NodeId, pyAgrum.DiscreteVariable]
  _name2id: Dict[str, NodeId]

  def __init__(self):
    self._graph = pyAgrum.DiGraph()
    self._cim = {}
    self._id2var = {}
    self._name2id = {}

  def add(self, var: pyAgrum.DiscreteVariable) -> NodeId:
    """
    Add a new variable to the Ctbn.

    Parameters
    ----------
    var : pyAgrum.DiscreteVariable
        The variable to add to the CTBN.

    Returns
    -------
    NodeId
        The id given to the variable.

    Raises
    ------
    NameError
        If a variable with the same name already exists.
    ValueError
        If the variable is None.
    """
    if var is None:
      raise ValueError("The var cannot be None.")
    if var.name() == "" or not CIM.isParent(var):
      raise NameError(f"The name '{var.name()}' is not correct.")
    if var.name() in self._name2id:
      raise NameError(f"A variable with the same name ({var.name()}) already exists in this CTBN.")

    # link variable to its name and NodeId
    n = NodeId(self._graph.addNode())
    self._id2var[n] = var
    self._name2id[var.name()] = n

    # add leaving and starting states in the CIM
    v_i = var.clone()
    v_i.setName(CIM.varI(var.name()))
    v_j = var.clone()
    v_j.setName(CIM.varJ(var.name()))
    self._cim[n] = CIM().add(v_j).add(v_i)

    return n

  def _nameOrId(self, val: NameOrId) -> NodeId:
    """
    Returns
    -------
    NodeId
        The id of a variable.

    Raises
    ------
    pyAgrum.NotFound
        If the variable's name isn't in the CTBN.
    """
    if isinstance(val, int):
      if val not in self._id2var.keys():
        raise pyAgrum.NotFound("the variable isn't in the ctbn")
      return val
    else:
      if val not in self.names():
        raise pyAgrum.NotFound("the variable isn't in the ctbn")
      return self._name2id[val]

  def addArc(self, val1: NameOrId, val2: NameOrId) -> Tuple[NodeId, NodeId]:
    """
    Adds an arc ``val1`` -> ``val2``.

    Parameters
    ----------
    val1 : NameOrId
        The name or id of the first variable.
    val2 : NameOrId
        The name or id of the second variable.

    Returns
    -------
    Tuple[NodeId, NodeId]
        The created arc (``val1``, ``val2``).

    Raises
    ------
    pyAgrum.NotFound
        If one the variables is not in the CTBN.
    """
    n1 = self._nameOrId(val1)
    n2 = self._nameOrId(val2)
    self._graph.addArc(n1, n2)

    # adding n1 as a parent of n2 in the CIM
    self._cim[n2].add(self._id2var[n1])

    return (n1, n2)

  def eraseArc(self, val1: NameOrId, val2: NameOrId):
    """
    Erases an arc from the graph.

    Parameters
    ----------
    val1 : NameOrId
        The name or id of the first variable.
    val2 : NameOrId
        The name or id of the second variable.

    Raises
    ------
    pyAgrum.NotFound
        If a variable isn't in the CIM.
    pyAgrum.InvalidArgument
        If a variable isn't a parent in the CIM.
    """
    n1 = self._nameOrId(val1)
    n2 = self._nameOrId(val2)
    self._graph.eraseArc(n1, n2)
    self._cim[n2].remove(self._id2var[n1])

  def name(self, node: NodeId) -> str:
    """
    Parameters
    ----------
    node : NodeId
        The id of the variable.

    Returns
    -------
    str
        The variable's name linked to the NodeId.

    Raises
    ------
    pyAgrum.NotFound
        If the variable is not found in the CTBN.
    """
    if node not in self._id2var.keys():
      raise pyAgrum.NotFound("The node isn't in the ctbn")
    return self._id2var[node].name()

  def node(self, name: str) -> NodeId:
    """
    Parameters
    ----------
    name : str
        The name of the variable.

    Returns
    -------
    NodeId
        The id of the variable.

    Raises
    ------
    pyAgrum.NotFound
        If the variable is not found in the CTBN.
    """
    if name not in self.names():
      raise pyAgrum.NotFound("the variable isn't in the ctbn")
    return self._name2id[name]

  def labels(self, val: NameOrId) -> tuple:
    """
    Parameters
    ----------
    val : NameOrId
        The name or id of the variable.

    Returns
    -------
    tuple
        A tuple containing the labels of the variable.

    Raises
    ------
    pyAgrum.NotFound
        If the variable is not found in the CTBN.
    """
    return self._id2var[self._nameOrId(val)].labels()

  def variable(self, val: NameOrId) -> "pyAgrum.DiscreteVariable":
    """
    Parameters
    ----------
    val : NameOrId
        The name or id of the variable.

    Returns
    -------
    pyAgrum.DiscreteVariable
        The corresponding variable.

    Raises
    ------
    pyAgrum.NotFound
        If the variable is not found in the CTBN.
    """
    return self._id2var[self._nameOrId(val)]

  def variables(self) -> List[pyAgrum.DiscreteVariable]:
    """
    Returns
    -------
    List[pyAgrum.DiscreteVariable]
        The list of variables in the CTBN.
    """
    return [self.variable(i) for i in self.nodes()]

  def nodes(self) -> List[NodeId]:
    """
    Returns
    -------
    List[NodeId]
        The list of variables id in the CTBN.
    """
    return list(self._id2var.keys())

  def names(self) -> List[str]:
    """
    Returns
    -------
    List[str]
        The list of variables name in the CTBN.
    """
    return list(self._name2id.keys())

  def arcs(self) -> Set[Tuple[NodeId, NodeId]]:
    """
    Returns
    -------
    Set[Tuple[NodeId, NodeId]]
        The set of arcs as a set of couple of NodeIds in the CTBN.
    """
    return self._graph.arcs()

  def parents(self, val: NameOrId) -> Set[NodeId]:
    """
    Parameters
    ----------
    val : NameOrId
        The variable's name or id.

    Returns
    -------
    Set[NodeId]
        A set containing the id of the variable's parents in the CTBN.

    Raises
    ------
    pyAgrum.NotFound
        If the variable isn't found in the CTBN.
    """
    return self._graph.parents(self._nameOrId(val))

  def parentNames(self, val: NameOrId) -> List[str]:
    """
    Parameters
    ----------
    val : NameOrId
        The variable's name or id.

    Returns
    -------
    List[str]
        A list containing the names of the variable's parents.

    Raises
    ------
    pyAgrum.NotFound
        If the variable isn't in the CTBN.
    """
    return [self.name(n) for n in self.parents(val)]

  def children(self, val: NameOrId) -> Set[NodeId]:
    """
    Parameters
    ----------
    val : NameOrId
        The variable's name or id.

    Returns
    -------
    Set[NodeId]
        A set containing the ids of the variable's children.

    Raises
    ------
    pyAgrum.NotFound
        If the variable isn't in the CTBN.
    """
    return self._graph.children(self._nameOrId(val))

  def childrenNames(self, val: NameOrId) -> List[str]:
    """
    Parameters
    ----------
    val : NameOrId
        The variable's name or id.

    Returns
    -------
    List[str]
        A list containing the names of a variable's children.

    Raises
    ------
    pyAgrum.NotFound
        If the variable isn't in the CTBN.
    """
    return [self.name(n) for n in self.children(val)]

  def CIM(self, val: NameOrId) -> CIM:
    """
    Parameters
    ----------
    val : NameOrId
        The variable's name or id.

    Returns
    -------
    CIM
        The variable's CIM.

    Raises
    ------
    pyAgrum.NotFound
        If the variable isn't in the CTBN.
    """
    return self._cim[self._nameOrId(val)]

  def completeInstantiation(self):
    """
    Returns
    -------
    pyAgrum.Instantiation
        An instantiation of the variables in the CTBN.
    """
    res = pyAgrum.Instantiation()
    for nod in self.nodes():
      res.add(self.variable(nod))
    return res

  def fullInstantiation(self):
    """
    Returns
    -------
    pyAgrum.Instatiation
        An instantiation of the variables in the CTBN including the corresponding
        starting and ending (i.e from/to variables) variables.
    """
    res = pyAgrum.Instantiation()
    for nod in self.nodes():
      res.add(self.variable(nod))
      res.add(self.CIM(nod).variable(0))  # v_i
      res.add(self.CIM(nod).variable(1))  # v_j
    return res

  def toDot(self):
    """
    Create a display of the graph representating the CTBN.

    Returns
    -------
    str
        A display of the graph.
    """

    chaine = """
        digraph  "ctbn" {
        graph [bgcolor=transparent,label=""];
        node [style=filled fillcolor="#ffffaa"];

        """

    # Add the name of the variables
    for nomVar in self.names():
      chaine += '  "' + str(nomVar) + '"' + ";\n"
    chaine += " "

    # Adding arcs
    for arc in list(self._graph.arcs()):
      num1 = arc[0]
      num2 = arc[1]
      nom1 = self.name(num1)
      nom2 = self.name(num2)
      chaine += '  "' + str(nom1) + '"->"' + str(nom2) + '"' + ";\n"

    chaine = chaine[:-1]
    chaine += "}"
    return chaine

  def equals(self, ctbn: "CTBN") -> bool:
    """
    Tests the topologic equality with another CTBN.

    Parameters
    ----------
    ctbn : CTBN
        CTBN to test equality with.

    Returns
    -------
    bool
        True if they are equal, False if not.
    """

    names1 = self.names()
    names2 = ctbn.names()

    arcs1 = self.arcs()
    arcs2 = ctbn.arcs()

    # Checks the number of nodes
    if len(names1) != len(names2):
      print("difference de taille")
      return False

    # Checks the number of arcs
    if len(arcs1) != len(arcs2):
      print("difference de taille")
      return False

    # Checks if all nodes from current CTBN are in the other one
    for name in names1:
      if name not in names2:
        print("nom non present dans le ctbn en parametres")
        return False

    # Checks if all arcs from current CTBN are in the other one
    for arc in arcs1:
      if not ctbn._graph.existsArc(ctbn._name2id[self._id2var[arc[0]].name()],
                                   ctbn._name2id[self._id2var[arc[1]].name()]):
        print(self._id2var[arc[0]].name(), self._id2var[arc[1]].name())
        print("arc non present dans le ctbn en parametres")
        return False

    return True

  def _compareCIM(self, ctbn: "CTBN") -> float:
    """
    Compute the relative equality of the CIMs with another CTBN's CIMs.

    Parameters
    ----------
    ctbn : CTBN
        CTBN to compare with.

    Returns
    -------
    float
        The difference between the Cims.
    """
    selfCIMList = [self.CIM(i) for i in self.nodes()]
    PCIM = selfCIMList.pop(0)
    for cim in selfCIMList:
      PCIM = PCIM.amalgamate(cim)

    ctbnCIMList = [ctbn.CIM(i) for i in ctbn.nodes()]
    QCIM = ctbnCIMList.pop(0)
    for cim in ctbnCIMList:
      QCIM = QCIM.amalgamate(cim)

    P = PCIM._pot
    Q = QCIM._pot

    P1 = pyAgrum.Potential(P)

    P1.fillWith(Q)

    diff = (P - P1).sq().max()

    return diff

  def __getstate__(self):
    state = {"nodes": [self.variable(i).toFast() for i in self.nodes()],
             # 0 and 1 is the node itself (#i and #j)
             "parents": {self.variable(i).name(): list(self.CIM(i).varNames)[2:] for i in self.nodes()},
             "cim": {self.variable(i).name(): self.CIM(i)._pot[:].flatten().tolist() for i in self.nodes()},
             }
    return state

  def __setstate__(self, state):
    self.__init__()
    for node in state["nodes"]:
      self.add(pyAgrum.fastVariable(node))
    for node, parents in state["parents"].items():
      for parent in parents:
        self.addArc(parent, node)
    for node, cim in state["cim"].items():
      self.CIM(node)._pot.fillWith(cim)
    return self
