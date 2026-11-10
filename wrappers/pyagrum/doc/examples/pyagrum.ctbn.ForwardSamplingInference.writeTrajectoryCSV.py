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
ie = ct.ForwardSamplingInference(ctbn)
ie.writeTrajectoryCSV("traj.csv", n=2, timeHorizon=10)
open("traj.csv").readline()
# 'IdSample,time,var,state\n'
sorted(ct.readTrajectoryCSV("traj.csv"))  # the ids of the samples
# [0, 1]
