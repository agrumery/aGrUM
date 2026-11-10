import pyagrum as gum

mrf = gum.fastMRF("A--B[5];B--C[3]")
mrf.maxVarDomainSize()
# 5
