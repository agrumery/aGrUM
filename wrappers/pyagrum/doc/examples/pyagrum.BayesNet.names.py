import pyagrum as gum

bn = gum.fastBN("A->B->C")
sorted(bn.names())
# ['A', 'B', 'C']
