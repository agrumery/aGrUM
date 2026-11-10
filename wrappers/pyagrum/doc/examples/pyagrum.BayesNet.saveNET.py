import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.saveNET("model.net")
bn2 = gum.loadBN("model.net")
bn2.hasSameStructure(bn)
# True
