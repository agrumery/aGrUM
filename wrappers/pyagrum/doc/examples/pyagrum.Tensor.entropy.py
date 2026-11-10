import pyagrum as gum

a = gum.LabelizedVariable("a", "a", 4)
p = gum.Tensor().add(a).fillWith([0.25, 0.25, 0.25, 0.25])
p.entropy()
# 2.0
p.fillWith([0.5, 0.5, 0, 0]).entropy()
# 1.0
