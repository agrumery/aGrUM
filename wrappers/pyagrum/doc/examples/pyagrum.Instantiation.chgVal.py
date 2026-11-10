import pyagrum as gum

a, b = gum.LabelizedVariable("a", "a", 2), gum.LabelizedVariable("b", "b", 3)
i = gum.Instantiation()
i.add(a)
i.add(b)
i.chgVal(b, 2)
print(i)
# <a:0|b:2>
i.chgVal("a", 1)  # by name
print(i)
# <a:1|b:2>
