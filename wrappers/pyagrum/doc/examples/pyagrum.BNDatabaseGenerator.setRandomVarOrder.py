import pyagrum as gum

bn = gum.fastBN("A->B->C;A->D")
generator = gum.BNDatabaseGenerator(bn)
gum.initRandom(42)
generator.setRandomVarOrder()
sorted(generator.varOrderNames())  # a permutation of the names
# ['A', 'B', 'C', 'D']
