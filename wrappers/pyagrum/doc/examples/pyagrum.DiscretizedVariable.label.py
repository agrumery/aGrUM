import pyagrum as gum

x = gum.DiscretizedVariable("x", "x", [0, 1, 5, 10])
x.label(1)
# '[1;5['
