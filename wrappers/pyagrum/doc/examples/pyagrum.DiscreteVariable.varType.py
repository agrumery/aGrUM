import pyagrum as gum

gum.fastVariable("x[0:10:5]").varType() == gum.VarType_DISCRETIZED
# True
gum.fastVariable("c{red|green}").varType() == gum.VarType_LABELIZED
# True
