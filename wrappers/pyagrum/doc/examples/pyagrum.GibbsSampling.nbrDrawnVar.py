import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.GibbsSampling(bn)
ie.nbrDrawnVar()
# 2
ie.setNbrDrawnVar(1)
ie.nbrDrawnVar()
# 1
