import pyagrum as gum

bn = gum.fastBN("A->B<-C;B->D")
eg = gum.EssentialGraph(bn)
eg.children(bn.idFromName("B"))
# {3}
