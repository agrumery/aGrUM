import pyagrum as gum

bn = gum.fastBN("F->A->B->C<-D;C->E")
mb = gum.MarkovBlanket(bn, "C")  # parents, children and parents of children of C
mb.hasSameStructure(gum.fastBN("B->C<-D;C->E"))
# True
mb.hasSameStructure(gum.fastBN("B->C;C->E"))
# False
