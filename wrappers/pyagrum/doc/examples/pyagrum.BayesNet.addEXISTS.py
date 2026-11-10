import pyagrum as gum

bn = gum.fastBN("A;B")
bn.addEXISTS(gum.LabelizedVariable("E", "exists a parent equal to 1", 2))
# 2
bn.addArcs([("A", "E"), ("B", "E")])
bn.cpt("E")[{"A": 0, "B": 1}].tolist()
# [0.0, 1.0]
bn.cpt("E")[{"A": 0, "B": 0}].tolist()
# [1.0, 0.0]
