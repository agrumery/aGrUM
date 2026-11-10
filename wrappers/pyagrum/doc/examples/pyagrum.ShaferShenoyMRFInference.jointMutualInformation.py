import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.factor({"A", "B"}).fillWith([4, 1, 1, 2])
mrf.factor({"B", "C"}).fillWith([1, 3, 2, 1])
ie = gum.ShaferShenoyMRFInference(mrf)
round(ie.jointMutualInformation(["A", "B"]), 4)  # the mutual information I(A;B)
# 0.1659
round(ie.jointMutualInformation(["A", "B", "C"]), 4)
# -0.0278
