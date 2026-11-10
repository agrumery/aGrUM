import pyagrum as gum

a, b = gum.LabelizedVariable("a", "a", 2), gum.LabelizedVariable("b", "b", 3)
i = gum.Instantiation()
i.add(a)
i.add(b)
j = gum.Instantiation()
j.add(a)
i.incOut(j)  # only the variables not in j (here b) are incremented
print(i)
# <a:0|b:1>
