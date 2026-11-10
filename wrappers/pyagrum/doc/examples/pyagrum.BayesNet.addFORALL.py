import pyagrum as gum

bn = gum.fastBN("A;B")
bn.addFORALL(gum.LabelizedVariable("F", "all the parents equal to 1", 2))
# 2
bn.addArcs([("A", "F"), ("B", "F")])
bn.cpt("F")[{"A": 1, "B": 1}].tolist()
# [0.0, 1.0]
bn.cpt("F")[{"A": 1, "B": 0}].tolist()
# [1.0, 0.0]
