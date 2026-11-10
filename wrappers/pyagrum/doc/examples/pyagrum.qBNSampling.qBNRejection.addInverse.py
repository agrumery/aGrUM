# requires: qiskit, qiskit_aer, qiskit_ibm_runtime
from qiskit import QuantumCircuit

import pyagrum as gum
from pyagrum.qBNSampling import qBNMC, qBNRejection

bn = gum.fastBN("A->B")
qinf = qBNRejection(qBNMC(bn))
M = QuantumCircuit(1)
M.ry(0.5, 0)
circuit = QuantumCircuit(1)
qinf.addInverse(circuit, M)
float(circuit.data[0].operation.params[0])  # the inverse of RY(0.5) is RY(-0.5)
# -0.5
