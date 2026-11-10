import pyagrum as gum

bn = gum.fastBN("A->B->C")
cm = gum.CausalModel(bn, [("U", ["A", "C"])])
obs = cm.observationalBN()
sorted(obs.names())  # the latent U is not in the observational BN
# ['A', 'B', 'C']
