import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1"))
model.cpt("X", 1).fillWith([0.9, 0.1, 0.2, 0.8])
print(model.cpt("X[1]"))
#
#       ║  X[1]             │
# X[0]  ║0        │1        │
# ──────║─────────│─────────│
# 0     ║ 0.9000  │ 0.1000  │
# 1     ║ 0.2000  │ 0.8000  │
