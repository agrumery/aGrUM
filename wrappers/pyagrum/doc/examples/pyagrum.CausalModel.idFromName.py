import pyagrum as gum

bn = gum.fastBN("A->B->C")
cm = gum.CausalModel(bn, [("U", ["A", "C"])])
cm.idFromName("B")
# 1
cm.idFromName("U")  # latent variables have an id too
# 3
