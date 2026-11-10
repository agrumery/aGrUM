import pyagrum as gum

v = gum.fastVariable("x[0:10:5]")  # a DiscretizedVariable
v.ticks()
# (0.0, 2.0, 4.0, 6.0, 8.0, 10.0)
