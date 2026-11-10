import pyagrum as gum

mpdag = gum.MPDAG.meekClosure(gum.fastPDAG("A-B->C-D"))
mpdag.contains(gum.fastDAG("A;B;C;D;B->A;B->C;C->D"))
# True
mpdag.contains(gum.fastDAG("A->B<-C->D"))  # C->B contradicts B->C
# False
