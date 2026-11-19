# requires: qiskit, qiskit_aer
import pyagrum as gum
from pyagrum.qBNSampling import qBNMC, qBNRejection

bn = gum.fastBN("A->B", uniform=True)
qinf = qBNRejection(qBNMC(bn))
qinf.setEvidence({"B": 1})
qinf.setMaxIter(10)  # 10 samples
res = qinf.makeInference()
sorted(res)
# ['A', 'B']
[round(p, 4) for p in res["B"]]
# [0.0, 1.0]
