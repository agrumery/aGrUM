import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.factor({"A", "B"}).fillWith([4, 1, 1, 2])
mrf.factor({"B", "C"}).fillWith([1, 3, 2, 1])
ie = gum.ShaferShenoyMRFInference(mrf)
# P(A, B | C) for every value of C
print(ie.evidenceJointImpact(["A", "B"], {"C"}))
#
#              ║  B                │
# A     │C     ║0        │1        │
# ──────│──────║─────────│─────────│
# 0     │0     ║ 0.2857  │ 0.2143  │
# 1     │0     ║ 0.0714  │ 0.4286  │
# 0     │1     ║ 0.6154  │ 0.0769  │
# 1     │1     ║ 0.1538  │ 0.1538  │
