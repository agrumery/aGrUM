import pyagrum as gum

bn = gum.fastBN("F->A->B->C<-D;C->E")
mb = gum.MarkovBlanket(bn, "C")  # parents, children and parents of children of C
b = bn.idFromName("B")
sorted(bn.variable(i).name() for i in mb.descendants(b))
# ['C', 'E']
