import pyagrum as gum

bn = gum.fastBN("A->C<-B;C->D")
bn.parents("C")
# {0, 2}
bn.parents(["C", "D"])
# {0, 1, 2}
