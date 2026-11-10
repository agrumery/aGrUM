import pyagrum as gum

n = gum.IntegerVariable("n", "n", [1, 3, 10])
n.varType() == gum.VarType_INTEGER
# True
