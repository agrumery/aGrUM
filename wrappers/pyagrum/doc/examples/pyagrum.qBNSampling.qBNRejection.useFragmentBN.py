# requires: qiskit, qiskit_aer
import pyagrum as gum
from pyagrum.qBNSampling import qBNMC, qBNRejection

bn = gum.fastBN("A->B->C;D->C")
qinf = qBNRejection(qBNMC(bn))
qinf.useFragmentBN(target={"B"})  # only the ancestors of B are needed for its posterior
sorted(qinf.qbn.bn.names())
# ['A', 'B']
qinf.qbn.getTotNumQBits()
# 2
