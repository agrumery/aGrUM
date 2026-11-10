import pyagrum as gum

n = gum.IntegerVariable("n", "n", [1, 3, 10])
n.isValue(3)
# True
n.isValue(4)
# False
