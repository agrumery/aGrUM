import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.MonteCarloSampling(bn)
p = ie.evidenceImpact("B", ["A", "C"])  # P(B|A,C)
p.names, p.shape
# (('B', 'A', 'C'), (2, 2, 2))
