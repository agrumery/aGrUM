import pyagrum as gum

bn = gum.fastBN("F->A->B->C<-D;C->E")
mb = gum.MarkovBlanket(bn, "C")  # parents, children and parents of children of C
dag = mb.dag()
dag.size()
# 4
dag.sizeArcs()
# 3
