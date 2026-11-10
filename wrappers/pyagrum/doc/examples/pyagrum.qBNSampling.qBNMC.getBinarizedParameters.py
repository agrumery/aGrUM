# requires: qiskit, qiskit_aer, qiskit_ibm_runtime
import pyagrum as gum
from pyagrum.qBNSampling import qBNMC

bn = gum.fastBN("A->B[4]")
qbn = qBNMC(bn)
qbn.getBinarizedParameters({"B": qbn.getWidth("B")}, {"B": 2})  # state 2 of B on 2 qubits
# {1: [1, 0]}
