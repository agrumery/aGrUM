import pyagrum as gum

a = gum.LabelizedVariable("a", "a", 3)
p = gum.Tensor().add(a).fillWith([1, 2, 4])
p.inverse()  # in place
p.tolist()
# [1.0, 0.5, 0.25]
