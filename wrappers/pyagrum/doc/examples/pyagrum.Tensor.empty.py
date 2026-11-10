import pyagrum as gum

p = gum.Tensor()
p.empty()
# True
p.add(gum.LabelizedVariable("a", "a", 2))
p.empty()
# False
