import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.ids(["C", "A"])
# [2, 0]
