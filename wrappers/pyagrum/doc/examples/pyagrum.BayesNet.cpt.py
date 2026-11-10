import pyagrum as gum

bn = gum.fastBN("A->B")
bn.cpt("B").fillWith([0.1, 0.9, 0.7, 0.3])
print(bn.cpt("B"))
#
#       ║  B                │
# A     ║0        │1        │
# ──────║─────────│─────────│
# 0     ║ 0.1000  │ 0.9000  │
# 1     ║ 0.7000  │ 0.3000  │
bn.cpt(0).names  # with the id of A
# ('A',)
