import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C[3]")
mrf.dim()
# 10
