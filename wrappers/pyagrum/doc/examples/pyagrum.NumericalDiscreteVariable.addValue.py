import pyagrum as gum

x = gum.NumericalDiscreteVariable("x", "x", [0.5, 1.5, 4])
x.addValue(2.5)
print(x)
# x:NumericalDiscrete({0.5|1.5|2.5|4})
