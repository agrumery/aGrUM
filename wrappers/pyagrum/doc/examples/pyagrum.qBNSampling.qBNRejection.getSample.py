# requires: qiskit, qiskit_aer, qiskit_ibm_runtime
import pyagrum as gum
from pyagrum.qBNSampling import qBNMC, qBNRejection

bn = gum.fastBN("A->B")
qinf = qBNRejection(qBNMC(bn))
qinf.setEvidence({"B": 1})
qinf.getGates()
sample = qinf.getSample(qinf.A, qinf.G, qinf.evidence)  # node id -> state
sample[bn.idFromName("B")]  # always compatible with the evidence
# 1
