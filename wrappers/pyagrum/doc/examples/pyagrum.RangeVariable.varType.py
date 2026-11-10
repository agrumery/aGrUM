import pyagrum as gum

r = gum.RangeVariable("r", "r", 2, 5)
r.varType() == gum.VarType_RANGE
# True
