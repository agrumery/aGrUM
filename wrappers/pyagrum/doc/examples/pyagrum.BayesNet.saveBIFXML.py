import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.saveBIFXML("model.bifxml")
bn2 = gum.loadBN("model.bifxml")
bn2.hasSameStructure(bn)
# True
