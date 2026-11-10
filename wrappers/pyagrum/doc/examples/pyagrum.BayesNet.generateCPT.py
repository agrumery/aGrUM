import pyagrum as gum

bn = gum.BayesNet()
bn.addVariables(["A", "B"])
# [0, 1]
bn.addArc("A", "B")
bn.generateCPT("B")  # random parameters for B
[round(v, 6) for v in bn.cpt("B").sumOut("B").tolist()]  # each distribution sums to 1
# [1.0, 1.0]
