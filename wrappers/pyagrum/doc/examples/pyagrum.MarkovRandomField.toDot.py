import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
s = mrf.toDot()
s.startswith("graph")
# True
