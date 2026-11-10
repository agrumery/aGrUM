import pyagrum as gum

mrf = gum.fastMRF("A--B")
mrf.factor(["A", "B"]).fillWith([0, 0.5, 2, 1])
mrf.minNonZeroParam()
# 0.5
