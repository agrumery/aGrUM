import pyagrum as gum

a, b = gum.LabelizedVariable("a", "a", 2), gum.LabelizedVariable("b", "b", 3)
i = gum.Instantiation()
i.add(a)
i.add(b)
j = gum.Instantiation()
j.add(b)
j.add(a)
i.reorder(j)  # the order of the variables in j
print(i)
# <b:0|a:0>
