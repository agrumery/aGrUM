import pyagrum as gum

bn = gum.fastBN("A->B")
bn.cpt("A").fillWith([0.5, 0.5])
bn.cpt("B")[:] = [[0, 1], [1, 0]]  # B is the opposite of A
generator = gum.BNDatabaseGenerator(bn)
ll = generator.drawSamples(4, {"A": 1})
generator.toCSV("samples.csv")
with open("samples.csv") as f:
  content = f.read()
print(content)
# A,B
# 1,0
# 1,0
# 1,0
# 1,0
