import pyagrum as gum

bn = gum.fastBN("A->C<-B;C->D")
mg = bn.moralGraph()  # the parents A and B of C are married
sorted(mg.edges())
# [(0, 1), (0, 2), (1, 2), (1, 3)]
