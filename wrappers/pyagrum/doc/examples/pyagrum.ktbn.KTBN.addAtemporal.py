import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN(3)
model.addAtemporal(gum.LabelizedVariable("C", "C", 2))
model.nodes()
# (('C', -1),)
model.atemporalVarNames()
# frozenset({'C'})
