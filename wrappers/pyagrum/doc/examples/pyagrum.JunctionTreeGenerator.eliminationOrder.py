import pyagrum as gum

bn = gum.fastBN("A->B->C<-D;A->D")
order = gum.JunctionTreeGenerator().eliminationOrder(bn)
sorted(order) == sorted(bn.nodes())
# True
