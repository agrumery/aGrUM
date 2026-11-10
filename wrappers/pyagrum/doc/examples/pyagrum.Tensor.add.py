import pyagrum as gum

a, b = gum.LabelizedVariable("a", "a", 2), gum.LabelizedVariable("b", "b", 3)
p = gum.Tensor()
p.add(a).add(b)  # add returns the tensor itself
p.names
# ('a', 'b')
p.domainSize()
# 6
