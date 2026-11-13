# requires: qiskit, qiskit_aer
from qiskit import QuantumCircuit

import pyagrum as gum
from pyagrum.qBNSampling import qBNMC, qBNRejection

bn = gum.fastBN("A->B")
qinf = qBNRejection(qBNMC(bn))
circuit = QuantumCircuit(2)
qinf.addS(circuit, {0: 0, 1: 1})  # phase flip of the states where qubit 0 is 0 and qubit 1 is 1
sorted(circuit.count_ops().items())
# [('cz', 1), ('x', 2)]
