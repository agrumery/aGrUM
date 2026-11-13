import pyagrum as gum

bn = gum.BayesNet("example")
bn.add(gum.LabelizedVariable("A", "a binary variable", 2))
# 0
bn.add("B{yes|no|maybe}")  # a variable in fast syntax
# 1
bn.add("C", 4)  # a name and a number of modalities
# 2
print(bn.variable("B"))
# B:Labelized({yes|no|maybe})
bn.add("'X-ray'[3]")  # a name between single quotes may contain any character
# 3
