import pyagrum as gum

bn = gum.fastBN("A->B->C;D")
sorted(v.name() for v in bn.variables())
# ['A', 'B', 'C', 'D']
sorted(v.name() for v in bn.variables(["A", "C"]))
# ['A', 'C']
sorted(v.name() for v in bn.variables({bn.idFromName("B")}))
# ['B']
