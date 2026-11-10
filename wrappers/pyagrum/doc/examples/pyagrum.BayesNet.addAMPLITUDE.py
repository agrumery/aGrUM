import pyagrum as gum

bn = gum.fastBN("A[0,3];B[0,3];C[0,3]")
bn.addAMPLITUDE(gum.RangeVariable("amp", "max - min", 0, 3))
# 3
bn.addArcs([("A", "amp"), ("B", "amp"), ("C", "amp")])
bn.cpt("amp")[{"A": 1, "B": 3, "C": 2}].tolist()  # amp = 3 - 1 = 2
# [0.0, 0.0, 1.0, 0.0]
