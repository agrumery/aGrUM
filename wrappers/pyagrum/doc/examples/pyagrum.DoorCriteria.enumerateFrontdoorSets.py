import pyagrum as gum

bn = gum.fastBN("U->X->M->Y;U->Y")
dag = bn.dag()
sets = gum.DoorCriteria.enumerateFrontdoorSets(dag, bn.idFromName("X"), bn.idFromName("Y"))
[sorted(bn.variable(i).name() for i in s) for s in sets]
# [['M']]
