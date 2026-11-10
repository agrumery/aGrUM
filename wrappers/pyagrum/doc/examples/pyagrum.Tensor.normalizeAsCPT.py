import pyagrum as gum

a, b = gum.LabelizedVariable("a", "a", 2), gum.LabelizedVariable("b", "b", 3)
p = gum.Tensor().add(a).add(b).fillWith([1, 2, 3, 4, 5, 6])
p.putFirst("b")  # the first variable is the conditioned one
p.normalizeAsCPT()
print(p)
#
#       ║  a                │
# b     ║0        │1        │
# ──────║─────────│─────────│
# 0     ║ 0.3333  │ 0.6667  │
# 1     ║ 0.4286  │ 0.5714  │
# 2     ║ 0.4545  │ 0.5455  │
