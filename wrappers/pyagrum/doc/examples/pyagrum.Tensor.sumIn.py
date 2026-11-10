import pyagrum as gum

a, b = gum.LabelizedVariable("a", "a", 2), gum.LabelizedVariable("b", "b", 3)
p = gum.Tensor().add(a).add(b).fillWith([1, 2, 3, 4, 5, 6])
p.sumIn(["b"]).tolist()
# [3.0, 7.0, 11.0]
