import pyagrum as gum

a, b = gum.LabelizedVariable("a", "a", 2), gum.LabelizedVariable("b", "b", 3)
i = gum.Instantiation()
i.add(a)
i.add(b)
n = 0
i.setFirst()
while not i.end():
  n += 1
  i.inc()
n
# 6
