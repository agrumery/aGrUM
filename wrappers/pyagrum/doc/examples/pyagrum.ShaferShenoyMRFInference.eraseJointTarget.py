import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C;C--D")
ie = gum.ShaferShenoyMRFInference(mrf)
ie.addJointTarget({"A", "C"})
ie.addJointTarget({"B", "D"})
ie.eraseJointTarget({"A", "C"})
ie.jointTargets()
# [{1, 3}]
