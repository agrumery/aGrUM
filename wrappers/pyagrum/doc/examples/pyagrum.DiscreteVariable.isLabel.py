import pyagrum as gum

v = gum.fastVariable("c{red|green}")  # a LabelizedVariable
v.isLabel("red")
# True
v.isLabel("blue")
# False
