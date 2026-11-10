import pyagrum as gum

eg = gum.EssentialGraph(gum.fastBN("A->B<-C;B->D"))
eg.sizeNodes()
# 4
