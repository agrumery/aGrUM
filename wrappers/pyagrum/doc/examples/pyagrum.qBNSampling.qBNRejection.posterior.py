# requires: qiskit, qiskit_aer
import pyagrum as gum
from pyagrum.qBNSampling import qBNMC, qBNRejection

bn = gum.fastBN("A->B")
qinf = qBNRejection(qBNMC(bn))
qinf.setEvidence({"B": 1})
qinf.setMaxIter(10)
p = qinf.posterior("A")  # runs the inference if needed
p.names
# ('A',)
round(p.sum(), 6)
# 1.0
[round(v, 4) for v in qinf.posterior("B").tolist()]
# [0.0, 1.0]
