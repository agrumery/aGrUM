import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C;C--D")
ie = gum.ShaferShenoyMRFInference(mrf)
ie.addJointTarget({"A", "C"})
ie.addJointTarget({"B", "D"})
ie.jointTargets()
# [{0, 2}, {1, 3}]
