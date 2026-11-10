import pyagrum as gum

bn = gum.fastBN("A[0,4];B[0,4];C[0,4]")
bn.addMEDIAN(gum.RangeVariable("med", "median of the parents", 0, 4))
# 3
bn.addArcs([("A", "med"), ("B", "med"), ("C", "med")])
bn.cpt("med")[{"A": 4, "B": 0, "C": 1}].tolist()
# [0.0, 1.0, 0.0, 0.0, 0.0]
