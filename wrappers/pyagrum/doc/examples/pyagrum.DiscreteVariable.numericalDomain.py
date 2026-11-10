import pyagrum as gum

v = gum.fastVariable("x{0.5|1.5|2.5}")  # a NumericalDiscreteVariable
v.numericalDomain()
# (0.5, 1.5, 2.5)
