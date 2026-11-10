import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C;C--D")
ie = gum.ShaferShenoyMRFInference(mrf)
ie.setEvidence({"A": 1, "C": 0})
ie.eraseEvidence("A")
ie.hasEvidence("A")
# False
ie.hasEvidence("C")
# True
