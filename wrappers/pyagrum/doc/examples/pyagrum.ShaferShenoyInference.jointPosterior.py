import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B").fillWith([0.9, 0.1, 0.3, 0.7])
bn.cpt("C").fillWith([0.6, 0.4, 0.1, 0.9])
ie = gum.ShaferShenoyInference(bn)
ie.addJointTarget({"A", "B"})
ie.setEvidence({"C": 1})
print(ie.jointPosterior({"A", "B"}).reorganize(["A", "B"]))  # fixed order of the variables
#
#       ║  A                │
# B     ║0        │1        │
# ──────║─────────│─────────│
# 0     ║ 0.1043  │ 0.1391  │
# 1     ║ 0.0261  │ 0.7304  │
