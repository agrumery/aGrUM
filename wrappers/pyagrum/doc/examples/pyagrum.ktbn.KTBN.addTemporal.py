import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN(3)
model.addTemporal(gum.LabelizedVariable("X", "X", 2))
model.nodes()  # one node per slice
# (('X', 0), ('X', 1), ('X', 2))
model.temporalVarNames()
# frozenset({'X'})
