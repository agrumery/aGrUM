import pyagrum as gum

bn = gum.fastBN("F->A->B->C<-D;C->E")
mb = gum.MarkovBlanket(bn, "C")  # parents, children and parents of children of C
dot = mb.toDot()  # the node C is highlighted
dot.startswith("digraph")
# True
