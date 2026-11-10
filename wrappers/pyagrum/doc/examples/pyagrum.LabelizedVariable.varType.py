import pyagrum as gum

c = gum.LabelizedVariable("c", "color", ["red", "green", "blue"])
c.varType() == gum.VarType_LABELIZED
# True
