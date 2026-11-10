import pyagrum as gum

x = gum.DiscretizedVariable("x", "x", [0, 1, 5, 10])
x.numerical(1)  # the middle of the interval [1;5[
# 3.0
