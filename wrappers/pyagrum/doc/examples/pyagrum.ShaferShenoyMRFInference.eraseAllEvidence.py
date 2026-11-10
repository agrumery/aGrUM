import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.factor({"A", "B"}).fillWith([4, 1, 1, 2])
mrf.factor({"B", "C"}).fillWith([1, 3, 2, 1])
ie = gum.ShaferShenoyMRFInference(mrf)
ie.setEvidence({"A": 1, "C": 0})
ie.nbrEvidence()
# 2
ie.eraseAllEvidence()
ie.nbrEvidence()
# 0
