import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.hasSameStructure(gum.fastMRF("A--B;B--C"))
# True
mrf.hasSameStructure(gum.fastMRF("A--B--C"))
# False
