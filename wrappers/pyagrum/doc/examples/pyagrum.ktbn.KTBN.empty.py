import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN(2)
model.empty()
# True
model.addTemporal(gum.LabelizedVariable("X", "X", 2))
model.empty()
# False
