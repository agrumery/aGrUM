import pyagrum as gum

a, b = gum.LabelizedVariable("a", "a", 2), gum.LabelizedVariable("b", "b", 3)
i = gum.Instantiation()
i.add(a)
i.add(b)
i.chgVal(a, 1)
i.chgVal(b, 2)
i.hamming()  # sum of the values
# 3
