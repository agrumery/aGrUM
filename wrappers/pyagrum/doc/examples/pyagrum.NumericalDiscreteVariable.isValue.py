import pyagrum as gum

x = gum.NumericalDiscreteVariable("x", "x", [0.5, 1.5, 4])
x.isValue(1.5)
# True
x.isValue(2)
# False
