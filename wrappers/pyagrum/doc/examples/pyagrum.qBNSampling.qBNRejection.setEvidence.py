# requires: qiskit, qiskit_aer, qiskit_ibm_runtime
import pyagrum as gum
from pyagrum.qBNSampling import qBNMC, qBNRejection

bn = gum.fastBN("A->B")
qinf = qBNRejection(qBNMC(bn))
qinf.setEvidence({"B": 1})  # state index of B
qinf.evidence
# {'B': 1}
