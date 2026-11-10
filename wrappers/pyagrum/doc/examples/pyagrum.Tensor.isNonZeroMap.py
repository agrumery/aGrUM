import pyagrum as gum

a = gum.LabelizedVariable("a", "a", 3)
p = gum.Tensor().add(a).fillWith([0.2, 0, 0.8])
p.isNonZeroMap().tolist()
# [1.0, 0.0, 1.0]
