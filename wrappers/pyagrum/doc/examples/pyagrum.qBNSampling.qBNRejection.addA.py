# requires: qiskit, qiskit_aer
from qiskit import QuantumCircuit

import pyagrum as gum
from pyagrum.qBNSampling import qBNMC, qBNRejection

bn = gum.fastBN("A->B")
qinf = qBNRejection(qBNMC(bn))
circuit = QuantumCircuit(*qinf.q_registers.values())
qinf.addA(circuit)  # the circuit that samples the BN
circuit.num_qubits
# 2
circuit.size() > 0
# True
