# requires: qiskit, qiskit_aer
import pyagrum as gum
from pyagrum.qBNSampling import qBNMC, qBNRejection

bn = gum.fastBN("A->B")
qinf = qBNRejection(qBNMC(bn))
qinf.maxIter()
# 1000
qinf.setMaxIter(50)
qinf.maxIter()
# 50
