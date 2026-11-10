import pyagrum as gum
import pyagrum.ktbn as ktbn

bn = gum.fastBN("X0->X1;Y0->Y1;X0->Y1;C->X0")
model = ktbn.KTBN.fromBN(bn)
model.k()
# 2
sorted(model.temporalVarNames())
# ['X', 'Y']
model.atemporalVarNames()
# frozenset({'C'})
ktbn.KTBN.fromBN(model.toBN()).arcs() == model.arcs()  # bracket names
# True
