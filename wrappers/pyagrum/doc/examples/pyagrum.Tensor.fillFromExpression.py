import pyagrum as gum

bn = gum.fastBN("A[3]->B[3]")
bn.cpt("B").fillFromExpression("A*B")
print(bn.cpt("B"))
#
#       ║  B                          │
# A     ║0        │1        │2        │
# ──────║─────────│─────────│─────────│
# 0     ║ 0.0000  │ 0.0000  │ 0.0000  │
# 1     ║ 0.0000  │ 1.0000  │ 2.0000  │
# 2     ║ 0.0000  │ 2.0000  │ 4.0000  │
