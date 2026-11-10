import pyagrum as gum

x = gum.fastVariable("x{1|2|4}")
p = gum.Tensor().add(x).fillWith([0.2, 0.5, 0.3])
p.expectedValue(lambda d: x.numerical(d["x"]))  # E[x]
# 2.4
