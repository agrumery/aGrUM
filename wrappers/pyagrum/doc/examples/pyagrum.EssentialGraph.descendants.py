import pyagrum as gum

bn = gum.fastBN("A->B<-C;B->D")
eg = gum.EssentialGraph(bn)
eg.descendants(bn.idFromName("A"))
# {1, 3}
