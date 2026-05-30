############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2026 by                                             #
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
#   See LICENCES for more details.                                         #
#                                                                          #
#   SPDX-FileCopyrightText: Copyright 2005-2026                            #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

from typing import Union

import numpy as np
from qiskit import ClassicalRegister, QuantumCircuit, QuantumRegister, transpile
from qiskit.circuit.library import XGate, ZGate

from pyagrum import BayesNetFragment
from pyagrum import Tensor as Tensor

from ._qBNMC import qBNMC


class qBNRejection:
  """Quantum rejection-sampling inference on a Bayesian network.

  Implements the quantum rejection-sampling algorithm to compute
  posterior distributions conditioned on evidence, using a quantum
  circuit encoding of the Bayesian network (Grover-based amplitude
  amplification).

  Based on: *Quantum Inference on Bayesian Networks*, Guang Hao Low.

  Parameters
  ----------
  qbn : qBNMC
      Quantum Bayesian network object built from :class:`qBNMC`.

  Attributes
  ----------
  qbn : qBNMC
      The underlying quantum Bayesian network.
  q_registers : dict[int, QuantumRegister]
      Quantum registers used to build rotation gates.
  evidence : dict[str or int, int]
      Current evidence: variable name → state index.
  max_iter : int
      Maximum number of rejection-sampling iterations (default 1000).
  """

  def __init__(self, qbn: qBNMC) -> None:
    self.qbn = qbn
    self.q_registers = self.qbn.getQuantumRegisters()
    self.all_qbits = np.hstack(list(self.qbn.n_qb_map.values())).tolist()
    self.evidence = {}
    self.inference_res = None
    self.max_iter = 1000
    self.log = {"A": 0, "G": 0}
    self.A = None
    self.G = None

  def maxIter(self) -> int:
    """Current maximum iteration count."""
    return self.max_iter

  def addA(self, circuit: QuantumCircuit) -> None:
    """Compose the sample-preparation operator A onto the circuit.

    Parameters
    ----------
    circuit : QuantumCircuit
        Circuit to extend in place.
    """
    A = self.qbn.buildCircuit(add_measure=False)
    circuit.compose(A, inplace=True)
    circuit.barrier()

  def addInverse(self, circuit: QuantumCircuit, M: QuantumCircuit) -> None:
    """Compose the adjoint of M onto the circuit.

    Parameters
    ----------
    circuit : QuantumCircuit
        Circuit to extend in place.
    M : QuantumCircuit
        Circuit whose inverse is appended.
    """
    circuit.compose(M.inverse(), inplace=True)

  def addB(self, circuit: QuantumCircuit, evidence_qbs: dict[int, int]) -> None:
    """Apply X gates to evidence qubits with state 0 (eq7 phase flip helper).

    Parameters
    ----------
    circuit : QuantumCircuit
        Circuit to extend in place.
    evidence_qbs : dict[int, int]
        Qubit ID → required quantum state.
    """
    for qb_id, qb_state in evidence_qbs.items():
      if qb_state == 0:
        circuit.compose(XGate(), [qb_id], inplace=True)

  def addZ(self, circuit: QuantumCircuit, evidence_qbs: dict[int, int]) -> None:
    """Apply a (multi-controlled) Z gate over evidence qubits (eq7).

    Parameters
    ----------
    circuit : QuantumCircuit
        Circuit to extend in place.
    evidence_qbs : dict[int, int]
        Qubit ID → required quantum state.
    """
    rotation = ZGate()
    if len(evidence_qbs) > 1:
      rotation = rotation.control(len(evidence_qbs) - 1)
    circuit.compose(rotation, list(evidence_qbs.keys()), inplace=True)

  def addS(self, circuit: QuantumCircuit, evidence_qbs: dict[int, int]) -> None:
    """Apply the phase-flip operator S = B Z B† (eq7).

    Parameters
    ----------
    circuit : QuantumCircuit
        Circuit to extend in place.
    evidence_qbs : dict[int, int]
        Qubit ID → required quantum state.
    """
    self.addB(circuit, evidence_qbs)
    self.addZ(circuit, evidence_qbs)
    self.addB(circuit, evidence_qbs)

  def addG(
    self,
    circuit: QuantumCircuit,
    A: QuantumCircuit,
    evidence_qbs: dict[int, int],
  ) -> None:
    """Compose one Grover iterate G = S_e A^{-1} S_0 A onto the circuit.

    Parameters
    ----------
    circuit : QuantumCircuit
        Circuit to extend in place.
    A : QuantumCircuit
        Sample-preparation circuit.
    evidence_qbs : dict[int, int]
        Qubit ID → required quantum state for the evidence flip S_e.
    """
    self.addS(circuit, evidence_qbs)
    circuit.barrier(label="Sₑ")
    self.addInverse(circuit, A)
    circuit.barrier(label="A⁻¹")
    # S₀ flips the phase of the all-zero state: apply X on every qubit (state=0 means flip)
    self.addS(circuit, {qb_id: 0 for qb_id in self.all_qbits})
    circuit.barrier(label="S₀")
    circuit.compose(A, inplace=True)
    circuit.barrier(label="A")

  def getGates(self) -> None:
    """Precompute and cache operators A and G for the current evidence."""
    evidence_n_id = {self.qbn.bn.nodeId(self.qbn.bn.variable(k)): v for k, v in self.evidence.items()}
    evidence_qbs = self.getEvidenceQuBits(evidence_n_id)

    self.A = QuantumCircuit(*list(self.q_registers.values()))
    self.addA(self.A)

    self.G = QuantumCircuit(*list(self.q_registers.values()))
    self.addG(self.G, self.A, evidence_qbs)

  def transpileGates(self) -> None:
    """Transpile cached A and G to optimisation level 3."""
    self.A = transpile(self.A, optimization_level=3)
    self.G = transpile(self.G, optimization_level=3)

  def getEvidenceQuBits(self, evidence: dict[int, int]) -> dict[int, int]:
    """Convert node-level evidence to qubit-level evidence.

    Parameters
    ----------
    evidence : dict[int, int]
        Node ID → state index.

    Returns
    -------
    dict[int, int]
        Qubit ID → qubit state (0 or 1).
    """
    res = {}
    for n_id, n_state in evidence.items():
      bin_state = np.binary_repr(n_state, width=self.qbn.getWidth(n_id))
      for qb_num in range(len(bin_state)):
        res[self.qbn.n_qb_map[n_id][qb_num]] = int(bin_state[qb_num])
    return res

  def getSample(
    self,
    A: QuantumCircuit,
    G: QuantumCircuit,
    evidence: dict[Union[str, int], int],
    verbose: int = 0,
  ) -> dict[int, int]:
    """Draw one sample consistent with evidence using Algorithm 1.

    Parameters
    ----------
    A : QuantumCircuit
        Sample-preparation circuit.
    G : QuantumCircuit
        Grover iterate circuit.
    evidence : dict[str or int, int]
        Variable name → required state.

    Returns
    -------
    dict[int, int]
        Node ID → sampled state.
    """
    cl_reg = ClassicalRegister(len(self.all_qbits), "meas")
    circuit = QuantumCircuit(*list(self.q_registers.values()), cl_reg)
    circuit.compose(A, inplace=True)

    # measure_all(add_bits=False) appends num_qubits measurements + 1 barrier.
    # We track this length to truncate cleanly before each new Grover power,
    # instead of relying on pop() which would break if qiskit's internal layout changes.
    n_meas_ops = circuit.num_qubits + 1
    circuit.measure_all(add_bits=False)

    k = -2
    while True:
      k += 1
      if k > 20:
        raise RuntimeError(
          "qBNRejection.getSample: evidence not satisfied after 2^20 Grover iterates — "
          "evidence may be inconsistent or have near-zero probability"
        )

      circuit.data = circuit.data[:-n_meas_ops]
      circuit.compose(G.power(int(np.ceil(2.0**k))), inplace=True)
      circuit.measure_all(add_bits=False)

      run_res = self.qbn.aerSimulation(circuit, shots=1)
      self.log["A"] += 1
      self.log["G"] += 2 ** (k + 1)

      # argmax instead of index(1.0): robust to numpy float representation
      run_res = {
        self.qbn.bn.nodeId(self.qbn.bn.variable(node)): int(np.argmax(state)) for node, state in run_res.items()
      }

      if verbose > 0:
        print(f"k={k + 1}, A:+1, G:+{2 ** (k + 1)}, run_res={run_res}")

      if all(run_res[self.qbn.bn.nodeId(self.qbn.bn.variable(node))] == state for node, state in evidence.items()):
        break

    return run_res

  def makeInference(self, verbose: int = 0) -> dict[str, list[float]]:
    """Run rejection sampling and accumulate marginal probabilities.

    Uses precomputed gates if available, otherwise calls :meth:`getGates`.

    Parameters
    ----------
    verbose : int, optional
        0 = silent, 1 = per-iteration stats, 2 = full debug.

    Returns
    -------
    dict[str, list[float]]
        Variable name → probability vector over its domain.
    """
    if self.A is None or self.G is None:
      self.getGates()

    self.log = {"A": 0, "G": 0}
    res = {node: [0.0] * self.qbn.bn.variable(node).domainSize() for node in self.qbn.n_qb_map}

    for i in range(self.max_iter):
      sample = self.getSample(self.A, self.G, self.evidence, verbose=1 if verbose == 2 else 0)
      for node, state in sample.items():
        res[node][state] += 1.0 / self.max_iter
      if verbose == 1:
        print(f"iteration: {i}, log: {self.log}")

    self.inference_res = {self.qbn.bn.variable(k).name(): v for k, v in res.items()}
    return self.inference_res

  def setEvidence(self, evidence: dict[Union[str, int], int]) -> None:
    """Set the evidence for subsequent inference calls.

    Parameters
    ----------
    evidence : dict[str or int, int]
        Variable name → observed state index.
    """
    self.evidence = evidence

  def setMaxIter(self, max_iter: int = 1000) -> None:
    """Set the maximum number of rejection-sampling iterations.

    Parameters
    ----------
    max_iter : int, optional
        Iteration cap (default 1000).
    """
    self.max_iter = max_iter

  def posterior(self, node: Union[str, int]) -> Tensor:
    """Return the posterior distribution of a variable as a Tensor.

    Calls :meth:`makeInference` if results are not yet available.

    Parameters
    ----------
    node : str or int
        Variable name or node ID.

    Returns
    -------
    Tensor
        pyAgrum Tensor with the posterior probability vector.
    """
    name = self.qbn.bn.variable(node).name()
    potential = Tensor().add(self.qbn.bn.variable(name))
    if self.inference_res is None:
      self.makeInference()
    potential.fillWith(self.inference_res[name])
    return potential

  def useFragmentBN(
    self,
    evidence: set[Union[str, int]] | None = None,
    target: set[Union[str, int]] | None = None,
  ) -> None:
    """Restrict the network to the ancestors of target and evidence nodes.

    Replaces the internal :class:`qBNMC` with one built from the minimal
    BayesNetFragment covering all relevant nodes.

    Parameters
    ----------
    evidence : set[str or int], optional
        Additional evidence nodes (beyond those in :attr:`evidence`).
    target : set[str or int], optional
        Target query nodes.
    """
    if evidence is None:
      evidence = set()
    if target is None:
      target = set()
    evidence = evidence.union(self.evidence.keys())

    fbn = BayesNetFragment(self.qbn.bn)
    for node in target.union(evidence):
      fbn.installAscendants(node)

    self.qbn = qBNMC(fbn.toBN())
    self.q_registers = self.qbn.getQuantumRegisters()
    self.all_qbits = np.hstack(list(self.qbn.n_qb_map.values())).tolist()
