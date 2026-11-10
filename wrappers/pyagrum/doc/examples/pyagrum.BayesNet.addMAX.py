import pyagrum as gum

bn = gum.fastBN("A[0,3];B[0,3]")
bn.addMAX(gum.RangeVariable("M", "max of the parents", 0, 3))
# 2
bn.addArcs([("A", "M"), ("B", "M")])
bn.cpt("M")[{"A": 1, "B": 2}].tolist()
# [0.0, 0.0, 1.0, 0.0]
