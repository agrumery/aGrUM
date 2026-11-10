import pyagrum as gum

bn = gum.fastBN("A->B->C;D->C;C->E")
bn.ancestors("C")
# {0, 1, 3}
bn.ancestors(4)  # with the id of E
# {0, 1, 2, 3}
