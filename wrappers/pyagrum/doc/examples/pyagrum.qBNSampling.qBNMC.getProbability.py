# requires: qiskit, qiskit_aer, qiskit_ibm_runtime
import pyagrum as gum
from pyagrum.qBNSampling import qBNMC

bn = gum.fastBN("A->B")
bn.cpt("A").fillWith([0.2, 0.8])
qbn = qBNMC(bn)
a = bn.idFromName("A")  # the node is given by its id
# probability that the (only) qubit of A is 1
round(float(qbn.getProbability(1, a, qbn.n_qb_map[a][0], {})), 4)
# 0.8
