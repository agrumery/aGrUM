import pyagrum as gum

bn = gum.fastBN("A->B<-C")
gum.saveBN(bn, "model.bif")  # the format is given by the extension
gum.loadBN("model.bif").sizeArcs()
# 2
