import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.factor({"A", "B"}).fillWith([4, 1, 1, 2])
mrf.factor({"B", "C"}).fillWith([1, 3, 2, 1])
ie = gum.ShaferShenoyMRFInference(mrf)
round(ie.H("C"), 4)
# 0.999
ie.setEvidence({"A": 1})
round(ie.H("C"), 4)  # entropy of C given the evidence
# 0.9457
