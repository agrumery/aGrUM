import pyagrum as gum

a, b = gum.LabelizedVariable("a", "a", 2), gum.LabelizedVariable("b", "b", 3)
i = gum.Instantiation()
i.add(a)
i.add(b)
i.setLast()
print(i)
# <a:1|b:2>
i.dec()
print(i)
# <a:0|b:2>
