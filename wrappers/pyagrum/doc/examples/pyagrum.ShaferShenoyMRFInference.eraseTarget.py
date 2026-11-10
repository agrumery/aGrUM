import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C;C--D")
ie = gum.ShaferShenoyMRFInference(mrf)
ie.addTarget("A")
ie.addTarget("C")
ie.eraseTarget("A")
sorted(ie.targets())
# [2]
