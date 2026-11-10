import pyagrum as gum

a = gum.LabelizedVariable("a", "a", 4)
p = gum.Tensor().add(a).fillWith([1, 2, 3, 4])
p.normalize().tolist()
# [0.1, 0.2, 0.3, 0.4]
