import pyagrum as gum

gum.initRandom(42)
a = gum.LabelizedVariable("a", "a", 5)
p = gum.Tensor().add(a).randomDistribution()
round(p.sum(), 4)
# 1.0
