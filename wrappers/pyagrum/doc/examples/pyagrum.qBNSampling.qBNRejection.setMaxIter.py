# requires: qiskit, qiskit_aer
import pyagrum as gum
from pyagrum.qBNSampling import qBNMC, qBNRejection

bn = gum.fastBN("A->B")
qinf = qBNRejection(qBNMC(bn))
qinf.setMaxIter(20)
qinf.maxIter()
# 20
