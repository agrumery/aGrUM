import pyagrum as gum

a = gum.LabelizedVariable("a", "a", 3)
p = gum.Tensor().add(a).fillWith([-3, 0, 2])
p.sgn()  # in place
p.tolist()
# [-1.0, 0.0, 1.0]
