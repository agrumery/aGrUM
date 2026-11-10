# requires: qiskit, qiskit_aer, qiskit_ibm_runtime
import pyagrum as gum
from pyagrum.qBNSampling import qBNMC

bn = gum.fastBN("A->B")
qbn = qBNMC(bn)
circuit = qbn.buildCircuit()
probas = qbn.aerSimulation(circuit, shots=1000)  # marginals estimated from 1000 shots
sorted(probas)
# ['A', 'B']
round(float(sum(probas["A"])), 6)
# 1.0
