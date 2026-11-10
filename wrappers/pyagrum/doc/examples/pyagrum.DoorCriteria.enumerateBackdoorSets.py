import pyagrum as gum

bn = gum.fastBN("Z1->X->Y;Z1->Z2->Y;Z3->X;Z3->Y")
dag = bn.dag()
sets = gum.DoorCriteria.enumerateBackdoorSets(dag, bn.idFromName("X"), bn.idFromName("Y"))
sorted(sorted(bn.variable(i).name() for i in s) for s in sets)
# [['Z1', 'Z3'], ['Z2', 'Z3']]
