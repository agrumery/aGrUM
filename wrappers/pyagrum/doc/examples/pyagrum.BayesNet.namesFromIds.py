import pyagrum as gum

bn = gum.fastBN("A->B->C<-D")
sorted(bn.namesFromIds(bn.parents("C")))
# ['B', 'D']
