import pyagrum as gum

bn = gum.BayesNet()
bn.empty()
# True
bn.add("A")
# 0
bn.empty()
# False
