import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.existsEdge("A", "B")
# True
mrf.existsEdge("A", "C")
# False
