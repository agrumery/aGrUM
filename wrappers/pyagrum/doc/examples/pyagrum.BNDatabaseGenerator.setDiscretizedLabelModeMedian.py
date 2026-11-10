import pyagrum as gum

bn = gum.fastBN("X[1,2,4]")
bn.cpt("X").fillWith([0, 1])  # X is always in [2;4]
generator = gum.BNDatabaseGenerator(bn)
generator.setDiscretizedLabelModeMedian()
ll = generator.drawSamples(3)
generator.samplesLabelAt(0, 0)
# '3.000000'
