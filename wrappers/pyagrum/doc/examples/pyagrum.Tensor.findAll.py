import pyagrum as gum

a, b = gum.LabelizedVariable("a", "a", 2), gum.LabelizedVariable("b", "b", 3)
p = gum.Tensor().add(a).add(b).fillWith([1, 0, 0, 1, 1, 0])
p.findAll(0)
# [{'a': 1, 'b': 0}, {'a': 0, 'b': 1}, {'a': 1, 'b': 2}]
