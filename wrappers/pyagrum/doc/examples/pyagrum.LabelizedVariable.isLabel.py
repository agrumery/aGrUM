import pyagrum as gum

c = gum.LabelizedVariable("c", "color", ["red", "green", "blue"])
c.isLabel("red")
# True
c.isLabel("pink")
# False
