import pyagrum as gum

a, b = gum.LabelizedVariable("a", "a", 2), gum.LabelizedVariable("b", "b", 3)
p = gum.Tensor().add(a).add(b)
p.fillWith(0.5).sum()
# 3.0
p.fillWith([1, 2, 3, 4, 5, 6])  # the first variable (a) varies fastest
print(p)
#
#       ║  a                │
# b     ║0        │1        │
# ──────║─────────│─────────│
# 0     ║ 1.0000  │ 2.0000  │
# 1     ║ 3.0000  │ 4.0000  │
# 2     ║ 5.0000  │ 6.0000  │
