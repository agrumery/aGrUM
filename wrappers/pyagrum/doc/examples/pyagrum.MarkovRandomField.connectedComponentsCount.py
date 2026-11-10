import pyagrum as gum

mrf = gum.fastMRF("A--B;C--D;E")
mrf.connectedComponentsCount()
# 3
