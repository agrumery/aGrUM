# requires: qiskit, qiskit_aer
import pyagrum as gum
from pyagrum.qBNSampling import qBNMC

bn = gum.fastBN("A[4]->B")  # A needs 2 qubits, B needs 1
qbn = qBNMC(bn)
circuit = qbn.buildCircuit()
circuit.num_qubits, circuit.num_clbits
# (3, 3)
qbn.buildCircuit(add_measure=False).num_clbits
# 0
