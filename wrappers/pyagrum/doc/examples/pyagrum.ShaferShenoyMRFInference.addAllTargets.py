import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C;C--D")
ie = gum.ShaferShenoyMRFInference(mrf)
ie.addTarget("A")
sorted(ie.targets())
# [0]
ie.addAllTargets()
sorted(ie.targets())
# [0, 1, 2, 3]
