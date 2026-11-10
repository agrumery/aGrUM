import pyagrum as gum

bn = gum.fastBN("A->B->C;A->D")
generator = gum.BNDatabaseGenerator(bn)
generator.setVarOrder(["D", "C", "B", "A"])
generator.varOrderNames()
# ('D', 'C', 'B', 'A')
