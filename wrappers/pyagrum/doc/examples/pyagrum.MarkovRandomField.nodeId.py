import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.nodeId(mrf.variable("B"))
# 1
