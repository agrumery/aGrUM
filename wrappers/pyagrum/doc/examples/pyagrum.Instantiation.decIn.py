import pyagrum as gum

a, b = gum.LabelizedVariable("a", "a", 2), gum.LabelizedVariable("b", "b", 3)
i = gum.Instantiation()
i.add(a)
i.add(b)
i.setLast()
j = gum.Instantiation()
j.add(b)
i.decIn(j)  # only the variables of j (here b) are decremented
print(i)
# <a:1|b:1>
