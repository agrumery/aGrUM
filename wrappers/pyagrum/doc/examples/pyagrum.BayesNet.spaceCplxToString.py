import pyagrum as gum

gum.BayesNet.spaceCplxToString(3.0, 42, 2048)  # log10 domain size, dimension, memory
# 'domainSize: 1000, dim: 42, mem: 2Ko 0o'
bn = gum.fastBN("A->B->C")
s = bn.spaceCplxToString(bn.log10DomainSize(), bn.dim(), bn.memoryFootprint())
s.startswith("domainSize: 8, dim: 5")
# True
