import pyagrum as gum

bn = gum.fastBN("A->B;C->D")
cm = gum.CausalModel(bn)
cm.connectedComponentsCount()
# 2
cm.addLatentVariable("U", ["B", "D"])  # a latent common cause links both parts
cm.connectedComponentsCount()
# 1
