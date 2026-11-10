import pyagrum as gum

a, b = gum.LabelizedVariable("a", "a", 2), gum.LabelizedVariable("b", "b", 3)
p = gum.Tensor().add(a).add(b).fillWith([1, 2, 3, 4, 5, 6])
print(p.variable(0))
# a:Labelized({0|1})
print(p.variable(1))
# b:Labelized({0|1|2})
