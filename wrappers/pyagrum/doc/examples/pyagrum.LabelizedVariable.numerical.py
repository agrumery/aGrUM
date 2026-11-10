import pyagrum as gum

c = gum.LabelizedVariable("c", "color", ["red", "green", "blue"])
c.numerical(2)  # the index for a non-numerical label
# 2.0
