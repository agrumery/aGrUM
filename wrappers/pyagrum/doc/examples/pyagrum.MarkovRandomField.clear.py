import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.clear()
mrf.size(), mrf.empty()
# (0, True)
