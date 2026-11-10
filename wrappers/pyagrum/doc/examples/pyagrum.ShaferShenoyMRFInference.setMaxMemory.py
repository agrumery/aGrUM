import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.factor({"A", "B"}).fillWith([4, 1, 1, 2])
mrf.factor({"B", "C"}).fillWith([1, 3, 2, 1])
ie = gum.ShaferShenoyMRFInference(mrf)
ie.setMaxMemory(1)  # at most 1 gigabyte for the inference
ie.makeInference()
print(ie.posterior("C"))
#
#   C                │
# 0        │1        │
# ─────────│─────────│
#  0.5185  │ 0.4815  │
