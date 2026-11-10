import pyagrum as gum

bn = gum.fastBN("A->B->C;A->D")
generator = gum.BNDatabaseGenerator(bn)
generator.setVarOrder(["D", "C", "B", "A"])
generator.varOrder()
# (3, 2, 1, 0)
