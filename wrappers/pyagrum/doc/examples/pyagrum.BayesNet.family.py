import pyagrum as gum

bn = gum.fastBN("A->C<-B;C->D")
bn.family("C")  # C and its parents
# {0, 1, 2}
