import pyagrum as gum

mrf = gum.MarkovRandomField()
mrf.addVariables(["A{x|y|z}", "B[4]", "C"], 2)
# [0, 1, 2]
[mrf.variable(n).domainSize() for n in ["A", "B", "C"]]
# [3, 4, 2]
