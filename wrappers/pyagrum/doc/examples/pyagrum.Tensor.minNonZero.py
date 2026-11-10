import pyagrum as gum

a = gum.LabelizedVariable("a", "a", 3)
p = gum.Tensor().add(a).fillWith([0.6, 0, 0.4])
p.minNonZero()
# 0.4
