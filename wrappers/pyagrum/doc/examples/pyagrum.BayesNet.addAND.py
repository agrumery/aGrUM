import pyagrum as gum

bn = gum.fastBN("A;B")
bn.addAND(gum.LabelizedVariable("C", "A and B", 2))
# 2
bn.addArcs([("A", "C"), ("B", "C")])
bn.cpt("C")[{"A": 1, "B": 1}].tolist()
# [0.0, 1.0]
bn.cpt("C")[{"A": 1, "B": 0}].tolist()
# [1.0, 0.0]
