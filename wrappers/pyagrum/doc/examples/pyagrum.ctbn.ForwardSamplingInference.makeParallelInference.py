import numpy as np

import pyagrum as gum
import pyagrum.ctbn as ct

ctbn = ct.CTBN()
ctbn.add(gum.LabelizedVariable("A", "A", ["a0", "a1"]))
ctbn.add(gum.LabelizedVariable("B", "B", ["b0", "b1"]))
ctbn.addArc("A", "B")
ctbn.CIM("A").fromMatrix(np.array([[-2.0, 2.0], [1.0, -1.0]]))
ctbn.CIM("B")[{"A": "a0"}] = [[-1, 1], [2, -2]]
ctbn.CIM("B")[{"A": "a1"}] = [[-3, 3], [4, -4]]
gum.initRandom(42)
ie = ct.ForwardSamplingInference(ctbn)
nbEvents = ie.makeParallelInference(nbTrajectories=2, timeHorizon=1000, burnIn=10)
len(nbEvents)  # the number of runs of each sampling
# 2
np.allclose(ie.posterior("A").toarray(), [1 / 3, 2 / 3], atol=0.05)  # exact P(A): [1/3, 2/3]
# True
