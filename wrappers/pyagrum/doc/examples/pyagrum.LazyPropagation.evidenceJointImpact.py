import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B").fillWith([0.9, 0.1, 0.3, 0.7])
bn.cpt("C").fillWith([0.6, 0.4, 0.1, 0.9])
ie = gum.LazyPropagation(bn)
# P(A, B | C) for every value of C
print(ie.evidenceJointImpact(["A", "B"], {"C"}))
#
#              ║  B                │
# A     │C     ║0        │1        │
# ──────│──────║─────────│─────────│
# 0     │0     ║ 0.3484  │ 0.0065  │
# 1     │0     ║ 0.4645  │ 0.1806  │
# 0     │1     ║ 0.1043  │ 0.0261  │
# 1     │1     ║ 0.1391  │ 0.7304  │
