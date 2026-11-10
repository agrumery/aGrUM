import pyagrum as gum

a = gum.LabelizedVariable("a", "a", 3)
p = gum.Tensor().add(a).fillWith([-1, 0, 2])
p.abs()  # in place
p.tolist()
# [1.0, 0.0, 2.0]
