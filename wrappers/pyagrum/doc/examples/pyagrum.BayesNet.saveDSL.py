import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.saveDSL("model.dsl")
bn2 = gum.loadBN("model.dsl")
bn2.hasSameStructure(bn)
# True
