import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyBeliefPropagation(bn)
ie.BN().size()
# 3
sorted(ie.BN().names())
# ['A', 'B', 'C']
