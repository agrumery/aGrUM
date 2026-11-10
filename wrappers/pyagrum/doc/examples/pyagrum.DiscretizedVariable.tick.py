import pyagrum as gum

x = gum.DiscretizedVariable("x", "x", [0, 1, 5, 10])
x.tick(2)
# 5.0
