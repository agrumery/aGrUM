import pyagrum as gum

a = gum.LabelizedVariable("a", "a", 3)
p = gum.Tensor().add(a).fillWith([-3, 0, 2])
p.sq()  # in place
p.tolist()
# [9.0, 0.0, 4.0]
