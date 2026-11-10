import pyagrum as gum

a, b = gum.LabelizedVariable("a", "a", 2), gum.LabelizedVariable("b", "b", 3)
p = gum.Tensor().add(a).add(b).fillWith([1, 2, 3, 4, 5, 6])
i = gum.Instantiation(p)
i.chgVal(b, 2)
p.set(i, 0)
p.tolist()
# [[1.0, 2.0], [3.0, 4.0], [0.0, 6.0]]
