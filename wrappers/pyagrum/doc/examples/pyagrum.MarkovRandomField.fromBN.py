import pyagrum as gum

bn = gum.fastBN("A->C<-B")
mrf = gum.MarkovRandomField.fromBN(bn)
sorted(mrf.edges())  # A and B are married
# [(0, 1), (0, 2), (1, 2)]
