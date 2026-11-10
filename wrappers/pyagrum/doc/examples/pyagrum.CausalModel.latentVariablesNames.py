import pyagrum as gum

bn = gum.fastBN("A->B->C;C->D")
cm = gum.CausalModel(bn, [("U1", ["A", "C"]), ("U2", ["B", "D"])])
sorted(cm.latentVariablesNames())
# ['U1', 'U2']
