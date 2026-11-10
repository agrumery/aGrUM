import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.saveBIF("model.bif")
bn2 = gum.loadBN("model.bif")
bn2.hasSameStructure(bn)
# True
