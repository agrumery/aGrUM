import pyagrum as gum

mrf = gum.fastMRF("A--B--C;C--D")
mrf.idsFromNames(["D", "A"])
# [3, 0]
