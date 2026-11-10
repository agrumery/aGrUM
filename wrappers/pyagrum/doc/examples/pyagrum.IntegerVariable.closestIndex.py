import pyagrum as gum

n = gum.IntegerVariable("n", "n", [1, 3, 10])
n.closestIndex(7)
# 2
