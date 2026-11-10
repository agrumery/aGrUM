import pyagrum as gum

a, b = gum.LabelizedVariable("a", "a", 2), gum.LabelizedVariable("b", "b", 3)
i = gum.Instantiation()
i.add(a)
i.add(b)
i.setLast()
j = gum.Instantiation()
j.add(b)
i.decOut(j)  # only the variables not in j (here a) are decremented
print(i)
# <a:0|b:2>
