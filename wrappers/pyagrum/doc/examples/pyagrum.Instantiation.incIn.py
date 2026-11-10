import pyagrum as gum

a, b = gum.LabelizedVariable("a", "a", 2), gum.LabelizedVariable("b", "b", 3)
i = gum.Instantiation()
i.add(a)
i.add(b)
j = gum.Instantiation()
j.add(b)
i.incIn(j)  # only the variables of j (here b) are incremented
print(i)
# <a:0|b:1>
