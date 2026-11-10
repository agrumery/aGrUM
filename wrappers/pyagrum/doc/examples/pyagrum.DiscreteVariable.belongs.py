import pyagrum as gum

v = gum.fastVariable("r[1,4]")  # a RangeVariable
v.belongs(3)
# True
v.belongs(5)
# False
