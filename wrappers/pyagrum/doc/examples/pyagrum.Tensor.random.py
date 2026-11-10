import pyagrum as gum

gum.initRandom(42)
a = gum.LabelizedVariable("a", "a", 5)
p = gum.Tensor().add(a).random()
all(0 <= x <= 1 for x in p.tolist())
# True
