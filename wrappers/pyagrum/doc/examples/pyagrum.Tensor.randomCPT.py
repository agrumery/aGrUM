import pyagrum as gum

gum.initRandom(42)
a, b = gum.LabelizedVariable("a", "a", 2), gum.LabelizedVariable("b", "b", 3)
p = gum.Tensor().add(a).add(b).randomCPT()  # a distribution over a for each value of b
[round(x, 4) for x in p.sumOut(["a"]).tolist()]
# [1.0, 1.0, 1.0]
