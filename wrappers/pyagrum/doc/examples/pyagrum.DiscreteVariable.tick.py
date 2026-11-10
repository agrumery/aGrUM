import pyagrum as gum

v = gum.fastVariable("x[0:10:5]")  # a DiscretizedVariable
v.tick(1)
# 2.0
