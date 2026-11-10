# requires: qiskit, qiskit_aer, qiskit_ibm_runtime
import pyagrum as gum
from pyagrum.qBNSampling import qBNMC

bn = gum.fastBN("A->C<-B[3]")
qbn = qBNMC(bn)
qbn.getAllParentSates("C")  # every configuration of the parents of C
# [{'A': 0, 'B': 0}, {'A': 1, 'B': 0}, {'A': 0, 'B': 1}, {'A': 1, 'B': 1}, {'A': 0, 'B': 2}, {'A': 1, 'B': 2}]
