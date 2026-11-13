# requires: qiskit, qiskit_aer
import pyagrum as gum
from pyagrum.qBNSampling import qBNMC, qBNRejection

bn = gum.fastBN("A->B")
qinf = qBNRejection(qBNMC(bn))
qinf.setEvidence({"B": 1})
qinf.getGates()  # computes the circuits A and G for this evidence
qinf.A.num_qubits, qinf.G.num_qubits
# (2, 2)
