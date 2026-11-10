import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.eraseFactor(["B", "C"])
sorted(mrf.edges())
# [(0, 1)]
len(mrf.factors())
# 1
