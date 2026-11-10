import pyagrum as gum

v = gum.fastVariable("x{0.5|1.5|2.5}")
w = v.asNumericalDiscreteVar()  # a NumericalDiscreteVariable (a copy of v)
print(w.addValue(3.5))
# x:NumericalDiscrete({0.5|1.5|2.5|3.5})
