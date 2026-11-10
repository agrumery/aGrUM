# requires: qiskit, qiskit_aer, qiskit_ibm_runtime
from qiskit import QuantumCircuit

import pyagrum as gum
from pyagrum.qBNSampling import qBNMC, qBNRejection

bn = gum.fastBN("A->B")
qinf = qBNRejection(qBNMC(bn))
circuit = QuantumCircuit(2)
qinf.addZ(circuit, {0: 1, 1: 1})  # a Z gate controlled by the other evidence qubits
dict(circuit.count_ops())
# {'cz': 1}
