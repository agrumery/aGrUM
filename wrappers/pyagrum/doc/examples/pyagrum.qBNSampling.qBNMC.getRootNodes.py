# requires: qiskit, qiskit_aer
import pyagrum as gum
from pyagrum.qBNSampling import qBNMC

bn = gum.fastBN("A->C<-B;C->D")
qbn = qBNMC(bn)
sorted(bn.variable(i).name() for i in qbn.getRootNodes())
# ['A', 'B']
