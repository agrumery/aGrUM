import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
f = mrf.factor(["A", "B"])
f.names
# ('A', 'B')
mrf.factor({0, 1}).domainSize()  # a set of ids gives the same factor
# 4
