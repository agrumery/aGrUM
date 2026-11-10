import pyagrum as gum

gum.saveBN(gum.fastBN("A->B<-C"), "model.bif")
bn = gum.loadBN("model.bif")
sorted(bn.names())
# ['A', 'B', 'C']
bn.sizeArcs()
# 2
