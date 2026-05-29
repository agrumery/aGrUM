# (c) Copyright 2005-2026 by                                               #
#   - Pierre-Henri WUILLEMIN(_at_LIP6)                                     #
#   - Christophe GONZALES(_at_AMU)                                         #
# The aGrUM/pyAgrum library is free software; you can redistribute it      #
# and/or modify it under the terms of either :                             #
#  - the GNU Lesser General Public License as published by                 #
#    the Free Software Foundation, either version 3 of the License,        #
#    or (at your option) any later version,                                #
#  - the MIT license (MIT),                                                #
#  - or both in dual license, as here.                                     #
# (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)      #
# This aGrUM/pyAgrum library is distributed in the hope that it will be    #
# useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,            #
# INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS   #
# FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE     #
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER   #
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,          #
# ARISING FROM, OUT OF OR IN conn WITH THE SOFTWARE OR THE USE OR          #
# OTHER DEALINGS IN THE SOFTWARE.                                           #
# See LICENCES for more details.                                            #
# SPDX-FileCopyrightText: Copyright 2005-2026                              #
#     - Pierre-Henri WUILLEMIN(_at_LIP6)                                   #
#     - Christophe GONZALES(_at_AMU)                                       #
# SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                        #
# Contact  : info_at_agrum_dot_org                                          #
# homepage : http://agrum.gitlab.io                                         #
# gitlab   : https://gitlab.com/agrumery/agrum                             #

from typing import Union

import numpy as np
from qiskit import QuantumCircuit, QuantumRegister, transpile
from qiskit.circuit.library import RYGate, XGate
from qiskit_aer import AerSimulator
from qiskit_ibm_runtime import SamplerV2

from pyagrum import BayesNet, Instantiation
from pyagrum import Tensor as Tensor


class qBNMC:
  """Quantum circuit representation of a Bayesian network.

  Encodes a pyAgrum BayesNet into a quantum circuit using multi-qubit RY
  rotations so that measuring the circuit samples from the joint distribution
  of the network.

  Based on: *Quantum circuit representation of Bayesian networks*,
  Sima E. Borujeni.

  Parameters
  ----------
  bn : BayesNet
      pyAgrum Bayesian network to encode.

  Attributes
  ----------
  bn : BayesNet
      The encoded Bayesian network.
  n_qb_map : dict[int, list[int]]
      Maps each node ID to the list of qubit IDs assigned to it.
  """

  def __init__(self, bn: BayesNet) -> None:
    self.bn = bn
    self.n_qb_map = self.mapNodeToQBit(self.bn.nodes())

  def mapNodeToQBit(self, nodes: set[int]) -> dict[int, list[int]]:
    """Map node IDs to lists of qubit IDs.

    Parameters
    ----------
    nodes : set[int]
        Node IDs from the Bayesian network.

    Returns
    -------
    dict[int, list[int]]
        Node ID → list of assigned qubit IDs.
    """
    res = {}
    qubit_id = 0
    for n_id in nodes:
      res[n_id] = []
      for _ in range(self.getWidth(n_id)):
        res[n_id].append(qubit_id)
        qubit_id += 1
    return res

  def getWidth(self, node: Union[str, int]) -> int:
    """Number of qubits needed to represent a variable.

    Parameters
    ----------
    node : str or int
        Variable name or node ID.

    Returns
    -------
    int
        Ceiling of log2 of the variable's domain size.
    """
    return int(np.ceil(np.log2(self.bn.variable(node).domainSize())))

  def getTotNumQBits(self) -> int:
    """Total number of qubits required for the full circuit.

    Returns
    -------
    int
        Sum of widths over all nodes.
    """
    return int(np.sum([self.getWidth(id) for id in self.bn.nodes()], dtype=int))

  def getBinarizedParameters(
    self,
    width_dict: dict[Union[str, int], int],
    param_dict: dict[Union[str, int], int],
  ) -> dict[int, list[int]]:
    """Binary-encode variable states.

    Parameters
    ----------
    width_dict : dict[str or int, int]
        Variable name → qubit width (see :meth:`getWidth`).
    param_dict : dict[str or int, int]
        Variable name → integer state.

    Returns
    -------
    dict[int, list[int]]
        Node ID → binary string as a list of 0s and 1s.
    """
    width_dict_id = {self.bn.nodeId(self.bn.variable(k)): v for k, v in width_dict.items()}
    param_dict_id = {self.bn.nodeId(self.bn.variable(k)): v for k, v in param_dict.items()}
    return {
      id: np.array(list(np.binary_repr(param_dict_id[id], width=width_dict_id[id]))).astype(int).tolist()
      for id in param_dict_id
    }

  def getRootNodes(self) -> set[int]:
    """IDs of root nodes (no parents) in the DAG.

    Returns
    -------
    set[int]
        Set of root node IDs.
    """
    return {id for id in self.bn.nodes() if len(self.bn.parents(id)) == 0}

  def getAllParentSates(self, node: Union[str, int]) -> list[dict[Union[str, int], int]]:
    """All parent-state combinations for a given node.

    Parameters
    ----------
    node : str or int
        Variable name or node ID.

    Returns
    -------
    list[dict[str or int, int]]
        One dict per combination mapping parent name → state index.
    """
    res = []
    inst = Instantiation()
    for name in self.bn.cpt(node).names[1:]:
      n_id = self.bn.nodeId(self.bn.variable(name))
      if n_id in self.bn.nodes():
        inst.add(self.bn.variable(name))
    inst.setFirst()
    while not inst.end():
      res.append(inst.todict())
      inst.inc()
    return res

  def getQuantumRegisters(self) -> dict[int, QuantumRegister]:
    """Quantum registers for the Bayesian network circuit.

    Returns
    -------
    dict[int, QuantumRegister]
        Node ID → QuantumRegister sized to hold the variable.
    """
    return {
      n_id: QuantumRegister(int(np.ceil(np.log2(self.bn.variable(n_id).domainSize()))), str(n_id))
      for n_id in self.bn.nodes()
    }

  def indicatorFunction(
    self, binary_list: list[list[int]], targets: dict[int, int], verbose: int = 0
  ) -> list[bool]:
    """Match binary strings against target conditions (eq17/eq19).

    Parameters
    ----------
    binary_list : list[list[int]]
        Basis states as lists of 0s and 1s.
    targets : dict[int, int]
        Relative qubit index → required value.

    Returns
    -------
    list[bool]
        True where the binary string satisfies all target conditions.
    """
    if verbose > 0:
      print(f"\nindicatorFunction: binary_list={binary_list}, targets={targets}")

    if not targets:
      return [False] * len(binary_list)

    sorted_keys = sorted(targets)
    binary_arr = np.array(binary_list)[:, sorted_keys]
    pattern = [targets[k] for k in sorted_keys]
    return list(np.all(binary_arr == pattern, axis=1))

  def getProbability(
    self,
    value: int,
    node: Union[str, int],
    qb_id: int,
    param_qbs: dict[int, int],
    param_nodes: dict[Union[str, int], int] | None = None,
    verbose: int = 0,
  ) -> float:
    """Conditional probability for one qubit given context (eq18/eq20).

    Parameters
    ----------
    value : int
        Target qubit state: 0 or 1.
    node : str or int
        Variable name or node ID owning the qubit.
    qb_id : int
        Global qubit index in the circuit.
    param_qbs : dict[int, int]
        Other qubits of the same variable already fixed (global id → value).
    param_nodes : dict[str or int, int], optional
        Parent variable states (name → state index).

    Returns
    -------
    float
        Probability value.
    """
    if param_nodes is None:
      param_nodes = {}

    if verbose > 0:
      print(f"\ngetProbability: node={node}, qb_id={qb_id}, param_nodes={param_nodes}")

    probability_list = self.bn.cpt(node)[param_nodes]
    width = self.getWidth(node)
    number_of_states = self.bn.cpt(node).shape[0]
    binary_state_list = [
      np.array(list(np.binary_repr(s, width=width)), dtype=int).tolist() for s in range(number_of_states)
    ]

    qb_number = self.n_qb_map[node].index(qb_id)
    params_qb_number_dict = {self.n_qb_map[node].index(k): v for k, v in param_qbs.items()}

    if verbose > 0:
      print(f"probability_list={probability_list}, binary_state_list={binary_state_list}")
      print(f"qb_id={qb_id}, qb_number={qb_number}, params_qb_number_dict={params_qb_number_dict}")

    I_qb = self.indicatorFunction(binary_state_list, {**{qb_number: value}, **params_qb_number_dict}, verbose=verbose)

    if verbose > 0:
      print(f"indicator={I_qb}")

    return np.sum(probability_list, where=I_qb)

  def multiQubitRotation(
    self,
    circuit: QuantumCircuit,
    node: Union[str, int],
    target_qbs: list[int],
    param_qbs: dict[int, int],
    param_nodes: dict[Union[str, int], int] | None = None,
    control_qbs: list[int] | None = None,
    verbose: int = 0,
  ) -> None:
    """Add multi-qubit RY rotations to the circuit (Fig9/eq18).

    Recursively encodes the CPT probabilities of `node` into controlled
    RY rotations on `target_qbs`.

    Parameters
    ----------
    circuit : QuantumCircuit
        Circuit to modify in place.
    node : str or int
        Variable name or node ID being encoded.
    target_qbs : list[int]
        Global qubit IDs representing the variable.
    param_qbs : dict[int, int]
        Same-variable qubits already set in the recursion (global id → value).
    param_nodes : dict[str or int, int], optional
        Parent variable conditioning states.
    control_qbs : list[int], optional
        Qubit IDs of parent registers used as controls.
    """
    if param_nodes is None:
      param_nodes = {}
    if control_qbs is None:
      control_qbs = []

    if verbose > 0:
      print(
        f"\nmultiQubitRotation: node={node}, target_qbs={target_qbs}, "
        f"param_qbs={param_qbs}, param_nodes={param_nodes}, control_qbs={control_qbs}"
      )

    target_copy = target_qbs.copy()
    params_qb_list = sorted(param_qbs)

    P1 = self.getProbability(1, node, target_copy[0], param_qbs, param_nodes, verbose=verbose)
    P0 = self.getProbability(0, node, target_copy[0], param_qbs, param_nodes, verbose=verbose)

    theta = np.pi if P0 == 0 else 2 * np.arctan(np.sqrt(P1 / P0))

    if verbose > 0:
      print(f"P1={P1}, P0={P0}, theta={theta}")

    RY = RYGate(theta)
    X = XGate()

    if len(target_copy) == 1:
      if len(param_qbs) + len(control_qbs) > 0:
        RY = RY.control(len(param_qbs) + len(control_qbs))
      circuit.append(RY, qargs=control_qbs + params_qb_list + [target_copy[0]])
    else:
      if len(param_qbs) + len(control_qbs) > 0:
        RY = RY.control(len(param_qbs) + len(control_qbs))
        X = X.control(len(param_qbs) + len(control_qbs))

      circuit.append(RY, qargs=control_qbs + params_qb_list + [target_copy[0]])

      popped_qb = target_copy.pop(0)

      self.multiQubitRotation(circuit, node, target_copy, {**param_qbs, **{popped_qb: 1}}, param_nodes, control_qbs, verbose=verbose)
      circuit.append(X, qargs=control_qbs + params_qb_list + [popped_qb])
      self.multiQubitRotation(circuit, node, target_copy, {**param_qbs, **{popped_qb: 0}}, param_nodes, control_qbs, verbose=verbose)
      circuit.append(X, qargs=control_qbs + params_qb_list + [popped_qb])

    if verbose > 0:
      print(circuit.draw())

  def buildCircuit(self, add_measure: bool = True, verbose: int = 0) -> QuantumCircuit:
    """Build the quantum circuit encoding the Bayesian network.

    Parameters
    ----------
    add_measure : bool, optional
        If True (default), append measurement gates to all qubits.

    Returns
    -------
    QuantumCircuit
        The compiled quantum circuit.
    """
    if verbose > 0:
      print("buildCircuit called")

    q_reg_dict = self.getQuantumRegisters()
    circuit = QuantumCircuit(*list(q_reg_dict.values()))

    root_nodes = self.getRootNodes()
    internal_nodes = [n_id for n_id in self.bn.topologicalOrder() if n_id not in root_nodes]

    for n_id in root_nodes:
      self.multiQubitRotation(circuit, n_id, self.n_qb_map[n_id], {}, verbose=verbose)

    for n_id in internal_nodes:
      parent_id_set = self.bn.parents(n_id)
      parent_qbit_list = list(np.hstack([self.n_qb_map[p_id] for p_id in parent_id_set]))

      for params_dict in self.getAllParentSates(n_id):
        width_dict = {p_id: self.getWidth(p_id) for p_id in parent_id_set}
        bin_params = self.getBinarizedParameters(width_dict, params_dict)

        circuit.barrier()

        zero_qbs = np.array(parent_qbit_list)[np.where(np.hstack(list(bin_params.values())) == 0)]
        for ctrl_qb_id in zero_qbs:
          circuit.append(XGate(), qargs=[ctrl_qb_id])

        self.multiQubitRotation(circuit, n_id, self.n_qb_map[n_id], {}, params_dict, parent_qbit_list, verbose=verbose)

        for ctrl_qb_id in zero_qbs:
          circuit.append(XGate(), qargs=[ctrl_qb_id])

    if add_measure:
      circuit.measure_all()

    return circuit

  def aerSimulation(self, circuit: QuantumCircuit, shots: int = 10000) -> dict[str, list[float]]:
    """Run a circuit on the Aer simulator and return marginal probabilities.

    Parameters
    ----------
    circuit : QuantumCircuit
        Circuit to run (must contain measurements).
    shots : int, optional
        Number of shots (default 10000).

    Returns
    -------
    dict[str, list[float]]
        Variable name → probability vector over its domain.
    """
    backend_aer = AerSimulator()
    sampler_aer = SamplerV2(mode=backend_aer)
    circuit_aer = transpile(circuit, backend=backend_aer)
    job_aer = sampler_aer.run([circuit_aer], shots=shots)
    counts_aer = job_aer.result()[0].data.meas.get_counts()

    res = {}
    for n_id in self.bn.nodes():
      width = len(self.n_qb_map[n_id])
      probability_vector = []
      for state in range(self.bn.variable(n_id).domainSize()):
        pattern = np.binary_repr(state, width=width)
        matches = [
          val
          for key, val in counts_aer.items()
          if key[::-1][self.n_qb_map[n_id][0]: self.n_qb_map[n_id][-1] + 1] == pattern
        ]
        probability_vector.append(np.sum(matches) / shots)
      res[self.bn.variable(n_id).name()] = probability_vector

    return res

  def runBN(self, shots: int = 10000) -> dict[str, Tensor]:
    """Build and run the circuit; return marginals as Tensors.

    Parameters
    ----------
    shots : int, optional
        Number of shots (default 10000).

    Returns
    -------
    dict[str, Tensor]
        Variable name → pyAgrum Tensor with marginal probabilities.
    """
    circuit = self.buildCircuit()
    run_res = self.aerSimulation(circuit, shots=shots)
    res = {}
    for var_name, p_vect in run_res.items():
      potential = Tensor().add(self.bn.variable(var_name))
      potential.fillWith(p_vect)
      res[var_name] = potential
    return res
