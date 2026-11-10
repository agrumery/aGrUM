import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.saveXDSL("model.xdsl")
bn2 = gum.loadBN("model.xdsl")
bn2.hasSameStructure(bn)
# True
