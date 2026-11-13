# requires: qiskit, qiskit_aer
import pyagrum as gum
from pyagrum.qBNSampling import qBNMC

bn = gum.fastBN("A[4]->B->C[5]")  # 2 + 1 + 3 qubits
qbn = qBNMC(bn)
qbn.getTotNumQBits()
# 6
