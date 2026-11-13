# requires: qiskit, qiskit_aer
from qiskit import QuantumCircuit

import pyagrum as gum
from pyagrum.qBNSampling import qBNMC, qBNRejection

bn = gum.fastBN("A->B")
qinf = qBNRejection(qBNMC(bn))
circuit = QuantumCircuit(2)
qinf.addB(circuit, {0: 0, 1: 1})  # an X gate on the qubits whose evidence is 0
dict(circuit.count_ops())
# {'x': 1}
