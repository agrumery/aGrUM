import pyagrum as gum

r = gum.RangeVariable("r", "r", 2, 5)
r.closestIndex(4.2)
# 2
