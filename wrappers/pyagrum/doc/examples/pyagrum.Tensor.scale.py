import pyagrum as gum

a, b = gum.LabelizedVariable("a", "a", 2), gum.LabelizedVariable("b", "b", 3)
p = gum.Tensor().add(a).add(b).fillWith([1, 2, 3, 4, 5, 6])
p.scale(10).tolist()
# [[10.0, 20.0], [30.0, 40.0], [50.0, 60.0]]
