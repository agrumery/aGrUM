# requires: qiskit, qiskit_aer
import pyagrum as gum
from pyagrum.qBNSampling import qBNMC

bn = gum.fastBN("A->B[3]->C[5]")
qbn = qBNMC(bn)
[qbn.getWidth(name) for name in "ABC"]  # ceil(log2(domain size))
# [1, 2, 3]
