import pyagrum as gum

a, b = gum.LabelizedVariable("a", "a", 2), gum.LabelizedVariable("b", "b", 3)
i = gum.Instantiation()
i.add(a)
i.add(b)
i.chgVal(b, 2)
i.val(b)
# 2
i.val(1)  # by position
# 2
i["b"]  # same thing
# 2
