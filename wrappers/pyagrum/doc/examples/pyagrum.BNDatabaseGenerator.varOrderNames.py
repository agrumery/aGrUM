import pyagrum as gum

bn = gum.fastBN("A->B->C;A->D")
generator = gum.BNDatabaseGenerator(bn)
generator.varOrderNames()
# ('A', 'B', 'C', 'D')
generator.setVarOrder(["D", "C", "B", "A"])
generator.varOrderNames()
# ('D', 'C', 'B', 'A')
