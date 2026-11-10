import pyagrum as gum

bn = gum.fastBN("A->B->C;A->D")
generator = gum.BNDatabaseGenerator(bn)
generator.setAntiTopologicalVarOrder()  # children before parents
order = generator.varOrderNames()
order.index("A") > order.index("B") > order.index("C")
# True
