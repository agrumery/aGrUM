import pyagrum as gum

bn = gum.fastBN("A->B->C")
cm = gum.CausalModel(bn, [("U", ["A", "C"])])
sorted(cm.names())  # observed and latent variables
# ['A', 'B', 'C', 'U']
