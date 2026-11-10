import pyagrum as gum

bn = gum.fastBN("A->B->C")
cm = gum.CausalModel(bn, [("U", ["A", "C"])])
cm.latentVariablesIds()
# {3}
cm.nameFromId(3)
# 'U'
