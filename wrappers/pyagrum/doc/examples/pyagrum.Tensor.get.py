import pyagrum as gum

a, b = gum.LabelizedVariable("a", "a", 2), gum.LabelizedVariable("b", "b", 3)
p = gum.Tensor().add(a).add(b).fillWith([1, 2, 3, 4, 5, 6])
i = gum.Instantiation(p)
i.chgVal(a, 1)
p.get(i)
# 2.0
i.chgVal(b, 2)
p.get(i)
# 6.0
