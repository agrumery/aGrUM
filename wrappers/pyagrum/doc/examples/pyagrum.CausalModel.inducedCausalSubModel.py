import pyagrum as gum

bn = gum.fastBN("Z->X->Y;Z->Y")
cm = gum.CausalModel(bn)
sub = cm.inducedCausalSubModel(cm, {cm.idFromName("X"), cm.idFromName("Y")})
sorted(sub.names())
# ['X', 'Y']
sub.existsArc("X", "Y")
# True
