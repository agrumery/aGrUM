import pyagrum as gum

a, b = gum.LabelizedVariable("a", "a", 2), gum.LabelizedVariable("b", "b", 3)
i = gum.Instantiation()
i.add(a)
i.add(b)
j = gum.Instantiation()
j.add(b)
j.chgVal(b, 2)
i.setVals(j)  # copy the value of b from j
print(i)
# <a:0|b:2>
