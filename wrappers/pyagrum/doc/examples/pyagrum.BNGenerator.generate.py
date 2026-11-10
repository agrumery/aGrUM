import pyagrum as gum

gum.initRandom(42)
generator = gum.BNGenerator()
bn = generator.generate(n_nodes=6, n_arcs=8, n_modmax=3)  # at most 8 arcs
bn.size(), bn.sizeArcs()
# (6, 6)
bn.maxVarDomainSize() <= 3
# True
