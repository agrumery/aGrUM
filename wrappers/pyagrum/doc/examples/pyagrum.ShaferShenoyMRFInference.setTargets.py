import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C;C--D")
ie = gum.ShaferShenoyMRFInference(mrf)
ie.addTarget("B")
ie.setTargets({"A", "C"})
sorted(ie.targets())
# [0, 2]
