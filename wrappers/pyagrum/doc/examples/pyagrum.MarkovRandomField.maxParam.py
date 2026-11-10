import pyagrum as gum

mrf = gum.fastMRF("A--B")
mrf.factor(["A", "B"]).fillWith([0.5, 1, 2, 0.8])
mrf.maxParam()
# 2.0
