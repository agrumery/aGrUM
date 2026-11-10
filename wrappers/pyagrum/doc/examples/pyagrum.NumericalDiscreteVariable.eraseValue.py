import pyagrum as gum

x = gum.NumericalDiscreteVariable("x", "x", [0.5, 1.5, 4])
x.eraseValue(1.5)
print(x)
# x:NumericalDiscrete({0.5|4})
