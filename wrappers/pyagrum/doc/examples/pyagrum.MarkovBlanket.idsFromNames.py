import pyagrum as gum

mb = gum.MarkovBlanket(gum.fastBN("A->B->C<-D;C->E"), "B")
mb.idsFromNames(["D", "A"])
# [3, 0]
