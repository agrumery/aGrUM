# requires: qiskit, qiskit_aer, qiskit_ibm_runtime
import pyagrum as gum
from pyagrum.qBNSampling import qBNMC, qBNRejection

bn = gum.fastBN("A[4]->B")
qinf = qBNRejection(qBNMC(bn))
qinf.getEvidenceQuBits({bn.idFromName("A"): 2})  # state 2 of A is 10 on the qubits 0 and 1
# {0: 1, 1: 0}
