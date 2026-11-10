import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.saveGUM("model.bgum")
bn2 = gum.loadBN("model.bgum")
bn2.hasSameStructure(bn)
# True
