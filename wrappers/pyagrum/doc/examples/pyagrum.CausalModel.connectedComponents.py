import pyagrum as gum

bn = gum.fastBN("A->B;C->D")
cm = gum.CausalModel(bn)
cc = cm.connectedComponents()
cc[cm.idFromName("A")] == cc[cm.idFromName("B")]
# True
cc[cm.idFromName("A")] == cc[cm.idFromName("C")]
# False
