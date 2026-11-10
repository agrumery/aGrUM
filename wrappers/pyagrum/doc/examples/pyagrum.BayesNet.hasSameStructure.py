import pyagrum as gum

bn1 = gum.fastBN("A->B->C")
bn2 = gum.fastBN("A->B->C")  # other parameters
bn1.hasSameStructure(bn2)
# True
bn1.hasSameStructure(gum.fastBN("A->B<-C"))
# False
