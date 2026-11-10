import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C;C--D")
ie = gum.ShaferShenoyMRFInference(mrf)
ie.setEvidence({"A": 1, "B": [0.5, 1], "C": [0.2, 0.9]})
ie.nbrSoftEvidence()
# 2
