import pyagrum as gum

v = gum.fastVariable("x[0:10:5]")  # a DiscretizedVariable
v.isTick(4)
# True
v.isTick(5)
# False
