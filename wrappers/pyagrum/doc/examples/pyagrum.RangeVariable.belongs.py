import pyagrum as gum

r = gum.RangeVariable("r", "r", 2, 5)
r.belongs(3)
# True
r.belongs(6)
# False
