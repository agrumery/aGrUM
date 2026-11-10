import pyagrum as gum

bn = gum.fastBN("A->B")
bn.cpt("A").fillWith([0.5, 0.5])
bn.cpt("B")[:] = [[0, 1], [1, 0]]  # B is the opposite of A
generator = gum.BNDatabaseGenerator(bn)
ll = generator.drawSamples(10)
generator.log2likelihood()
# -10.0
