import pyagrum as gum

eg = gum.EssentialGraph(gum.fastBN("A->B<-C;B->D"))
pdag = eg.pdag()  # a modifiable copy
pdag.addNode()
# 4
type(pdag).__name__, pdag.size(), eg.size()
# ('PDAG', 5, 4)
