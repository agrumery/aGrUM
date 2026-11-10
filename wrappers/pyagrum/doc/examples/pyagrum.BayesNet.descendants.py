import pyagrum as gum

bn = gum.fastBN("A->B->C;D->C;C->E")
bn.descendants("B")
# {2, 4}
