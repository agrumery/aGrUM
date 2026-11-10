import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.sizeEdges()
# 2
