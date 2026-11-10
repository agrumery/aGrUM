import pyagrum as gum

bn = gum.fastBN("A->B")
bn.cpt("A").fillWith([0.5, 0.5])
bn.cpt("B")[:] = [[0, 1], [1, 0]]  # B is the opposite of A
generator = gum.BNDatabaseGenerator(bn)
ll = generator.drawSamples(5, {"A": 1})  # only samples with A=1
generator.samplesAt(0, 0), generator.samplesAt(0, 1)
# (1, 0)
