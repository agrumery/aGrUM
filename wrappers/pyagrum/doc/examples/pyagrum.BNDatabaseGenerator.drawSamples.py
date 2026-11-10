import pyagrum as gum

bn = gum.fastBN("A->B")
bn.cpt("A").fillWith([0.5, 0.5])
bn.cpt("B")[:] = [[0, 1], [1, 0]]  # B is the opposite of A
generator = gum.BNDatabaseGenerator(bn)
generator.drawSamples(10)  # the log2-likelihood of the database: each row has probability 1/2
# -10.0
generator.samplesNbRows()
# 10
