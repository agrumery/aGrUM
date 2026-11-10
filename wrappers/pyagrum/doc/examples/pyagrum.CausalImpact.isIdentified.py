import pyagrum as gum

bn = gum.fastBN("X->Y")
cm = gum.CausalModel(bn, [("U", ["X", "Y"])], True)
gum.CausalImpact(cm, on="Y", doing="X").isIdentified()  # confounded by the latent U
# False
cm2 = gum.CausalModel(bn)
gum.CausalImpact(cm2, on="Y", doing="X").isIdentified()
# True
