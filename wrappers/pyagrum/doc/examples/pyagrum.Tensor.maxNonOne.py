import pyagrum as gum

a = gum.LabelizedVariable("a", "a", 3)
p = gum.Tensor().add(a).fillWith([0.2, 1, 0.7])
p.maxNonOne()
# 0.7
