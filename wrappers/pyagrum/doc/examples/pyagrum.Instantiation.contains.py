import pyagrum as gum

a, b = gum.LabelizedVariable("a", "a", 2), gum.LabelizedVariable("b", "b", 3)
i = gum.Instantiation()
i.add(a)
i.add(b)
i.contains(a)
# True
i.contains(gum.LabelizedVariable("c", "c", 2))
# False
