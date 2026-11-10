import pyagrum as gum

r = gum.RangeVariable("r", "r", 2, 5)
r.toFast()
# 'r[2,5]'
