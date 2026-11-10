import pyagrum as gum

bn = gum.fastBN("A{yes|no}->B{low|high}")
bn.cpt("A").fillWith([0.5, 0.5])
bn.cpt("B")[:] = [[0, 1], [1, 0]]  # B is "high" iff A is "yes"
generator = gum.BNDatabaseGenerator(bn)
ll = generator.drawSamples(5, {"A": "yes"})
generator.samplesLabelAt(0, 0), generator.samplesLabelAt(0, 1)
# ('yes', 'high')
