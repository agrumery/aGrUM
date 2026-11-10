import pyagrum as gum

bn = gum.fastBN("F->A->B->C<-D;C->E")
mb = gum.MarkovBlanket(bn, "C")  # parents, children and parents of children of C
sorted((bn.variable(t).name(), bn.variable(h).name()) for t, h in mb.arcs())
# [('B', 'C'), ('C', 'E'), ('D', 'C')]
