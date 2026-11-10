import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.isIndependent("A", "C", "B")
# True
mrf.isIndependent("A", "C", [])
# False
