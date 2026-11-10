# requires: qiskit, qiskit_aer, qiskit_ibm_runtime
import pyagrum as gum
from pyagrum.qBNSampling import qBNMC

qbn = qBNMC(gum.fastBN("A->B"))
states = [[0, 0], [0, 1], [1, 0], [1, 1]]
[bool(b) for b in qbn.indicatorFunction(states, {0: 1})]  # first qubit equal to 1
# [False, False, True, True]
