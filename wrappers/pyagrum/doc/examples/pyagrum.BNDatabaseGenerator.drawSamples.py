import pyagrum as gum

bn = gum.fastBN("A->B")
bn.cpt("A").fillWith([0.5, 0.5])
bn.cpt("B")[:] = [[0, 1], [1, 0]]  # B is the opposite of A
generator = gum.BNDatabaseGenerator(bn)
generator.drawSamples(10)  # the log2-likelihood of the database: each row has probability 1/2
# -10.0
generator.samplesNbRows()
# 10

# only the samples compatible with the evidence B=0 are kept (hence A=1)
ll = generator.drawSamples(5, {"B": 0})
generator.samplesNbRows()
# 5
generator.varOrderNames()
# ('A', 'B')
sorted({(generator.samplesLabelAt(row, 0), generator.samplesLabelAt(row, 1)) for row in range(5)})
# [('1', '0')]
