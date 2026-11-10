import pyagrum as gum

bn = gum.fastBN("A;B;C")
bn.addCOUNT(gum.RangeVariable("nb", "number of parents equal to 1", 0, 3))
# 3
bn.addArcs([("A", "nb"), ("B", "nb"), ("C", "nb")])
bn.cpt("nb")[{"A": 1, "B": 0, "C": 1}].tolist()
# [0.0, 0.0, 1.0, 0.0]
