import pyagrum as gum

x = gum.DiscretizedVariable("x", "x", [0, 1, 5, 10])
x.ticks()
# (0.0, 1.0, 5.0, 10.0)
