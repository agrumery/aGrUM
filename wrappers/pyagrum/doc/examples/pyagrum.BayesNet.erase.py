import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.erase("B")
sorted(bn.names())
# ['A', 'C']
bn.arcs()
# set()
