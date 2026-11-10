import pyagrum as gum

x = gum.DiscretizedVariable("x", "x", [0, 1, 5, 10])
x.isTick(5)
# True
x.isTick(4)
# False
