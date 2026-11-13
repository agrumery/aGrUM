# requires: qiskit, qiskit_aer
import pyagrum as gum
from pyagrum.qBNSampling import qBNMC

gum.initRandom(42)
bn = gum.fastBN("A->B")
qbn = qBNMC(bn)
marginals = qbn.runBN(shots=1000)  # a Tensor per variable
sorted(marginals)
# ['A', 'B']
round(marginals["B"].sum(), 6)
# 1.0
