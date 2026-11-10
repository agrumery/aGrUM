import pyagrum as gum

a, b = gum.LabelizedVariable("a", "a", 2), gum.LabelizedVariable("b", "b", 3)
p = gum.Tensor().add(a).add(b).fillWith([1, 2, 3, 4, 5, 6])
v = p.as_nparray()  # a view sharing the data of p
v.shape
# (3, 2)
v[0, 0] = 10
p[{"a": 0, "b": 0}]
# 10.0
