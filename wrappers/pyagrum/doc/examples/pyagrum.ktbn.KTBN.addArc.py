import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN(2)
model.addTemporal(gum.LabelizedVariable("X", "X", 2))
model.addAtemporal(gum.LabelizedVariable("C", "C", 2))
model.addArc("X", 0, "X", 1)
model.addArc("C", ktbn.KTBN.ATEMPORAL, "X", 1)
model.addArc("C", "X[0]")  # with engine names
model.arcs()
# ((('X', 0), ('X', 1)), (('C', -1), ('X', 1)), (('C', -1), ('X', 0)))
