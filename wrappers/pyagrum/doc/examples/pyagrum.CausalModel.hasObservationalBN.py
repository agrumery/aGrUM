import pyagrum as gum

cm = gum.CausalModel(gum.fastBN("A->B"))
cm.hasObservationalBN()
# True
