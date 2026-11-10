import pyagrum as gum

bn = gum.fastBN("A->B->C;A->D")
generator = gum.BNDatabaseGenerator(bn)
with open("header.csv", "w") as f:
  f.write("C,A,D,B\n")
generator.setVarOrderFromCSV("header.csv")
generator.varOrderNames()
# ('C', 'A', 'D', 'B')
