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
ie = ct.SimpleInference(ctbn)
ie.makeInference(0.5)  # at t=0.5, from a uniform initial state
[round(p, 4) for p in ie.posterior("A").tolist()]
# [0.3705, 0.6295]
ie.makeInference()  # t=5000: the stationary distribution
print(ie.posterior("A"))
#
#   A                │
# a0       │a1       │
# ─────────│─────────│
#  0.3333  │ 0.6667  │
