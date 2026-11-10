import pyagrum as gum

x = gum.DiscretizedVariable("x", "x", [0, 1, 5, 10])
x.domain()
# '<[0;1[,[1;5[,[5;10]>'
