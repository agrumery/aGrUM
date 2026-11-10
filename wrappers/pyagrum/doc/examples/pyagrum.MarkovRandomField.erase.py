import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.erase("B")
sorted(mrf.names())
# ['A', 'C']
mrf.sizeEdges()
# 0
