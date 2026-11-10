import pyagrum as gum

bn = gum.fastBN("A->B<-C;B->D")
eg = gum.EssentialGraph(bn)
eg.parents(bn.idFromName("B"))  # the v-structure A->B<-C is compelled
# {0, 2}
