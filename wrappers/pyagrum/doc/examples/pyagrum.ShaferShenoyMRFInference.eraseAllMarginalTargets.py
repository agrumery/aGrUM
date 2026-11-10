import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C;C--D")
ie = gum.ShaferShenoyMRFInference(mrf)
ie.addTarget("A")
ie.addJointTarget({"B", "D"})
ie.eraseAllMarginalTargets()
ie.nbrTargets()
# 0
ie.nbrJointTargets()  # the joint targets are kept
# 1
