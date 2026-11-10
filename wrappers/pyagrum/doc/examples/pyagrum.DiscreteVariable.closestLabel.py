import pyagrum as gum

v = gum.fastVariable("x{1|3|10}")
v.closestLabel(4.2)
# '3'
