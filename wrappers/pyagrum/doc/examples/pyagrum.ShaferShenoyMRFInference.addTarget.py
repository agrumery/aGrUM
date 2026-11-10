import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C;C--D")
ie = gum.ShaferShenoyMRFInference(mrf)
ie.addTarget("B")
sorted(ie.targets())
# [1]
ie.isTarget("A")
# False
