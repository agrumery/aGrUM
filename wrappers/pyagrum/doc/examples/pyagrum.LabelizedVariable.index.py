import pyagrum as gum

c = gum.LabelizedVariable("c", "color", ["red", "green", "blue"])
c.index("blue")
# 2
