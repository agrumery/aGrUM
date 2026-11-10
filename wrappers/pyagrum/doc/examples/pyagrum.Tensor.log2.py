import pyagrum as gum

a = gum.LabelizedVariable("a", "a", 3)
p = gum.Tensor().add(a).fillWith([1, 2, 8])
p.log2()  # in place
p.tolist()
# [0.0, 1.0, 3.0]
