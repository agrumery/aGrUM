# requires: qiskit, qiskit_aer
from qiskit import QuantumCircuit

import pyagrum as gum
from pyagrum.qBNSampling import qBNMC, qBNRejection

bn = gum.fastBN("A->B")
qinf = qBNRejection(qBNMC(bn))
A = QuantumCircuit(*qinf.q_registers.values())
qinf.addA(A)
circuit = QuantumCircuit(*qinf.q_registers.values())
qinf.addG(circuit, A, {1: 1})  # one Grover iterate for the evidence B=1 (qubit 1)
circuit.num_qubits
# 2
circuit.size() > 2 * A.size()
# True
