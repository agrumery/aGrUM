import pyagrum as gum

a, b = gum.LabelizedVariable("a", "a", 2), gum.LabelizedVariable("b", "b", 3)
p = gum.Tensor().add(a).add(b).fillWith([1, 2, 3, 4, 5, 6])
p.translate(10).tolist()
# [[11.0, 12.0], [13.0, 14.0], [15.0, 16.0]]
