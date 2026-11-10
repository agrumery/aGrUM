import pyagrum as gum

a = gum.LabelizedVariable("a", "a", 2)
p = gum.Tensor().add(a).fillWith([0.5, 0.5])
q = gum.Tensor().add(a).fillWith([0.9, 0.1])
round(p.KL(q), 4)
# 0.737
p.KL(p)
# 0.0
