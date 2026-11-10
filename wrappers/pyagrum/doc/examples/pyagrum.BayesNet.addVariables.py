import pyagrum as gum

bn = gum.BayesNet()
bn.addVariables(["A", "B[3]", "C{low|high}"])
# [0, 1, 2]
print(bn.variable("B"))
# B:Range([0,2])
