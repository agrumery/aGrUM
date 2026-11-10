# requires: qiskit, qiskit_aer, qiskit_ibm_runtime
import pyagrum as gum
from pyagrum.qBNSampling import qBNMC

bn = gum.fastBN("A[4]->B")
qbn = qBNMC(bn)
{node: register.size for node, register in qbn.getQuantumRegisters().items()}  # one register per node
# {0: 2, 1: 1}
