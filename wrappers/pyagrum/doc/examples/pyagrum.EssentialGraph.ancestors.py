import pyagrum as gum

bn = gum.fastBN("A->B<-C;B->D")
eg = gum.EssentialGraph(bn)
eg.ancestors(bn.idFromName("D"))
# {0, 1, 2}
