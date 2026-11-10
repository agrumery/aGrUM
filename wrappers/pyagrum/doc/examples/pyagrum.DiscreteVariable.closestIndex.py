import pyagrum as gum

v = gum.fastVariable("x{1|3|10}")
v.closestIndex(4.2)
# 1
v.closestIndex(100)
# 2
