import pyagrum as gum

i = gum.Instantiation()
i.empty()
# True
i.add(gum.LabelizedVariable("a", "a", 2))
i.empty()
# False
