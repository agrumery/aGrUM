import pyagrum as gum

bn = gum.fastBN("A[0,3];B[0,3]")
bn.addMIN(gum.RangeVariable("m", "min of the parents", 0, 3))
# 2
bn.addArcs([("A", "m"), ("B", "m")])
bn.cpt("m")[{"A": 1, "B": 2}].tolist()
# [0.0, 1.0, 0.0, 0.0]
