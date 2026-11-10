import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C;C--D")
ie = gum.ShaferShenoyMRFInference(mrf)
ie.hasEvidence("A")
# False
ie.setEvidence({"A": 1})
ie.hasEvidence("A")
# True
ie.hasEvidence("C")
# False
