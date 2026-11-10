import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C;C--D")
ie = gum.ShaferShenoyMRFInference(mrf)
ie.addJointTarget({"A", "C"})
ie.isJointTarget({"A", "C"})
# True
ie.isJointTarget({"B", "D"})
# False
