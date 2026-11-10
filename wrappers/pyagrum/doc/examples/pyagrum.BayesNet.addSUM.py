import pyagrum as gum

bn = gum.fastBN("A[0,3];B[0,3]")
bn.addSUM(gum.RangeVariable("S", "sum of the parents", 0, 6))
# 2
bn.addArcs([("A", "S"), ("B", "S")])
bn.cpt("S")[{"A": 1, "B": 3}].tolist()
# [0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0]
