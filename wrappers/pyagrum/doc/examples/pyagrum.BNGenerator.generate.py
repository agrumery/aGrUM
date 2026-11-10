import pyagrum as gum

gum.initRandom(42)
generator = gum.BNGenerator()
bn = generator.generate(n_nodes=6, n_arcs=8, n_modmax=3)
bn.size(), bn.sizeArcs()
# (6, 8)
bn.maxVarDomainSize() <= 3
# True
