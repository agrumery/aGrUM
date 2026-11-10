import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C;C--D")
ie = gum.ShaferShenoyMRFInference(mrf)
# with no target declared, every node is a target
sorted(ie.targets())
# [0, 1, 2, 3]
ie.addTarget("C")
sorted(ie.targets())
# [2]
