# requires: qiskit, qiskit_aer, qiskit_ibm_runtime
from qiskit import QuantumCircuit

import pyagrum as gum
from pyagrum.qBNSampling import qBNMC

bn = gum.fastBN("A->B")
qbn = qBNMC(bn)
circuit = QuantumCircuit(1)
qbn.multiQubitRotation(circuit, bn.idFromName("A"), [0], {})  # encodes P(A) as a RY rotation on qubit 0
dict(circuit.count_ops())
# {'ry': 1}
