import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN(2)
model.add(gum.LabelizedVariable("X", "X", 2))  # temporal: one node per slice
model.add(gum.LabelizedVariable("C", "C", 2), False)  # atemporal: a single node
model.nodes()
# (('C', -1), ('X', 0), ('X', 1))
