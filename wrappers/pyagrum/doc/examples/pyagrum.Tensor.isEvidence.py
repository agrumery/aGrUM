import pyagrum as gum

a, b = gum.LabelizedVariable("a", "a", 2), gum.LabelizedVariable("b", "b", 2)
gum.Tensor().add(a).fillWith([1, 0.3]).isEvidence()
# True
gum.Tensor().add(a).fillWith([2, 1]).isEvidence()
# False
gum.Tensor().add(a).add(b).fillWith(0.5).isEvidence()
# False
