import pyagrum as gum

x = gum.NumericalDiscreteVariable("x", "x", [0.5, 1.5, 4])
x.closestIndex(3)
# 2
