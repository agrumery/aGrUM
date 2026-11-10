# requires: qiskit, qiskit_aer, qiskit_ibm_runtime
import pyagrum as gum
from pyagrum.qBNSampling import qBNMC

bn = gum.fastBN("A[4]->B")
qbn = qBNMC(bn)
qbn.mapNodeToQBit(bn.nodes())  # node id -> qubit ids
# {0: [0, 1], 1: [2]}
