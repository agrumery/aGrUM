import pyagrum as gum

a, b = gum.LabelizedVariable("a", "a", 2), gum.LabelizedVariable("b", "b", 3)
i = gum.Instantiation()
i.add(a)
i.add(b)
[j.todict() for j in i.loopIn()][:3]
# [{'a': 0, 'b': 0}, {'a': 1, 'b': 0}, {'a': 0, 'b': 1}]
