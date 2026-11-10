import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B").fillWith([0.9, 0.1, 0.3, 0.7])
bn.cpt("C").fillWith([0.6, 0.4, 0.1, 0.9])
ie = gum.VariableElimination(bn)
ie.addJointTarget({"A", "C"})
ie.jointTargets()
# [{0, 2}]
print(ie.jointPosterior({"A", "C"}).reorganize(["A", "C"]))  # fixed order of the variables
#
#       ║  A                │
# C     ║0        │1        │
# ──────║─────────│─────────│
# 0     ║ 0.1100  │ 0.2000  │
# 1     ║ 0.0900  │ 0.6000  │
