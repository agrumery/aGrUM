import pyagrum as gum

gum.initRandom(42)
a = gum.LabelizedVariable("a", "a", 3)
p = gum.Tensor().add(a).fillWith([0.2, 0.5, 0.3])
v = p.draw()  # index of a value of a drawn with probabilities p
v in [0, 1, 2]
# True
