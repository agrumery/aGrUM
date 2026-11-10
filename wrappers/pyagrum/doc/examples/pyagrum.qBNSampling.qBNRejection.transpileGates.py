# requires: qiskit, qiskit_aer, qiskit_ibm_runtime
import pyagrum as gum
from pyagrum.qBNSampling import qBNMC, qBNRejection

bn = gum.fastBN("A->B")
qinf = qBNRejection(qBNMC(bn))
qinf.setEvidence({"B": 1})
qinf.getGates()
qinf.transpileGates()  # optimizes the circuits A and G
qinf.A.num_qubits, qinf.G.num_qubits
# (2, 2)
