import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C;C--D")
ie = gum.ShaferShenoyMRFInference(mrf)
ie.setEvidence({"A": 1, "B": 0, "C": [0.2, 0.9]})
sorted(ie.hardEvidenceNodes())
# [0, 1]
