import pyagrum as gum

bn = gum.fastBN("A->B->C<-D;C->E")
fragment = gum.BayesNetFragment(bn)
fragment.installAscendants("C")  # A, B, C, D
s = fragment.spaceCplxToString(fragment.log10DomainSize(), fragment.dim(), fragment.memoryFootprint())
s.startswith("domainSize: 16, dim: 8")
# True
